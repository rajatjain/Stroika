/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_AIX
#include    <libperfstat.h>
#endif

#include    "../../../Foundation/Characters/FloatConversion.h"
#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Int.h"
#include    "../../../Foundation/Configuration/SystemConfiguration.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/Containers/Set.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include    "../../../Foundation/Debug/Trace.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#include    "../../../Foundation/Execution/ErrNoException.h"
#include    "../../../Foundation/Execution/ProcessRunner.h"
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/IO/FileSystem/FileInputStream.h"
#include    "../../../Foundation/Streams/InputStream.h"
#include    "../../../Foundation/Streams/MemoryStream.h"
#include    "../../../Foundation/Streams/TextReader.h"

#include    "Memory.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::Memory;


using   Characters::Character;
using   Characters::String_Constant;
using   Containers::Mapping;
using   Containers::Sequence;
using   Containers::Set;
using   IO::FileSystem::FileInputStream;
using   Time::DurationSecondsType;




// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1




#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_       qPlatform_Windows
#endif


#if     qUseWMICollectionSupport_
#include    "../Support/WMICollector.h"

using   SystemPerformance::Support::WMICollector;
#endif




#if     qUseWMICollectionSupport_
namespace {
    const   String_Constant     kInstanceName_      { L"_Total" };

    const   String_Constant     kCommittedBytes_    { L"Committed Bytes" };
    const   String_Constant     kCommitLimit_       { L"Commit Limit" };
    const   String_Constant     kPagesPerSec_       { L"Pages/sec" };           // hard page faults/sec
    const   String_Constant     kFreeMem_           { L"Free & Zero Page List Bytes" };
}
#endif








namespace {
    struct  CapturerWithContext_COMMON_ {
        Options                 fOptions_;
        DurationSecondsType     fMinimumAveragingInterval_;
        DurationSecondsType     fPostponeCaptureUntil_ { 0 };
        DurationSecondsType     fLastCapturedAt_ {};
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }
        DurationSecondsType    GetLastCaptureAt () const { return fLastCapturedAt_; }
        void    NoteCompletedCapture_ ()
        {
            auto now = Time::GetTickCount ();
            fPostponeCaptureUntil_ = now + fMinimumAveragingInterval_;
            fLastCapturedAt_ = now;
        }
    };
}







#if     qPlatform_AIX
namespace {
    struct  CapturerWithContext_AIX_ : CapturerWithContext_COMMON_ {
        Time::DurationSecondsType   fSaved_VMPageStats_At {};
        uint64_t                    fSaved_MinorPageFaultsSinceBoot {};
        uint64_t                    fSaved_MajorPageFaultsSinceBoot {};
        uint64_t                    fSaved_PageOutsSinceBoot {};

        CapturerWithContext_AIX_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of  updating aved_MajorPageFaultsSinc etc
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
        CapturerWithContext_AIX_ (const CapturerWithContext_AIX_&) = default;   // copy by value fine - no need to re-wait...

        Instruments::Memory::Info capture_ ()
        {
            Instruments::Memory::Info   result;
            result = capture_perfstat_ ();
            NoteCompletedCapture_ ();
            return result;
        }
        Instruments::Memory::Info capture_perfstat_ ()
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("capture_perfstat_");
#endif
            Instruments::Memory::Info   result;

            perfstat_memory_total_t memResults;
            Execution::ThrowErrNoIfNegative (::perfstat_memory_total (nullptr, &memResults,  sizeof (memResults), 1));

