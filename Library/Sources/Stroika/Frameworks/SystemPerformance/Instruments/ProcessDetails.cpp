/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <sys/sysinfo.h>
#elif   qPlatform_Windows
#include    <Windows.h>
#include    <psapi.h>
#include    <Wdbgexts.h>
#endif

#include    "../../../Foundation/Characters/CString/Utilities.h"
#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Int.h"
#include    "../../../Foundation/Characters/StringBuilder.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Debug/Trace.h"
#include    "../../../Foundation/Execution/Thread.h"
#if     qPlatform_POSIX
#include    "../../../Foundation/Execution/Platform/POSIX/Users.h"
#endif
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "../../../Foundation/IO/FileSystem/DirectoryIterable.h"
#include    "../../../Foundation/IO/FileSystem/FileSystem.h"
#include    "../../../Foundation/IO/FileSystem/PathName.h"
#include    "../../../Foundation/Memory/BLOB.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Streams/BufferedBinaryInputStream.h"
#include    "../../../Foundation/Streams/iostream/FStreamSupport.h"

#include    "ProcessDetails.h"


// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails;

using   Characters::String_Constant;
using   IO::FileSystem::BinaryFileInputStream;





#ifndef     qUseProcFS_
#define     qUseProcFS_ qPlatform_POSIX
#endif


#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_       qPlatform_Windows
#endif






#if     qUseWMICollectionSupport_
#include    "../Support/WMICollector.h"

using   SystemPerformance::Support::WMICollector;
#endif






#if     defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "psapi.lib")
#endif





const EnumNames<ProcessType::RunStatus>   ProcessType::Stroika_Enum_Names(RunStatus)
{
    { ProcessType::RunStatus::eRunning, L"Running" },
    { ProcessType::RunStatus::eSleeping, L"Sleeping" },
    { ProcessType::RunStatus::eWaitingOnDisk, L"WaitingOnDisk" },
    { ProcessType::RunStatus::eWaitingOnPaging, L"WaitingOnPaging" },
    { ProcessType::RunStatus::eZombie, L"Zombie" },
    { ProcessType::RunStatus::eSuspended, L"Suspended" },
};









