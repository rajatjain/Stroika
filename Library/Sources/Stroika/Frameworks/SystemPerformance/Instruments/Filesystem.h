/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_
#define _Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Time/Realtime.h"

#include    "../Instrument.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 *  TODO:
 *      @todo   add optional Options filter 'Set<String> fRestrictToVolumnesContainingPaths';
 *
 *      @todo   Consider using http://linux.die.net/man/3/setmntent, or some such, to get the list of mount
 *              points and associated device names.
 *
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {
                namespace   Filesystem {


                    using   Memory::Optional;
                    using   DataExchange::ObjectVariantMapper;


                    /**
                    */
                    enum    MountedDeviceType {
                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Removable Disk" or
                         *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_REMOVABLE
                         */
                        eRemovableDisk,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Local Disk" or
                         *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_FIXED
                         */
                        eLocalDisk,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Network Drive" or
                         *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_REMOTE
                         */
                        eNetworkDrive,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "RAM Disk" or
                         *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_RAMDISK
                         *  On Linux, this is tmpfs
                         */
                        eTemporaryFiles,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Compact Disc" or
                         *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_CDROM
                         */
                        eReadOnlyEjectable,

                        /**
                         *  e.g. Linux procfs
                         */
                        eSystemInformation,

                        Stroika_Define_Enum_Bounds(eRemovableDisk, eSystemInformation)
                    };


                    /**
                     *  \note   These print names are mostly for display and debugging purposes, and they are not gauranteed to be safe for
                     *          persistence (so be sure to version).
                     */
                    constexpr   Configuration::EnumNames<MountedDeviceType>    Stroika_Enum_Names(MountedDeviceType)
                    {
                        Configuration::EnumNames<MountedDeviceType>::BasicArrayInitializer {
                            {
                                { MountedDeviceType::eRemovableDisk, L"Removable-Disk" },
                                { MountedDeviceType::eLocalDisk, L"Local-Disk" },
                                { MountedDeviceType::eNetworkDrive, L"Network-Drive" },
                                { MountedDeviceType::eTemporaryFiles, L"Temporary-Files" },
                                { MountedDeviceType::eReadOnlyEjectable, L"Read-Only-Ejectable" },
                                { MountedDeviceType::eSystemInformation, L"System-Information" },
                            }
                        }
                    };


                    /**
                     *
                     */
                    struct  VolumeInfo {
                        Optional<MountedDeviceType> fMountedDeviceType;
                        Optional<String>            fFileSystemType;
                        Optional<String>            fDeviceOrVolumeName;
                        Optional<String>            fVolumeID;
                        String                      fMountedOnName;
                        Optional<double>            fSizeInBytes;
                        Optional<double>            fAvailableSizeInBytes;
                        Optional<double>            fUsedSizeInBytes;

                        /**
                         *  IOStats represents the # of bytes (fBytesTransfered) and total number of transfers
                         *  (fTotalTransfers) during the given capture interval. It is NOT cummulative.
                         *
                         *  The reason fCombinedIOStats is returned redundantly, is because some system may only be able
                         *  to report totals, and not read/write breakdown. It is the same as Read + Write stats (if all available)
                         */
                        struct  IOStats {
                            Optional<double>    fBytesTransfered;
                            Optional<double>    fTotalTransfers;
                            Optional<double>    fAverageQLength;
                        };
                        Optional<IOStats>   fReadIOStats;
                        Optional<IOStats>   fWriteIOStats;
                        Optional<IOStats>   fCombinedIOStats;
                        /**
                         *  This is the average length - in 'requests' - of the IO Q. This combine input and output requests (read/write).
                         */
                        Optional<double>    fIOQLength;

                        Optional<double>    EstimatedPercentInUse () const
                        {
                            // %InUse = QL / (1 + QL).
                            if (fCombinedIOStats and fCombinedIOStats->fAverageQLength) {
                                double QL = fCombinedIOStats->fAverageQLength.value ();
                                return 100.0 * (QL / (1 + QL));
                            }
                            return Optional<double> ();
                        }
                    };


                    using   Info    =   Containers::Sequence<VolumeInfo>;


                    /**
                     *  For VolumeInfo, Collection<VolumeInfo>, and Sequence<VolumeInfo> types.
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

                        /**
                         */
                        bool    fDiskspaceUsage { true };

                        /**
                         */
                        bool    fIOStatistics { true };

                        /**
                         *  Include 'ram disks' - that are intended to store temporary files a short period (MountedDeviceType::eTemporaryFiles)
                         */
                        bool    fIncludeTemporaryDevices { true };

                        /**
                         *  On unix, there are many 'fake' disks, like 'procfs'. For many purposes, this are uninteresting to list/query.
                         */
                        bool    fIncludeSystemDevices { true };
                    };


                    /**
                     *  Instrument returning Sequence<VolumeInfo> measurements (cross-platform).
                     */
                    Instrument          GetInstrument (Options options = Options ());


                }
            }


            /*
             *  Specialization to improve performance
             */
            template    <>
            Instruments::Filesystem::Info   Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_*/