            /*
             * From /usr/include/libperfstat.h:
             *      u_longlong_t real_free;     /* free real memory (in 4KB pages)
             *      u_longlong_t real_avail     - number of pages (in 4KB pages) of memory available without paging out working segments
             *      u_longlong_t real_inuse;    * real memory which is in use (in 4KB pages)
             *      u_longlong_t pgsp_total;    * total paging space (in 4KB pages)
             *      u_longlong_t pgsp_free;     * free paging space (in 4KB pages)
             *      u_longlong_t virt_active;   Active virtual pages. Virtual pages are considered active if they have been accessed
             *      u_longlong_t virt_total;    * total virtual memory (in 4KB pages)
             *
             *      u_longlong_t pgins;         number of pages paged in
             *      u_longlong_t pgouts;        number of pages paged out
             *      u_longlong_t pgspins;       number of page ins from paging space
             *      u_longlong_t pgspouts;      number of page outs from paging space
             *
             *  Empirically, and logically from the (vague) definitions (perfstat_memory_total_t), real_total  = real_inuse + real_free
             *
             *  Some assorted unused stats from
             *          /usr/include/libperfstat.h:
             *
             *      u_longlong_t real_system;    number of pages used by system segments.
             *                                  * This is the sum of all the used pages in segment marked for system usage.
             *                                  * Since segment classifications are not always guaranteed to be accurate,
             *                                  * This number is only an approximation.
             *      u_longlong_t real_user;         * number of pages used by non-system segments.
             *                                  * This is the sum of all pages used in segments not marked for system usage.
             *                                  * Since segment classifications are not always guaranteed to be accurate,
             *                                  * This number is only an approximation.
             *      u_longlong_t real_process;  * number of pages used by process segments.
             *      u_longlong_t scans;         * number of page scans by clock
             *      u_longlong_t cycles;        * number of page replacement cycles
             *      u_longlong_t pgsteals;      * number of page steals
             *      u_longlong_t numperm;       * number of frames used for files (in 4KB pages)
             */
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            // [---MAIN---][0000.007]       real_total=983040
            // [---MAIN---][0000.007]       real_free=280802
            // [---MAIN---][0000.007]       real_inuse=702238
            // [---MAIN---][0000.007]       real_pinned=237141      *not used*????
            // [---MAIN---][0000.007]       real_avail=606596
            // [---MAIN---][0000.007]       real_user=472485        *not used*
            // [---MAIN---][0000.007]       real_system=195546      *not used*
            // [---MAIN---][0000.007]       real_process=148875     *not used*
            // [---MAIN---][0000.007]       virt_active=332889
            // [---MAIN---][0000.007]       virt_total=1998848
            DbgTrace ("real_total=%lld", memResults.real_total);
            DbgTrace ("real_free=%lld", memResults.real_free);
            DbgTrace ("real_inuse=%lld", memResults.real_inuse);
            DbgTrace ("real_pinned=%lld", memResults.real_pinned);
            DbgTrace ("real_avail=%lld", memResults.real_avail);
            DbgTrace ("real_user=%lld", memResults.real_user);
            DbgTrace ("real_system=%lld", memResults.real_system);
            DbgTrace ("real_process=%lld", memResults.real_process);
            DbgTrace ("virt_active=%lld", memResults.virt_active);
            DbgTrace ("virt_total=%lld", memResults.virt_total);
#endif

            result.fPhysicalMemory.fFree = memResults.real_free * 4 * 1024;

            /*
             *  Pinned pages cannot be paged out.
             *      https://www-01.ibm.com/support/knowledgecenter/ssw_aix_71/com.ibm.aix.performance/support_pinned_mem.htm
             *          "Pinning a memory region prohibits the pager from stealing pages from the pages backing the pinned memory region"
             *
             *  What we want to call active is really LARGER than this, but this is at least an estimate of actively in use memory.
             *  (FOR NOW IGNORE ABOVE BUT LATER WE MAY WANT TO FACTOR 'pinned' in - maybe assuring activePhysRam at least as much
             *  as pinned?)
             *
             *  SINCE we know:
             *      real_total  = real_inuse + real_free;
             *      since we define RAMTOTAL = INACTIVE + ACTIVE + FREE (roughtly)
             *  that implies that
             *      what we call "ACTIVE + INACTIVE = real_inuse";
             *
             *  It APPEARS 'real_avail = real_free + what I call INACTIVE'
             *  SO:
             *      INACIVE = is real_avail - real_free
             *      ACTIVE= (real_inuse - (real_avail-real_free));
             */
            result.fPhysicalMemory.fActive = (memResults.real_inuse - memResults.real_avail + memResults.real_free) * 4 * 1024;
            result.fPhysicalMemory.fInactive = (memResults.real_avail - memResults.real_free) * 4 * 1024;