/*
 ********************************************************************************
 ************** Instruments::ProcessDetails::GetObjectVariantMapper *************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::ProcessDetails::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<ProcessType::RunStatus> (ProcessType::Stroika_Enum_Names(RunStatus)));
        mapper.AddCommonType<Optional<String>> ();
        mapper.AddCommonType<Optional<ProcessType::RunStatus>> ();
        mapper.AddCommonType<Optional<pid_t>> ();
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddCommonType<Optional<unsigned int>> ();
        mapper.AddCommonType<Optional<MemorySizeType>> ();
        mapper.AddCommonType<Optional<Time::DateTime>> ();
        mapper.AddCommonType<Optional<Time::DurationSecondsType>> ();
        mapper.AddCommonType<Optional<Mapping<String, String>>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<ProcessType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fParentProcessID), String_Constant (L"Parent-Process-ID"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fUserName), String_Constant (L"User-Name"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCommandLine), String_Constant (L"Command-Line"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCurrentWorkingDirectory), String_Constant (L"Current-Working-Directory"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fEnvironmentVariables), String_Constant (L"Environment-Variables"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fEXEPath), String_Constant (L"EXE-Path"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fRoot), String_Constant (L"Root"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fProcessStartedAt), String_Constant (L"Process-Started-At"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fRunStatus), String_Constant (L"Run-Status"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fVirtualMemorySize), String_Constant (L"Virtual-Memory-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fResidentMemorySize), String_Constant (L"Resident-Memory-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fPrivateBytes), String_Constant (L"Private-Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fPageFaultCount), String_Constant (L"Page-Fault-Count"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fWorkingSetSize), String_Constant (L"Working-Set-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fTotalCPUTimeUsed), String_Constant (L"Total-CPUTime-Used"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fPercentCPUTime), String_Constant (L"Percent-CPUTime-Used"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fThreadCount), String_Constant (L"Thread-Count"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fIOTotalReadRate), String_Constant (L"IO-Total-Read-Rate"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fIOTotalWriteRate), String_Constant (L"IO-Total-Write-Rate"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fIOTotalReadBytes), String_Constant (L"IO-Total-Read-Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fIOTotalWriteBytes), String_Constant (L"IO-Total-Write-Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddCommonType<ProcessMapType> ();
        return mapper;
    } ();
    return sMapper_;
}






#if     qPlatform_POSIX
namespace {
    struct  CapturerWithContext_POSIX_ {
        struct PerfStats_ {
            double  fTotalCPUTimeUsed;
        };
        DurationSecondsType         fContextStatsCapturedAt_ {}
        Mapping<pid_t, PerfStats_>  fContextStats_;

        CapturerWithContext_POSIX_ (const Options& options)
        {
            capture_ ();        // for side-effect of setting fContextStats_
            if (fOptions_.fMinimumAveragingInterval > 0) {
                Execution::Sleep (fOptions_.fMinimumAveragingInterval);
            }
        }
        ProcessMapType  capture_ ()
        {
            ProcessMapType  result {};
#if     qUseProcFS_
            result = ExtractFromProcFS_ ();
#endif
            return result;
        }
#if     qUseProcFS_
        ProcessMapType  ExtractFromProcFS_ ()
        {
            /// Most status - like time - come from http://linux.die.net/man/5/proc
            ///proc/[pid]/stat
            //  Status information about the process. This is used by ps(1). It is defined in /usr/src/linux/fs/proc/array.c.
            //
            ProcessMapType  results;

            Mapping<pid_t, PerfStats_>  newContextStats;

            for (String dir : IO::FileSystem::DirectoryIterable (String_Constant (L"/proc"))) {
                bool isAllNumeric = not dir.FindFirstThat ([] (Character c) -> bool { return not c.IsDigit (); });
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::ExtractFromProcFS_::reading proc files");
                DbgTrace (L"isAllNumeric=%d, dir= %s", isAllNumeric, dir.c_str ());
#endif
                if (isAllNumeric) {
                    pid_t pid = String2Int<pid_t> (dir);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("reading for pid = %d", pid);
#endif
                    String  processDirPath = IO::FileSystem::AssureDirectoryPathSlashTerminated (String_Constant (L"/proc/") + dir);
                    ProcessType processDetails;
                    processDetails.fCommandLine = OptionallyReadFileString_ (processDirPath + String_Constant (L"cmdline"));
                    processDetails.fCurrentWorkingDirectory = OptionallyResolveShortcut_ (processDirPath + String_Constant (L"cwd"));
                    processDetails.fEnvironmentVariables = OptionallyReadFileStringsMap_ (processDirPath + String_Constant (L"environ"));
                    processDetails.fEXEPath = OptionallyResolveShortcut_ (processDirPath + String_Constant (L"exe"));
                    if (processDetails.fEXEPath and processDetails.fEXEPath->EndsWith (L" (deleted)")) {
                        processDetails.fEXEPath = processDetails.fEXEPath->CircularSubString (0, -10);
                    }
                    processDetails.fRoot = OptionallyResolveShortcut_ (processDirPath + String_Constant (L"root"));

                    static  const   double  kClockTick_ = ::sysconf (_SC_CLK_TCK);

                    try {
                        StatFileInfo_   stats    =  ReadStatFile_ (processDirPath + String_Constant (L"stat"));

                        //One character from the string "RSDZTW" where R is running,
                        //S is sleeping in an interruptible wait, D is waiting in uninterruptible disk sleep,
                        // Z is zombie, T is traced or stopped (on a signal), and W is paging.
                        switch (stats.state) {
                            case 'R':
                                processDetails.fRunStatus = ProcessType::RunStatus::eRunning;
                                break;
                            case 'S':
                                processDetails.fRunStatus = ProcessType::RunStatus::eSleeping;
                                break;
                            case 'D':
                                processDetails.fRunStatus = ProcessType::RunStatus::eWaitingOnDisk;
                                break;
                            case 'Z':
                                processDetails.fRunStatus = ProcessType::RunStatus::eZombie;
                                break;
                            case 'T':
                                processDetails.fRunStatus = ProcessType::RunStatus::eSuspended;
                                break;
                            case 'W':
                                processDetails.fRunStatus = ProcessType::RunStatus::eWaitingOnPaging;
                                break;
                        }

                        static  const   size_t  kPageSizeInBytes = ::sysconf (_SC_PAGESIZE);

                        static  const time_t    kSecsSinceBoot_ = [] () {
                            struct sysinfo info;
                            ::sysinfo (&info);
                            return time(NULL) - info.uptime;
                        } ();
                        //starttime %llu (was %lu before Linux 2.6)
                        //(22) The time the process started after system boot. In kernels before Linux 2.6,
                        // this value was expressed in jiffies. Since Linux 2.6,
                        // the value is expressed in clock ticks (divide by sysconf(_SC_CLK_TCK)).
                        processDetails.fProcessStartedAt = DateTime (static_cast<time_t> (stats.start_time / kClockTick_ + kSecsSinceBoot_));

                        processDetails.fTotalCPUTimeUsed = (double (stats.utime) + double (stats.stime)) / kClockTick_;
                        if (Optional<PerfStats_> p = fContextStats_.Lookup (pid)) {
                            processDetails.fPercentCPUTime =   (*processDetails.fTotalCPUTimeUsed - p->fTotalCPUTimeUsed) * 100.0 / (Time::GetTickCount () - fContextStatsCapturedAt_);
                        }
                        if (stats.nlwp != 0) {
                            processDetails.fThreadCount = stats.nlwp;
                        }
                        processDetails.fParentProcessID = stats.ppid;
                        processDetails.fVirtualMemorySize = stats.vsize;
                        processDetails.fResidentMemorySize = stats.rss * kPageSizeInBytes;

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"loaded processDetails.fProcessStartedAt=%s wuit stats.start_time = %lld", (*processDetails.fProcessStartedAt).Format ().c_str (), stats.start_time);
                        DbgTrace (L"loaded processDetails.fTotalCPUTimeUsed=%f wuit stats.utime = %lld, stats.stime = %lld", (*processDetails.fTotalCPUTimeUsed), stats.utime , stats.stime);
#endif
                    }
                    catch (...) {
                    }

                    try {
                        proc_status_data_   stats    =  Readproc_proc_status_data_ (processDirPath + String_Constant (L"status"));
                        processDetails.fUserName = Execution::Platform::POSIX::uid_t2UserName (stats.ruid);
                    }
                    catch (...) {
                    }

                    try {
                        Optional<proc_io_data_>   stats    =  Readproc_io_data_ (processDirPath + String_Constant (L"io"));
                        if (stats.IsPresent ()) {
                            processDetails.fIOTotalReadBytes = (*stats).read_bytes;
                            processDetails.fIOTotalWriteBytes = (*stats).write_bytes;
                        }
                    }
                    catch (...) {
                    }

                    fContextStatsCapturedAt_ = Time::GetTickCount ();
                    newContextStats.Add (pid, PerfStats_ { *processDetails.fTotalCPUTimeUsed });
                    results.Add (pid, processDetails);
                }
            }
            return results;
        }
        template    <typename T>
        Optional<T> OptionallyReadIfFileExists_ (const String& fullPath, const function<T(const Streams::BinaryInputStream&)>& reader)
        {
            if (IO::FileSystem::FileSystem::Default ().Access (fullPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (return reader (BinaryFileInputStream::mk (fullPath, BinaryFileInputStream::eNotSeekable)));
            }
            return Optional<T> ();
        }

        // this reads /proc format files - meaning that a trialing nul-byte is the EOS
        String  ReadFileString_(const Streams::BinaryInputStream& in)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::ReadFileString_");
#endif
            StringBuilder sb;
            for (Memory::Optional<Memory::Byte> b; (b = in.Read ()).IsPresent ();) {
                if (*b == '\0') {
                    break;
                }
                else {
                    sb.Append ((char) (*b));    // for now assume no charset
                }
            }
            return (sb.As<String> ());
        }
        String  ReadFileString_(const String& fullPath)
        {
            return ReadFileString_ (BinaryFileInputStream::mk (fullPath, BinaryFileInputStream::eNotSeekable));
        }
        Sequence<String>  ReadFileStrings_(const String& fullPath)
        {
            Sequence<String>    results;
            Streams::BinaryInputStream   in = BinaryFileInputStream::mk (fullPath, BinaryFileInputStream::eNotSeekable);
            StringBuilder sb;
            for (Memory::Optional<Memory::Byte> b; (b = in.Read ()).IsPresent ();) {
                if (*b == '\0') {
                    results.Append (sb.As<String> ());
                    sb.clear();
                }
                else {
                    sb.Append ((char) (*b));    // for now assume no charset
                }
            }
            return results;
        }
        Mapping<String, String>  ReadFileStringsMap_(const String& fullPath)
        {
            Mapping<String, String>    results;
            for (String i : ReadFileStrings_ (fullPath)) {
                auto tokens = i.Tokenize (Set<Character> { '=' });
                if (tokens.size () == 2) {
                    results.Add (tokens[0], tokens[1]);
                }
            }
            return results;
        }

        // if fails (cuz not readable) dont throw but return missing, but avoid noisy stroika exception logging
        Optional<String>    OptionallyReadFileString_(const String& fullPath)
        {
            if (IO::FileSystem::FileSystem::Default ().Access (fullPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (return ReadFileString_ (fullPath));
            }
            return Optional<String> ();
        }
        // if fails (cuz not readable) dont throw but return missing, but avoid noisy stroika exception logging
        Optional<String>    OptionallyResolveShortcut_ (const String& shortcutPath)
        {
            if (IO::FileSystem::FileSystem::Default ().Access (shortcutPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (return IO::FileSystem::FileSystem::Default ().ResolveShortcut (shortcutPath));
            }
            return Optional<String> ();
        }
        Optional<Mapping<String, String>>  OptionallyReadFileStringsMap_(const String& fullPath)
        {
            if (IO::FileSystem::FileSystem::Default ().Access (fullPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (return ReadFileStringsMap_ (fullPath));
            }
            return Optional<Mapping<String, String>> ();
        }

        struct  StatFileInfo_ {
            //@todo REDO BASED on http://linux.die.net/man/5/proc,  search for '/proc/[pid]/stat'

            // trim down and find better source - but for now use 'procps-3.2.8\proc\'
            int                 ppid;
            char                state;      // stat,status     single-char code for process state (S=sleeping)
            unsigned long long  utime;      // stat            user-mode CPU time accumulated by process
            unsigned long long  stime;      // stat            kernel-mode CPU time accumulated by process
            unsigned long long  cutime;     // stat            cumulative utime of process and reaped children
            unsigned long long  cstime;     // stat            cumulative stime of process and reaped children
            unsigned long long  start_time; // stat            start time of process -- seconds since 1-1-70

            long
            priority,   // stat            kernel scheduling priority
            nice,       // stat            standard unix nice level of process
            rss,        // stat            resident set size from /proc/#/stat (pages)
            alarm,      // stat            ?
            // the next 7 members come from /proc/#/statm
            size,       // statm           total # of pages of memory
            resident,   // statm           number of resident set (non-swapped) pages (4k)
            share,      // statm           number of pages of shared (mmap'd) memory
            trs,        // statm           text resident set size
            lrs,        // statm           shared-lib resident set size
            drs,        // statm           data resident set size
            dt;     // statm           dirty pages

            unsigned long
            vm_size,        // status          same as vsize in kb
            vm_lock,        // status          locked pages in kb
            vm_rss,         // status          same as rss in kb
            vm_data,        // status          data size
            vm_stack,       // status          stack size
            vm_exe,         // status          executable size
            vm_lib,         // status          library size (all pages, not just used ones)
            rtprio,     // stat            real-time priority
            sched,      // stat            scheduling class
            vsize,      // stat            number of pages of virtual memory ...
            rss_rlim,   // stat            resident set size limit?
            flags,      // stat            kernel flags for the process
            min_flt,    // stat            number of minor page faults since process start
            maj_flt,    // stat            number of major page faults since process start
            cmin_flt,   // stat            cumulative min_flt of process and child processes
            cmaj_flt;   // stat            cumulative maj_flt of process and child processes

            int
            pgrp,       // stat            process group id
            session,    // stat            session id
            nlwp,       // stat,status     number of threads, or 0 if no clue
            tgid,       // (special)       task group ID, the POSIX PID (see also: tid)
            tty,        // stat            full device number of controlling terminal
            euid, egid,     // stat(),status   effective
            ruid, rgid,     // status          real
            suid, sgid,     // status          saved
            fuid, fgid,     // status          fs (used for file access only)
            tpgid,      // stat            terminal process group id
            exit_signal,    // stat            might not be SIGCHLD
            processor;

        };
        StatFileInfo_   ReadStatFile_ (const String& fullPath)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::ReadStatFile_");
            DbgTrace (L"fullPath=%s", fullPath.c_str ());
#endif
            StatFileInfo_    result {};
            Streams::BinaryInputStream   in = BinaryFileInputStream::mk (fullPath, BinaryFileInputStream::eNotSeekable);
            Byte    data[10 * 1024];
            size_t nBytes = in.Read (begin (data), end (data));
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("nBytes read = %d", nBytes);
#endif

            const char* S = reinterpret_cast<const char*> (data);

            ///@TODO - FIX - THIS CODE UNSAFE - CAN CRASH!

            {
                S = strchr(S, '(') + 1;
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("S = %x", S);
#endif
                const char* tmp = strrchr(S, ')');
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("S(tmp) = %x", tmp);
#endif
                S = tmp + 2;                 // skip ") "
            }

            // MSVC SILLY WARNING ABOUT USING swscanf_s
            // (warning doesnt appear to check if we have mismatch between types and format args provided.
            //      --LGP 2015-01-07
            DISABLE_COMPILER_MSC_WARNING_START(4996)
            int num = sscanf(S,
                             "%c "
                             "%d %d %d %d %d "
                             "%lu %lu %lu %lu %lu "
                             "%llu %llu %llu %llu "  /* utime stime cutime cstime */
                             "%ld %ld "
                             "%d "
                             "%ld "
                             "%llu "  /* start_time */
                             "%lu "
                             "%ld "
