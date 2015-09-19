/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_Memory_h_
#define _Stroika_Framework_SystemPerformance_Instruments_Memory_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../../Foundation/Memory/Optional.h"

#include    "../Instrument.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {
                namespace   Memory {


                    using   DataExchange::ObjectVariantMapper;
                    using   Foundation::Memory::Optional;


                    /**
                     *      @see https://www.centos.org/docs/5/html/5.1/Deployment_Guide/s2-proc-meminfo.html
                     *      @see https://github.com/torvalds/linux/blob/master/Documentation/filesystems/proc.txt
                     */
                    struct  Info {
                        /**
                         *  The amount of physical RAM, left unused by the system (in bytes).
                         *
                         *  From Linux:
                         *      /proc/meminfo::MemFree
                         *
                         *  From Windows:
                         *      ::GlobalMemoryStatusEx (&statex), statex.ullAvailPhys
                         *
                         */
                        Optional<uint64_t>  fFreePhysicalMemory {};

                        /**
                         *  The amount of physical RAM which is actively being used.
                         *
                         *  For AIX:
                         *      perfstat_memory_total_t::real_process
                         *      Number of pages used by process segments.
                         *
                         *      This is a WEAK definition for AIX, but I haven't yet found better. I cannot even find free's list of 'cached and buffers' to subtract from
                         *      total RAM. Also no working-set-size
                         *
                         *  From Linux:
                         *      /proc/meminfo::Active
                         *      Memory that has been used more recently and usually not
                         *      reclaimed unless absolutely necessary.
                         *
                         *      (possibly should add kernel meminfo::Buffers to this?)
                         *
                         *  From Windows:
                         *      MEMORYSTATUSEX::dwMemoryLoad * MEMORYSTATUSEX::ullTotalPhys / 100
                         */
                        Optional<uint64_t>  fActivePhysicalMemory {};

                        /**
                         *  The amount of physical RAM which is in use, but not actively. Windows calls this 'Standby'
                         *
                         *  For AIX:
                         *      @todo - WRONG
                         *      perfstat_memory_total_t::real_process
                         *      Number of pages used by process segments.
                         *
                         *      This is a WEAK definition for AIX, but I haven't yet found better. I cannot even find free's list of 'cached and buffers' to subtract from
                         *      total RAM. Also no working-set-size
                         *
                         *  From Linux:
                         *      /proc/meminfo::Inactive
                         *      Inactive: Memory which has been less recently used.  It is more
                         *      eligible to be reclaimed for other purposes.
                         *
                         *  From Windows:
                         *      @todo Total RAM - ActiveMemory(MEMORYSTATUSEX::dwMemoryLoad * MEMORYSTATUSEX::ullTotalPhys / 100) - Free-Memory(how)
                         */
                        Optional<uint64_t>  fInactivePhysicalMemory {};

                        /**
                         *     DEFINITION UNCLEAR (cross-platform):
                         *          But roughtly - this is the number of bytes of pagefile allocated + number of bytes of physical memory.
                         *
                         *  We use as our definition of this "Limit" from http://en.wikipedia.org/wiki/Commit_charge
                         *      Limit is the maximum possible value for Total; it is the sum of the current pagefile size plus the physical memory
                         *      available for pageable contents (this excludes RAM that is assigned to non-pageable areas).
                         *      The corresponding performance counter is called "Commit Limit".
                         *
                         *  From Linux:
                         *      /proc/meminfo::CommitLimit
                         *
                         *      Note - on Linux - this value is controlled BOTH by the amount of RAM, and overcommit ratio.
                         *      @see  https://www.kernel.org/doc/Documentation/vm/overcommit-accounting about
                         *      sysctl vm.overcommit_memory vm.overcommit_ratio
                         *
                         *      So from: http://liyiadam.blogspot.com/2008/01/procmeminfo.html
                         *          CommitLimit: Based on the overcommit ratio ('vm.overcommit_ratio'),
                         *          this is the total amount of memory currently available to
                         *          be allocated on the system. This limit is only adhered to
                         *          if strict overcommit accounting is enabled (mode 2 in
                         *          'vm.overcommit_memory').
                         *
                         *          The CommitLimit is calculated with the following formula:
                         *              CommitLimit = ('vm.overcommit_ratio' * Physical RAM) + Swap
                         *
                         *          For example, on a system with 1G of physical RAM and 7G
                         *          of swap with a `vm.overcommit_ratio` of 30 it would
                         *          yield a CommitLimit of 7.3G
                         *
                         *          (note that overcommit_ratio is a PERCENTAGE).
                         *
                         *  From Windows:
                         *      WMI:    "Memory(_Total)/Commit Limit"
                         *
                         *          Commit Limit is the amount of virtual memory that can be committed without having to extend
                         *          the paging file(s).  It is measured in bytes.
                         *          Committed memory is the physical memory which has space reserved on the disk paging files.
                         *          There can be one paging file on each logical drive). If the paging file(s) are be expanded,
                         *          this limit increases accordingly.
                         */
                        Optional<uint64_t>  fCommitLimit {};

                        /**
                         *  From Windows:
                         *      WMI:    "Memory(_Total)/Committed Bytes"
                         *
                         *              Committed Bytes is the amount of committed virtual memory, in bytes.
                         *              Committed memory is the physical memory which has space reserved on the disk paging file(s).
                         *              There can be one or more paging files on each physical drive.
                         *
                         *  From Linux (http://lwn.net/Articles/28345/)
                         *      /proc/meminfo:  Committed_AS:
                         *
                         *              An estimate of how much RAM you would need to make a
                         *              99.99% guarantee that there never is OOM (out of memory)
                         *              for this workload. Normally the kernel will overcommit
                         *              memory. That means, say you do a 1GB malloc, nothing
                         *              happens, really. Only when you start USING that malloc
                         *              memory you will get real memory on demand, and just as
                         *              much as you use. So you sort of take a mortgage and hope
                         *              the bank doesn't go bust. Other cases might include when
                         *              you mmap a file that's shared only when you write to it
                         *              and you get a private copy of that data. While it normally
                         *              is shared between processes. The Committed_AS is a
                         *              guesstimate of how much RAM/swap you would need
                         *              worst-case.
                         *
                         *  From https://github.com/torvalds/linux/blob/master/Documentation/filesystems/proc.txt
                         *          Committed_AS:
                         *              The amount of memory presently allocated on the system.
                         *              The committed memory is a sum of all of the memory which
                         *              has been allocated by processes, even if it has not been
                         *              "used" by them as of yet. A process which malloc()'s 1G
                         *              of memory, but only touches 300M of it will show up as
                         *              using 1G. This 1G is memory which has been "committed" to
                         *              by the VM and can be used at any time by the allocating
                         *              application. With strict overcommit enabled on the system
                         *              (mode 2 in 'vm.overcommit_memory'),allocations which would
                         *              exceed the CommitLimit (detailed above) will not be permitted.
                         *              This is useful if one needs to guarantee that processes will
                         *              not fail due to lack of memory once that memory has been
                         *              successfully allocated.
                         *
                         *  For AIX:
                         *      RAM IN USE + PageFile INUSE
                         *
                         *  So in ALL cases it represents 'total reserved VM' but in the Linux case, its a weaker
                         *  form of reservation.
                         */
                        Optional<uint64_t>  fCommittedBytes {};

#if 0
                        /**
                         *      The largest contiguous block of available virtual memory (in bytes).
                         *
                         *      /proc/meminfo::VMallocChunk
                         */
                        Optional<uint64_t>  fLargestAvailableVirtualChunk {};
#endif

                        /**
                         *      Total size of all loaded swapfiles (or on windows pagefiles).
                         *
                         *      /proc/meminfo::SwapTotal
                         */
                        Optional<uint64_t>  fPagefileTotalSize {};

                        /**
                         *  @see http://en.wikipedia.org/wiki/Page_fault
                         *  @see http://www.linuxinsight.com/proc_vmstat.html
                         *
                         *  fMajorPageFaultsSinceBoot   is the number of major (requiring a disk read)
                         *  faults since the system booted.
                         *
                         *  @see /proc/vmstat::pgmajfault
                         *  @see /proc/vmstat::pgfault
                         */
                        Optional<uint64_t>    fMajorPageFaultsSinceBoot {};

                        /**
                         *  @see http://en.wikipedia.org/wiki/Page_fault
                         *  @see http://www.linuxinsight.com/proc_vmstat.html
                         *
                         *  fMinorPageFaultsSinceBoot   is the number of minor (not requiring a disk read)
                         *  faults since the system booted.
                         *
                         *  @see /proc/vmstat::pgfault and proc/vmstat::pgmajfault (this is pgfault-pgmajfault)
                         */
                        Optional<uint64_t>    fMinorPageFaultsSinceBoot {};

                        /**
                         *  @see fMajorPageFaultsSinceBoot.
                         *
                         *  This is not computed in the first call to the intstrument, but based on successive calls
                         */
                        Optional<double>    fMajorPageFaultsPerSecond {};

                        /**
                         *  @see fMinorPageFaultsSinceBoot.
                         *
                         *  This is not computed in the first call to the intstrument, but based on successive calls
                         */
                        Optional<double>    fMinorPageFaultsPerSecond {};


                        /**
                         *  Calculated from Free and Inactive memory, or from the OS provided claim about memory available.
                         *  On Linux (see https://github.com/torvalds/linux/blob/master/Documentation/filesystems/proc.txt
                         *  'MemAvailable'
                         */
                        nonvirtual  Optional<uint64_t>  MemoryAvailable () const;
                    };


                    /**
                     *  For Info type.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    /**
                     *  To control the behavior of the instrument.
                     */
                    struct  Options {
                        /**
                         *  \req fMinimumAveragingInterval >= 0
                         */
                        Time::DurationSecondsType   fMinimumAveragingInterval { 1.0 };
                    };


                    /**
                     *  Instrument returning Info measurements.
                     */
                    Instrument          GetInstrument (Options options = Options ());


                }
            }


            /*
             *  Specialization to improve performance
             */
            template    <>
            Instruments::Memory::Info   Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_Memory_h_*/