            /*
             *  This is our best estimate of what is available. On LINUX, we can also add in 'SReclaimable' - kernel RAM
             *  we could use if needed.
             */
            result.fPhysicalMemory.fAvailable = memResults.real_avail * 4 * 1024;

            /*
             *  This number (virt_active) by nmon to summarize virtual memory status. But very little else...
             *
             *  Tried (memResults.real_inuse + (memResults.pgsp_total - memResults.pgsp_free)) * 4 * 1024;
             *  but that didn't produce a good/representative answer.
             */
            result.fVirtualMemory.fCommittedBytes = (memResults.virt_active) * 4 * 1024;

            result.fVirtualMemory.fCommitLimit = memResults.virt_total * 4 * 1024;

            result.fVirtualMemory.fPagefileTotalSize  = memResults.pgsp_total * 4 * 1024;

            result.fPaging.fMinorPageFaultsSinceBoot = memResults.pgins - memResults.pgspins;
            result.fPaging.fMajorPageFaultsSinceBoot = memResults.pgspins;
            result.fPaging.fPageOutsSinceBoot = memResults.pgouts;

            Time::DurationSecondsType   now = Time::GetTickCount ();
            if (result.fPaging.fMinorPageFaultsSinceBoot) {
                if (fSaved_VMPageStats_At != 0) {
                    result.fPaging.fMinorPageFaultsPerSecond = (*result.fPaging.fMinorPageFaultsSinceBoot - fSaved_MinorPageFaultsSinceBoot) / (now - fSaved_VMPageStats_At);
                }
                fSaved_MinorPageFaultsSinceBoot = *result.fPaging.fMinorPageFaultsSinceBoot;
            }
            {
                if (fSaved_VMPageStats_At != 0) {
                    result.fPaging.fPageOutsPerSecond = (memResults.pgouts - fSaved_PageOutsSinceBoot) / (now - fSaved_VMPageStats_At);
                }
                fSaved_PageOutsSinceBoot = memResults.pgouts;
            }
            if (result.fPaging.fMajorPageFaultsSinceBoot) {
                if (fSaved_VMPageStats_At != 0) {
                    result.fPaging.fMajorPageFaultsPerSecond = (*result.fPaging.fMajorPageFaultsSinceBoot - fSaved_MajorPageFaultsSinceBoot) / (now - fSaved_VMPageStats_At);
                }
                fSaved_MajorPageFaultsSinceBoot = *result.fPaging.fMajorPageFaultsSinceBoot;
            }
            if (result.fPaging.fMinorPageFaultsSinceBoot) {
                if (fSaved_VMPageStats_At != 0) {
                    result.fPaging.fMinorPageFaultsPerSecond = (*result.fPaging.fMinorPageFaultsSinceBoot - fSaved_MinorPageFaultsSinceBoot) / (now - fSaved_VMPageStats_At);
                }
                fSaved_MinorPageFaultsSinceBoot = *result.fPaging.fMinorPageFaultsSinceBoot;
            }
            fSaved_VMPageStats_At = now;

            return result;
        }
        Instruments::Memory::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
    };
}
#endif






#if     qPlatform_Linux
namespace {
    struct  CapturerWithContext_Linux_ : CapturerWithContext_COMMON_ {
        uint64_t                    fSaved_MajorPageFaultsSinceBoot {};
        uint64_t                    fSaved_PageOutsSinceBoot {};
        Time::DurationSecondsType   fSaved_VMPageStats_At {};