#if 0
                             /*
                             "%lu %"KLF"u %"KLF"u %"KLF"u %"KLF"u %"KLF"u "
                             "%*s %*s %*s %*s " // discard, no RT signals & Linux 2.1 used hex
                             "%"KLF"u %*lu %*lu "
                             "%d %d "
                             "%lu %lu"
                             */
#endif
                             ,
                             &result.state,
                             &result.ppid, &result.pgrp, &result.session, &result.tty, &result.tpgid,
                             &result.flags, &result.min_flt, &result.cmin_flt, &result.maj_flt, &result.cmaj_flt,
                             &result.utime, &result.stime, &result.cutime, &result.cstime,
                             &result.priority, &result.nice,
                             &result.nlwp,
                             &result.alarm,
                             &result.start_time,
                             &result.vsize,
                             &result.rss
#if 0
                             & result.rss_rlim, &result.start_code, &result.end_code, &result.start_stack, &result.kstk_esp, &result.kstk_eip,
                             /*     P->signal, P->blocked, P->sigignore, P->sigcatch,   */ /* can't use */
                             &result.wchan, /* &P->nswap, &P->cnswap, */  /* nswap and cnswap dead for 2.4.xx and up */
                             /* -- Linux 2.0.35 ends here -- */
                             &result.exit_signal, &result.processor,  /* 2.2.1 ends with "exit_signal" */
                             /* -- Linux 2.2.8 to 2.5.17 end here -- */
                             &result.rtprio, &result.sched  /* both added to 2.5.18 */