        CapturerWithContext_Linux_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of  updating aved_MajorPageFaultsSinc etc
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
        CapturerWithContext_Linux_ (const CapturerWithContext_Linux_&) = default;   // copy by value fine - no need to re-wait...

        Instruments::Memory::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Instruments::Memory::Info capture_ ()
        {
            Instruments::Memory::Info   result;
            Read_ProcMemInfo (&result);
            Read_ProcVMStat_ (&result);
            NoteCompletedCapture_ ();
            return result;
        }
        void    Read_ProcMemInfo (Instruments::Memory::Info* updateResult)
        {
            auto    ReadMemInfoLine_  = [] (Optional<uint64_t>* result, const String & n, const Sequence<String>& line) {
                if (line.size () >= 3 and line[0] == n) {
                    String  unit = line[2];
                    double  factor = (unit == L"kB") ? 1024 : 1;
                    *result = static_cast<uint64_t> (round (Characters::String2Float<double> (line[1]) * factor));
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"Set %s = %ld", n.c_str (), static_cast<long> (**result));
#endif
                }
            };
            static  const   String_Constant kProcMemInfoFileName_ { L"/proc/meminfo" };
            //const String_Constant kProcMemInfoFileName_ { L"c:\\Sandbox\\VMSharedFolder\\meminfo" };
            DataExchange::CharacterDelimitedLines::Reader reader {{ ':', ' ', '\t' }};
            // Note - /procfs files always unseekable
            Optional<uint64_t>  memTotal;
            Optional<uint64_t>  slabReclaimable;
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcMemInfoFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                ReadMemInfoLine_ (&memTotal, String_Constant (L"MemTotal"), line);
                ReadMemInfoLine_ (&updateResult->fPhysicalMemory.fFree, String_Constant (L"MemFree"), line);
                ReadMemInfoLine_ (&updateResult->fPhysicalMemory.fAvailable, String_Constant (L"MemAvailable"), line);
                ReadMemInfoLine_ (&updateResult->fPhysicalMemory.fActive, String_Constant (L"Active"), line);
                ReadMemInfoLine_ (&updateResult->fPhysicalMemory.fInactive, String_Constant (L"Inactive"), line);
                ReadMemInfoLine_ (&updateResult->fVirtualMemory.fCommitLimit, String_Constant (L"CommitLimit"), line);
                ReadMemInfoLine_ (&updateResult->fVirtualMemory.fCommittedBytes, String_Constant (L"Committed_AS"), line);
                ReadMemInfoLine_ (&updateResult->fVirtualMemory.fPagefileTotalSize, String_Constant (L"SwapTotal"), line);
                ReadMemInfoLine_ (&slabReclaimable, String_Constant (L"SReclaimable"), line);
            }
            if (memTotal and updateResult->fPhysicalMemory.fFree and updateResult->fPhysicalMemory.fInactive and updateResult->fPhysicalMemory.fActive) {
                updateResult->fPhysicalMemory.fOSReserved = *memTotal - *updateResult->fPhysicalMemory.fFree - *updateResult->fPhysicalMemory.fInactive - *updateResult->fPhysicalMemory.fActive;
            }
            if (updateResult->fPhysicalMemory.fAvailable.IsMissing () and updateResult->fPhysicalMemory.fFree and updateResult->fPhysicalMemory.fInactive) {
                updateResult->fPhysicalMemory.fAvailable = *updateResult->fPhysicalMemory.fFree + *updateResult->fPhysicalMemory.fInactive + slabReclaimable.Value ();
            }
        }
        void    Read_ProcVMStat_ (Instruments::Memory::Info* updateResult)
        {
            auto    ReadVMStatLine_ = [] (Optional<uint64_t>* result, const String & n, const Sequence<String>& line) {
                if (line.size () >= 2 and line[0] == n) {
                    *result = Characters::String2Int<uint64_t> (line[1]);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"Set %s = %ld", n.c_str (), static_cast<long> (**result));
#endif
                }
            };
            {
                static  const   String_Constant kProcVMStatFileName_ { L"/proc/vmstat" };
                Optional<uint64_t>  pgfault;
                Optional<uint64_t>  pgpgout;
                // Note - /procfs files always unseekable
                DataExchange::CharacterDelimitedLines::Reader reader {{ ' ', '\t' }};
                for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcVMStatFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                    ReadVMStatLine_ (&pgfault, String_Constant (L"pgfault"), line);
                    // Unsure if this should be pgpgout or pgpgout, or none of the above. On a system with no swap, I seem to get both happening,
                    // which makes no sense
                    ReadVMStatLine_ (&pgpgout, String_Constant (L"pgpgout"), line);     // tried pgpgout but I dont know what it is but doesnt appear to be pages out - noneof this well documented
                    ReadVMStatLine_ (&updateResult->fPaging.fMajorPageFaultsSinceBoot, String_Constant (L"pgmajfault"), line);
                }
                Time::DurationSecondsType   now = Time::GetTickCount ();
                if (pgpgout) {
                    updateResult->fPaging.fPageOutsSinceBoot = pgpgout;
                    if (fSaved_VMPageStats_At != 0) {
                        updateResult->fPaging.fPageOutsPerSecond = (*updateResult->fPaging.fPageOutsSinceBoot - fSaved_PageOutsSinceBoot) / (now - fSaved_VMPageStats_At);
                    }
                    fSaved_PageOutsSinceBoot = *pgpgout;
                }
                if (pgfault and updateResult->fPaging.fMajorPageFaultsSinceBoot) {
                    updateResult->fPaging.fMinorPageFaultsSinceBoot = *pgfault - *updateResult->fPaging.fMajorPageFaultsSinceBoot;
                }
                if (updateResult->fPaging.fMajorPageFaultsSinceBoot) {
                    if (fSaved_VMPageStats_At != 0) {
                        updateResult->fPaging.fMajorPageFaultsPerSecond = (*updateResult->fPaging.fMajorPageFaultsSinceBoot - fSaved_MajorPageFaultsSinceBoot) / (now - fSaved_VMPageStats_At);
                    }
                    fSaved_MajorPageFaultsSinceBoot = *updateResult->fPaging.fMajorPageFaultsSinceBoot;
                }
                if (updateResult->fPaging.fMinorPageFaultsSinceBoot) {
                    if (fSaved_VMPageStats_At != 0) {
                        updateResult->fPaging.fMinorPageFaultsPerSecond = (*updateResult->fPaging.fMinorPageFaultsSinceBoot - fSaved_MinorPageFaultsSinceBoot) / (now - fSaved_VMPageStats_At);
                    }
                    fSaved_MinorPageFaultsSinceBoot = *updateResult->fPaging.fMinorPageFaultsSinceBoot;
                }
                fSaved_VMPageStats_At = now;
            }
        }
    };
}
#endif





#if     qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ : CapturerWithContext_COMMON_ {
#if     qUseWMICollectionSupport_
        WMICollector            fMemoryWMICollector_ { String_Constant { L"Memory" }, {kInstanceName_},  {kCommittedBytes_, kCommitLimit_, kPagesPerSec_, kFreeMem_ } };
#endif
        CapturerWithContext_Windows_ (const Options& options)
            : CapturerWithContext_COMMON_ (options)
        {
            capture_ ();    // to pre-seed context
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            for (String i : fMemoryWMICollector_.GetAvailableCounters ()) {
                DbgTrace (L"Memory:Countername: %s", i.c_str ());
            }
#endif
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
            : CapturerWithContext_COMMON_ (from)
#if     qUseWMICollectionSupport_
            , fMemoryWMICollector_ (from.fMemoryWMICollector_)
#endif
        {
#if   qUseWMICollectionSupport_
            capture_ ();    // to pre-seed context
#endif
        }

        Instruments::Memory::Info capture_ ()
        {
            Instruments::Memory::Info   result;
            uint64_t                    totalRAM {};
            Read_GlobalMemoryStatusEx_(&result, &totalRAM);
#if     qUseWMICollectionSupport_
            Read_WMI_ (&result, totalRAM);
#endif
            NoteCompletedCapture_ ();
            return result;
        }
        Instruments::Memory::Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        void    Read_GlobalMemoryStatusEx_ (Instruments::Memory::Info* updateResult, uint64_t* totalRAM)
        {
            RequireNotNull (totalRAM);
            MEMORYSTATUSEX statex;
            memset (&statex, 0, sizeof (statex));
            statex.dwLength = sizeof (statex);
            Verify (::GlobalMemoryStatusEx (&statex) != 0);
            //updateResult->fPhysicalMemory.fFree = statex.ullAvailPhys;
            *totalRAM = statex.ullTotalPhys;

            /*
             *  dwMemoryLoad
             *  A number between 0 and 100 that specifies the approximate percentage of physical
             *  memory that is in use (0 indicates no memory use and 100 indicates full memory use)
             */
            updateResult->fPhysicalMemory.fActive = statex.ullTotalPhys * statex.dwMemoryLoad / 100;
        }
#if     qUseWMICollectionSupport_
        void    Read_WMI_ (Instruments::Memory::Info* updateResult, uint64_t totalRAM)
        {
            fMemoryWMICollector_.Collect ();
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommittedBytes_).CopyToIf (&updateResult->fVirtualMemory.fCommittedBytes);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kCommitLimit_).CopyToIf (&updateResult->fVirtualMemory.fCommitLimit);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kPagesPerSec_).CopyToIf (&updateResult->fPaging.fMajorPageFaultsPerSecond);
            fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kFreeMem_).CopyToIf (&updateResult->fPhysicalMemory.fFree);
            if (Optional<double> freeMem = fMemoryWMICollector_.PeekCurrentValue (kInstanceName_, kFreeMem_)) {
                if (updateResult->fPhysicalMemory.fActive) {
                    // Active + Inactive + Free == TotalRAM
                    updateResult->fPhysicalMemory.fInactive = totalRAM - *updateResult->fPhysicalMemory.fActive - static_cast<uint64_t> (*freeMem);
                }
            }
            // WAG TMPHACK - probably should add "hardware in use" memory + private WS of each process + shared memory "WS" - but not easy to compute...
            updateResult->fPhysicalMemory.fAvailable = updateResult->fPhysicalMemory.fFree + updateResult->fPhysicalMemory.fInactive;
        }