#endif
                            );
            DISABLE_COMPILER_MSC_WARNING_END(4996)// MSVC SILLY WARNING ABOUT USING swscanf_s

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("result.start_time=%lld", result.start_time);
            DbgTrace ("result.vsize=%ld", result.vsize);
            DbgTrace ("result.rss=%ld", result.rss);
            DbgTrace ("result.utime=%lld", result.utime);
#endif

            return result;
        }

        // https://www.kernel.org/doc/Documentation/filesystems/proc.txt
        // search for 'cat /proc/3828/io'
        struct proc_io_data_ {
            uint64_t read_bytes;
            uint64_t write_bytes;
        };
        Optional<proc_io_data_>   Readproc_io_data_ (const String& fullPath)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::Readproc_io_data_");
            DbgTrace (L"fullPath=%s", fullPath.c_str ());
#endif

            if (not IO::FileSystem::FileSystem::Default ().Access (fullPath)) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"Skipping read cuz no access");
#endif
                return Optional<proc_io_data_> ();
            }
            proc_io_data_    result {};
            ifstream r;
            Streams::iostream::OpenInputFileStream (&r, fullPath);
            while (r) {
                char buf[1024];
                buf [0] = '\0';
                if (r.getline (buf, sizeof(buf))) {
                    const char kReadLbl_ [] = "read_bytes:";
                    const char kWriteLbl_ [] = "write_bytes:";
                    if (strncmp (buf, kReadLbl_, strlen (kReadLbl_)) == 0) {
                        result.read_bytes = Characters::CString::String2Int<decltype (result.read_bytes)> (buf + strlen (kReadLbl_));
                    }
                    else if (strncmp (buf, kWriteLbl_, strlen (kWriteLbl_)) == 0) {
                        result.write_bytes = Characters::CString::String2Int<decltype (result.write_bytes)> (buf + strlen (kWriteLbl_));
                    }
                }
            }
            return result;
        }

#if     !qPlatform_POSIX
        using uid_t = int;
#endif

        // https://www.kernel.org/doc/Documentation/filesystems/proc.txt
        // search for 'cat /proc/PID/status'
        struct proc_status_data_ {
            uid_t ruid;
        };
        proc_status_data_   Readproc_proc_status_data_ (const String& fullPath)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::Readproc_proc_status_data_");
            DbgTrace (L"fullPath=%s", fullPath.c_str ());
#endif
            proc_status_data_    result {};
            ifstream r;
            Streams::iostream::OpenInputFileStream (&r, fullPath);
            while (r) {
                char buf[1024];
                buf [0] = '\0';
                if (r.getline (buf, sizeof(buf))) {
                    const char kUidLbl [] = "Uid:";
                    if (strncmp (buf, kUidLbl, strlen(kUidLbl)) == 0) {
                        char* S = buf + strlen(kUidLbl);
                        int ruid = strtol (S, &S, 10);
                        int euid = strtol (S, &S, 10);
                        int suid = strtol (S, &S, 10);
                        int fuid = strtol (S, &S, 10);
                        result.ruid = ruid;
                    }
                }
            }
            return result;
        }
#endif




// consider using this as a backup if /procfs/ not present...
#if   qPlatform_POSIX && 0
        Collection<pair<pid_t, ProcessType>> capture_using_ps_ ()
        {
            Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails::{}::capture_using_ps_");
            Collection<pair<pid_t, ProcessType>>   result;
            ProcessRunner pr (L"ps -axl");
            Streams::BasicBinaryInputOutputStream   useStdOut;
            pr.SetStdOut (useStdOut);
            pr.Run ();
            String out;
            Streams::TextInputStreamBinaryAdapter   stdOut  =   Streams::TextInputStreamBinaryAdapter (useStdOut);
            bool skippedHeader = false;
            for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                if (not skippedHeader) {
                    skippedHeader = true;
                    continue;
                }
                Sequence<String>    l    =  Characters::Tokenize<String> (i, String_Constant (L" "));
                if (l.size () < 13) {
                    DbgTrace ("skipping line cuz len=%d", l.size ());
                    continue;
                }
                ProcessType p;
                p.fUserID = Characters::String2Int<int> (l[1]);
                pid_t   pid = Characters::String2Int<int> (l[2]);
                p.fParentProcessID = Characters::String2Int<int> (l[3]);
                {
                    string  tmp =   l[11].AsUTF8 ();
                    int minutes = 0;
                    int seconds = 0;
                    sscanf (tmp.c_str (), "%d:%d", &minutes, &seconds);
                    p.fCPUTimeUsed = minutes * 60 + seconds;
                }
                {
                    // wrong - must grab EVERYHTING from i past a certain point
                    const size_t kCmdNameStartsAt_ = 69;    // not sure this is always true? Empirical!
                    p.fCommandName = i.size () <= kCmdNameStartsAt_ ? String () : i.SubString (kCmdNameStartsAt_);
                }
                {
                    // Fake but usable answer
                    Sequence<String>    t    =  Characters::Tokenize<String> (p.fCommandName, String_Constant (L" "));
                    if (not t.empty ()) {
                        p.fEXEPath = t[0];
                    }
                }
                result.Add ({pid, p});
            }
            return result;
        }