#endif
    };
}
#endif





namespace {
    struct  CapturerWithContext_
            : Debug::AssertExternallySynchronizedLock
#if     qPlatform_AIX
            , CapturerWithContext_AIX_
#elif   qPlatform_Linux
            , CapturerWithContext_Linux_
#elif   qPlatform_Windows
            , CapturerWithContext_Windows_
#endif
    {
#if     qPlatform_AIX
        using inherited = CapturerWithContext_AIX_;
#elif   qPlatform_Linux
        using inherited = CapturerWithContext_Linux_;
#elif   qPlatform_Windows
        using inherited = CapturerWithContext_Windows_;
#endif
        CapturerWithContext_ (Options options)
            : inherited (options)
        {
        }
        Instruments::Memory::Info capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Memory::Info capture");
#endif
            return inherited::capture ();
        }
    };
}






/*
 ********************************************************************************
 ****************** Instruments::Memory::GetObjectVariantMapper *****************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::Memory::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    static  const   ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<uint64_t>> ();
        mapper.AddCommonType<Optional<double>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<Info::PhysicalRAMDetailsType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PhysicalRAMDetailsType, fAvailable), String_Constant (L"Available"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PhysicalRAMDetailsType, fActive), String_Constant (L"Active"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PhysicalRAMDetailsType, fInactive), String_Constant (L"Inactive"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PhysicalRAMDetailsType, fFree), String_Constant (L"Free"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PhysicalRAMDetailsType, fOSReserved), String_Constant (L"OS-Reserved"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddClass<Info::VirtualMemoryDetailsType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::VirtualMemoryDetailsType, fCommitLimit), String_Constant (L"Commit-Limit"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::VirtualMemoryDetailsType, fCommittedBytes), String_Constant (L"Committed-Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::VirtualMemoryDetailsType, fPagefileTotalSize), String_Constant (L"Pagefile-Total-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddClass<Info::PagingDetailsType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PagingDetailsType, fMajorPageFaultsSinceBoot), String_Constant (L"Major-Faults-Since-Boot"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PagingDetailsType, fMinorPageFaultsSinceBoot), String_Constant (L"Minor-Faults-Since-Boot"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PagingDetailsType, fPageOutsSinceBoot), String_Constant (L"Page-Outs-Since-Boot"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PagingDetailsType, fMajorPageFaultsPerSecond), String_Constant (L"Major-Faults-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PagingDetailsType, fMinorPageFaultsPerSecond), String_Constant (L"Minor-Faults-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info::PagingDetailsType, fPageOutsPerSecond), String_Constant (L"Page-Outs-Per-Second"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fPhysicalMemory), String_Constant (L"Physical-Memory") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fVirtualMemory), String_Constant (L"Virtual-Memory") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fPaging), String_Constant (L"Paging") },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    } ();
    return sMapper_;
}


namespace {
    const   MeasurementType kMemoryUsageMeasurement_         =   MeasurementType (String_Constant (L"Memory-Usage"));
}




namespace {
    class   MyCapturer_ : public Instrument::ICapturer {
        CapturerWithContext_ fCaptureContext;
    public:
        MyCapturer_ (const CapturerWithContext_& ctx)
            : fCaptureContext (ctx)
        {
        }
        virtual MeasurementSet  Capture ()
        {
            MeasurementSet  results;
            results.fMeasurements.Add (Measurement { kMemoryUsageMeasurement_, GetObjectVariantMapper ().FromObject (Capture_Raw (&results.fMeasuredAt))});
            return results;
        }
        nonvirtual Info  Capture_Raw (Range<DurationSecondsType>* outMeasuredAt)
        {
            DurationSecondsType before = fCaptureContext.GetLastCaptureAt ();
            Info                rawMeasurement = fCaptureContext.capture ();
            if (outMeasuredAt != nullptr) {
                *outMeasuredAt = Range<DurationSecondsType> (before, fCaptureContext.GetLastCaptureAt ());
            }
            return rawMeasurement;
        }
        virtual unique_ptr<ICapturer>   Clone () const override
        {
#if     qCompilerAndStdLib_make_unique_Buggy
            return unique_ptr<ICapturer> (new MyCapturer_ (fCaptureContext));
#else
            return make_unique<MyCapturer_> (fCaptureContext);
#endif
        }
    };
}





/*
 ********************************************************************************
 ******************* Instruments::Memory::GetInstrument *************************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::Memory::GetInstrument (Options options)
{
    return Instrument (
               InstrumentNameType  { String_Constant (L"Memory") },
#if     qCompilerAndStdLib_make_unique_Buggy
               Instrument::SharedByValueCaptureRepType (unique_ptr<MyCapturer_> (new MyCapturer_ (CapturerWithContext_ { options }))),
#else
               Instrument::SharedByValueCaptureRepType (make_unique<MyCapturer_> (CapturerWithContext_ { options })),
#endif
    { kMemoryUsageMeasurement_ },
    GetObjectVariantMapper ()
           );
}







/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template    <>
Instruments::Memory::Info   SystemPerformance::Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
{
    MyCapturer_*    myCap = dynamic_cast<MyCapturer_*> (fCapFun_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}