#endif
    };
};
#endif



#if     qPlatform_Windows
namespace {
    struct UNICODE_STRING {
        USHORT Length;
        USHORT MaximumLength;
        PWSTR Buffer;
    };
    struct PROCESS_BASIC_INFORMATION {
        PVOID Reserved1;
        PVOID /*PPEB*/ PebBaseAddress;
        PVOID Reserved2[2];
        ULONG_PTR UniqueProcessId;
        PVOID Reserved3;
    };
    PVOID GetPebAddress_ (HANDLE ProcessHandle)
    {
        static  LONG    (WINAPI * NtQueryInformationProcess)(HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength) =  (LONG    (WINAPI*)(HANDLE , ULONG , PVOID , ULONG , PULONG ))::GetProcAddress (::LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");
        PROCESS_BASIC_INFORMATION pbi;
        NtQueryInformationProcess (ProcessHandle, 0, &pbi, sizeof(pbi), NULL);
        return pbi.PebBaseAddress;
    }
}
#endif



#if     qUseWMICollectionSupport_
namespace {
    const   String_Constant     kProcessID_             { L"ID Process" };
    const   String_Constant     kThreadCount_           { L"Thread Count" };
    const   String_Constant     kIOReadBytesPerSecond_  { L"IO Read Bytes/sec" };
    const   String_Constant     kIOWriteBytesPerSecond_ { L"IO Write Bytes/sec" };
    const   String_Constant     kPercentProcessorTime_  { L"% Processor Time" };            // % Processor Time is the percentage of elapsed time that all of process threads
    // used the processor to execution instructions. An instruction is the basic unit of
    // execution in a computer, a thread is the object that executes instructions, and a
    // process is the object created when a program is run. Code executed to handle some
    // hardware interrupts and trap conditions are included in this count.
    const   String_Constant     kElapsedTime_           { L"Elapsed Time" };                // The total elapsed time, in seconds, that this process has been running.
}
#endif





#if     qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ {
#if     qUseWMICollectionSupport_
        WMICollector        fProcessWMICollector_ { String_Constant { L"Process" }, {WMICollector::kWildcardInstance},  {kProcessID_, kThreadCount_, kIOReadBytesPerSecond_, kIOWriteBytesPerSecond_, kPercentProcessorTime_, kElapsedTime_ } };
        DurationSecondsType fMinTimeBeforeFirstCapture_;
#endif
        CapturerWithContext_Windows_ (const Options& options)
        {
            capture_ ();// hack so we prefill with each process capture
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
#if     qUseWMICollectionSupport_
            : fProcessWMICollector_ (from.fProcessWMICollector_)
#endif
        {
#if   qUseWMICollectionSupport_
            IgnoreExceptionsForCall (fProcessWMICollector_.Collect ()); // hack cuz no way to copy
#endif
        }
        ProcessMapType  capture_ ()
        {
#if     qUseWMICollectionSupport_
            Time::DurationSecondsType   timeOfPrevCollection = fProcessWMICollector_.GetTimeOfLastCollection ();
            IgnoreExceptionsForCall (fProcessWMICollector_.Collect ()); // hack cuz no way to copy
            Time::DurationSecondsType   timeCollecting { fProcessWMICollector_.GetTimeOfLastCollection () - timeOfPrevCollection };

            for (String i : fProcessWMICollector_.GetAvailableInstaces ()) {
                DbgTrace (L"wmi isntance name %s", i.c_str ());
            }

            // NOTE THIS IS BUGGY - MUST READ BACK AS INT NOT DOUBLE
            Mapping<pid_t, String>  pid2InstanceMap;
            for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kProcessID_)) {
                pid2InstanceMap.Add (static_cast<int> (i.fValue), i.fKey);
            }
#endif

            ProcessMapType  results;
            for (pid_t pid : GetAllProcessIDs_ ()) {
                ProcessType         processInfo;
                {
                    HANDLE hProcess = ::OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
                    if (hProcess != nullptr) {
                        Execution::Finally cleanup {[hProcess] ()
                        {
                            Verify (::CloseHandle (hProcess));
                        }
                                                   };
                        {
                            Optional<String>    processEXEPath;
                            Optional<pid_t>     parentProcessID;
                            Optional<String>    cmdLine;
                            LookupProcessPath_ (hProcess,  &processEXEPath, &parentProcessID, &cmdLine);
                            processEXEPath.CopyToIf (&processInfo.fEXEPath);
                            parentProcessID.CopyToIf (&processInfo.fParentProcessID);
                            cmdLine.CopyToIf (&processInfo.fCommandLine);
                        }
                        {
                            PROCESS_MEMORY_COUNTERS_EX  memInfo;
                            if (::GetProcessMemoryInfo (hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS*> (&memInfo), sizeof(memInfo))) {
                                processInfo.fWorkingSetSize = memInfo.WorkingSetSize;
                                processInfo.fPrivateBytes = memInfo.PrivateUsage;
                                processInfo.fPageFaultCount = memInfo.PageFaultCount;
                            }
                        }
                    }
                }
#if   qUseWMICollectionSupport_
                {
                    String instanceVal = pid2InstanceMap.LookupValue (pid);

                    // Not the most efficient appraoch ;-)
                    for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kThreadCount_)) {
                        if (instanceVal == i.fKey) {
                            processInfo.fThreadCount = static_cast<unsigned int> (i.fValue);
                            break;
                        }
                    }
                    // Not the most efficient appraoch ;-)
                    for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kIOReadBytesPerSecond_)) {
                        if (instanceVal == i.fKey) {
                            processInfo.fIOTotalReadRate = i.fValue;
                            break;
                        }
                    }
                    // Not the most efficient appraoch ;-)
                    for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kIOWriteBytesPerSecond_)) {
                        if (instanceVal == i.fKey) {
                            processInfo.fIOTotalWriteRate = i.fValue;
                            break;
                        }
                    }
                    // Not the most efficient appraoch ;-)
                    for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kPercentProcessorTime_)) {
                        if (instanceVal == i.fKey) {
                            processInfo.fPercentCPUTime = i.fValue;
                            break;
                        }
                    }
                    // Not the most efficient appraoch ;-)
                    for (KeyValuePair<String, double> i : fProcessWMICollector_.GetCurrentValues (kElapsedTime_)) {
                        if (instanceVal == i.fKey) {
                            processInfo.fProcessStartedAt = DateTime::Now ().AddSeconds (-static_cast<time_t> (i.fValue));
                            break;
                        }
                    }
                }
#endif
                results.Add (pid, processInfo);
            }
            return results;
        }
        Set<pid_t>  GetAllProcessIDs_ ()
        {
            DWORD aProcesses[10 * 1024];
            DWORD cbNeeded;

            Set<pid_t> result;
            if (not ::EnumProcesses (aProcesses, sizeof (aProcesses), &cbNeeded)) {
                AssertNotReached ();
                return result;
            }

            // Calculate how many process identifiers were returned.
            DWORD   cProcesses = cbNeeded / sizeof (DWORD);
            for (DWORD i = 0; i < cProcesses; ++i) {
                result.Add (aProcesses[i]);
            }
            return result;
        }
        void    LookupProcessPath_ (HANDLE hProcess, Optional<String>* processEXEPath, Optional<pid_t>* parentProcessID, Optional<String>* cmdLine)
        {
            RequireNotNull (hProcess);
            RequireNotNull (processEXEPath);
            RequireNotNull (parentProcessID);
            RequireNotNull (cmdLine);
            HMODULE     hMod        {};    // note no need to free handles returned by EnumProcessModules () accorind to man-page for EnumProcessModules
            DWORD       cbNeeded    {};
            if (::EnumProcessModules (hProcess, &hMod, sizeof (hMod), &cbNeeded)) {
                TCHAR moduleFullPath[MAX_PATH];
                moduleFullPath[0] = '\0';
                if (::GetModuleFileNameEx (hProcess, hMod, moduleFullPath, NEltsOf(moduleFullPath)) != 0) {
                    *processEXEPath =  String::FromSDKString (moduleFullPath);
                }
            }

            {
                const   ULONG   ProcessBasicInformation  = 0;
                static  LONG    (WINAPI * NtQueryInformationProcess)(HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength) =  (LONG    (WINAPI*)(HANDLE , ULONG , PVOID , ULONG , PULONG ))::GetProcAddress (::LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");
                if (NtQueryInformationProcess) {
                    ULONG_PTR pbi[6];
                    ULONG ulSize = 0;
                    if (NtQueryInformationProcess (hProcess, ProcessBasicInformation,  &pbi, sizeof (pbi), &ulSize) >= 0 && ulSize == sizeof(pbi)) {
                        *parentProcessID =  pbi[5];

                        // Cribbed from http://windows-config.googlecode.com/svn-history/r59/trunk/doc/cmdline/cmdline.cpp
                        void*   pebAddress = GetPebAddress_ (hProcess);
                        if (pebAddress != nullptr) {
                            void*   rtlUserProcParamsAddress {};
#ifdef  _WIN64
                            const int kUserProcParamsOffset_ = 0x20;
                            const int kCmdLineOffset_ = 112;
#else
                            const int kUserProcParamsOffset_ = 0x10;
                            const int kCmdLineOffset_ = 0x40;
#endif
                            /* get the address of ProcessParameters */
                            if (not ::ReadProcessMemory(hProcess, (PCHAR)pebAddress + kUserProcParamsOffset_, &rtlUserProcParamsAddress, sizeof(PVOID), NULL)) {
                                goto SkipCmdLine_;
                            }
                            UNICODE_STRING commandLine;

                            /* read the CommandLine UNICODE_STRING structure */
                            if (not ::ReadProcessMemory (hProcess, (PCHAR)rtlUserProcParamsAddress + kCmdLineOffset_,  &commandLine, sizeof(commandLine), NULL)) {
                                goto SkipCmdLine_;
                            }
                            {
                                size_t  strLen = commandLine.Length / sizeof (WCHAR);   // length field in bytes
                                Memory::SmallStackBuffer<WCHAR> commandLineContents (strLen + 1);
                                /* read the command line */
                                if (not ReadProcessMemory(hProcess, commandLine.Buffer, commandLineContents.begin (), commandLine.Length, NULL)) {
                                    goto SkipCmdLine_;
                                }
                                commandLineContents[strLen] = 0;
                                *cmdLine = commandLineContents.begin ();
                            }
SkipCmdLine_:
                            ;
                        }

                    }
                }
            }
        }
    };
};
#endif










namespace {
    struct  CapturerWithContext_
            : Debug::AssertExternallySynchronizedLock
#if     qPlatform_POSIX
            , CapturerWithContext_POSIX_
#elif   qPlatform_Windows
            , CapturerWithContext_Windows_
#endif
    {
#if     qPlatform_POSIX
        using inherited = CapturerWithContext_POSIX_;
#elif   qPlatform_Windows
        using inherited = CapturerWithContext_Windows_;
#endif
        CapturerWithContext_ (const Options& options)
            : inherited (options)
        {
        }
        ProcessMapType capture_ ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::ProcessDetails capture_");
#endif
            return inherited::capture_ ();
        }
    };
}


const   MeasurementType SystemPerformance::Instruments::ProcessDetails::kProcessMapMeasurement = MeasurementType (String_Constant (L"Process-Details"));






/*
 ********************************************************************************
 ************** Instruments::ProcessDetails::GetInstrument **********************
 ********************************************************************************
 */
Instrument          SystemPerformance::Instruments::ProcessDetails::GetInstrument (const Options& options)
{
    CapturerWithContext_ useCaptureContext { options };  // capture context so copyable in mutable lambda
    return Instrument (
               InstrumentNameType (String_Constant (L"Process-Details")),
    [useCaptureContext] () mutable -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        auto rawMeasurement = useCaptureContext.capture_();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kProcessMapMeasurement;
        results.fMeasurements.Add (m);
        return results;
    },
    {kProcessMapMeasurement},
    GetObjectVariantMapper ()
           );
}
