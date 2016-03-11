﻿/*
* Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
*/
//  TEST    Foundation::DataExchangeFormat::ObjectVariantMapper
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Configuration/Enumeration.h"
#include    "Stroika/Foundation/Configuration/Locale.h"
#include    "Stroika/Foundation/Containers/Bijection.h"
#include    "Stroika/Foundation/DataExchange/BadFormatException.h"
#include    "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include    "Stroika/Foundation/DataExchange/JSON/Reader.h"
#include    "Stroika/Foundation/DataExchange/JSON/Writer.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include    "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include    "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include    "Stroika/Foundation/Streams/MemoryStream.h"
#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Time/DateTime.h"
#include    "Stroika/Foundation/Time/Duration.h"
#include    "Stroika/Foundation/Traversal/Range.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;

using   Memory::Byte;
using   Time::Date;
using   Time::DateTime;
using   Time::Duration;
using   Time::TimeOfDay;





namespace   {
    namespace DoRegressionTests_BasicDataRoundtrips_1_ {
        template    <typename T>
        void    RoundTripTest_ (T v)
        {
            VariantValue mv = v;
            VerifyTestResult (mv.As<T> () == v);
        }
        template    <typename T>
        void    RoundTripMinMax_ ()
        {
            RoundTripTest_ (numeric_limits<T>::lowest ());
            RoundTripTest_ (numeric_limits<T>::max ());
        }
        void    DoAll ()
        {
            RoundTripMinMax_<int> ();
            RoundTripMinMax_<unsigned int> ();
            RoundTripMinMax_<long> ();
            RoundTripMinMax_<unsigned long> ();
            RoundTripMinMax_<long long> ();
            RoundTripMinMax_<unsigned long long> ();
            RoundTripMinMax_<int8_t> ();
            RoundTripMinMax_<uint8_t> ();
            RoundTripMinMax_<int16_t> ();
            RoundTripMinMax_<uint16_t> ();
            RoundTripMinMax_<int32_t> ();
            RoundTripMinMax_<uint32_t> ();
            RoundTripMinMax_<int64_t> ();
            RoundTripMinMax_<uint64_t> ();
            RoundTripMinMax_<float> ();
            RoundTripMinMax_<float> ();
            RoundTripMinMax_<double> ();
            RoundTripMinMax_<double> ();
            //enum class Fred { Barny };
            //RoundTripMinMax_<Fred> ();
            RoundTripTest_<int> (3);
        }
    }
}

namespace   {
    void    DoRegressionTests_SimpleMapToFromJSON_2_ ()
    {
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            bool                        fEnabled;
            DateTime                    fLastSynchronizedAt;
            Mapping<String, String>     fThisPHRsIDToSharedContactID;
            Bijection<String, String>   fThisPHRsIDToSharedContactID2;

            SharedContactsConfig_ ()
                : fEnabled (false)
                , fLastSynchronizedAt ()
                , fThisPHRsIDToSharedContactID ()
                , fThisPHRsIDToSharedContactID2 ()
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return fEnabled == rhs.fEnabled and
                       fLastSynchronizedAt == rhs.fLastSynchronizedAt and
                       fThisPHRsIDToSharedContactID == rhs.fThisPHRsIDToSharedContactID and
                       fThisPHRsIDToSharedContactID2 == rhs.fThisPHRsIDToSharedContactID2
                       ;
            }
        };

        ObjectVariantMapper mapper;

        mapper.AddCommonType<Bijection<String, String>> ();

        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        // register each of your mappable (even private) types
        mapper.AddClass<SharedContactsConfig_> ({
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fEnabled), L"Enabled" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fLastSynchronizedAt), L"Last-Synchronized-At" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fThisPHRsIDToSharedContactID), L"This-HR-ContactID-To-SharedContactID-Map" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fThisPHRsIDToSharedContactID2), L"This-HR-ContactID-To-SharedContactID-Bijection"},
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

        bool newEnabled = true;
        SharedContactsConfig_   tmp;
        tmp.fEnabled = newEnabled;
        tmp.fThisPHRsIDToSharedContactID.Add (L"A", L"B");
        tmp.fThisPHRsIDToSharedContactID2.Add (L"A", L"B");
        tmp.fLastSynchronizedAt = DateTime (Time::Date (Time::Year (1998), Time::MonthOfYear::eApril, Time::DayOfMonth::e11), Time::TimeOfDay::Parse (L"3pm", locale::classic ()));

        VariantValue v = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::MemoryStream<Byte>   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            IO::FileSystem::FileOutputStream tmp (IO::FileSystem::WellKnownLocations::GetTemporary () + L"t.txt");
            JSON::Writer ().Write (v, tmp);
        }

        if (kWrite2FileAsWell_) {
            IO::FileSystem::FileInputStream tmp (IO::FileSystem::WellKnownLocations::GetTemporary () + L"t.txt");
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmp));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}

namespace   {
    void    DoRegressionTests_SimpleMapToFromJSON_3_ ()
    {
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            int fInt1;
            unsigned long long fInt2;
            long long fInt3;
            int32_t fInt4;

            SharedContactsConfig_ ()
                : fInt1 (0)
                , fInt2 (0)
                , fInt3 (0)
                , fInt4 (0)
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return
                    fInt1 == rhs.fInt1 and
                    fInt2 == rhs.fInt2 and
                    fInt3 == rhs.fInt3 and
                    fInt4 == rhs.fInt4
                    ;
            }
        };

        ObjectVariantMapper mapper;

        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        // register each of your mappable (even private) types
        mapper.AddClass<SharedContactsConfig_> ({
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fInt1), L"Int1" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fInt2), L"Int2" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fInt3), L"Int3" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fInt4), L"Int4" },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

        SharedContactsConfig_   tmp;
        tmp.fInt1 = 2;
        tmp.fInt2 = numeric_limits<decltype (tmp.fInt2)>::max ();
        tmp.fInt3 = numeric_limits<decltype (tmp.fInt3)>::max ();
        tmp.fInt4 = numeric_limits<decltype (tmp.fInt4)>::min ();

        VariantValue v = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::MemoryStream<Byte>   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"t.txt";
            JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream (fileName));
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::FileInputStream (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}




namespace   {
    void    DoRegressionTests_SimpleMapRangeTypes_4_ ()
    {
        using   namespace Traversal;
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            Range<int>  fIntRange;
            DiscreteRange<int> fDiscIntRange2;

            SharedContactsConfig_ ()
                : fIntRange (-3, 99)
                , fDiscIntRange2 (4, 19)
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return
                    fIntRange == rhs.fIntRange
                    and fDiscIntRange2 == rhs.fDiscIntRange2
                    ;
            }
        };

        ObjectVariantMapper mapper;

        mapper.AddCommonType<Range<int>> ();
        mapper.AddCommonType<DiscreteRange<int>> ();

        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<SharedContactsConfig_> ({
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fIntRange), L"fIntRange" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fDiscIntRange2), L"fDiscIntRange2" },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

        SharedContactsConfig_   tmp;
        tmp.fIntRange = Range<int> (1, 10);
        tmp.fDiscIntRange2 = DiscreteRange<int> (38, 39);
        VariantValue v = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::MemoryStream<Byte>   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"4.txt";
            JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream (fileName));
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::FileInputStream (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}







namespace   {
#if     qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy
    enum class Fred {
        a = -3,
        b,
        c,
        d,
        e,
        f,
        g,
        h,

        Stroika_Define_Enum_Bounds (a, h)
    };
#endif
    void    DoRegressionTests_SimpleEnumTypes_5_ ()
    {
        using   namespace Traversal;
        const bool kWrite2FileAsWell_ = true;      // just for debugging

#if     !qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy
        enum class Fred {
            a = -3,
            b,
            c,
            d,
            e,
            f,
            g,
            h,

            Stroika_Define_Enum_Bounds (a, h)
        };
#endif
        const Configuration::EnumNames<Fred>   Fred_NAMES = {
            { Fred::a, L"a" },
            { Fred::b, L"b" },
            { Fred::c, L"c" },
            { Fred::d, L"d" },
            { Fred::e, L"e" },
            { Fred::f, L"f" },
            { Fred::g, L"g" },
            { Fred::h, L"h" },
        };
        struct SharedContactsConfig_ {
            Fred fEnum1;

            SharedContactsConfig_ ()
                : fEnum1 (Fred::a)
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return
                    fEnum1 == rhs.fEnum1
                    ;
            }
        };

        {
            ObjectVariantMapper mapper;

            mapper.Add (ObjectVariantMapper::MakeCommonSerializer_NamedEnumerations<Fred> (Fred_NAMES));
            DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
            mapper.AddClass<SharedContactsConfig_> ({
                ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fEnum1), L"fEnum1" },
            });
            DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

            SharedContactsConfig_   tmp;
            tmp.fEnum1 = Fred::b;
            VariantValue v = mapper.FromObject (tmp);

            // at this point - we should have VariantValue object with "Enabled" field.
            // This can then be serialized using

            Streams::MemoryStream<Byte>   tmpStream;
            JSON::Writer ().Write (v, tmpStream);

            if (kWrite2FileAsWell_) {
                String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"5.txt";
                JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream (fileName));
                SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::FileInputStream (fileName)));
            }

            // THEN deserialized, and mapped back to C++ object form
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
            VerifyTestResult (tmp2 == tmp);
        }

        {
            ObjectVariantMapper mapper;

            mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<Fred> (Bijection<Fred, String> (Fred_NAMES)));
            DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
            mapper.AddClass<SharedContactsConfig_> ({
                ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fEnum1), L"fEnum1" },
            });
            DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

            SharedContactsConfig_   tmp;
            tmp.fEnum1 = Fred::b;
            VariantValue v = mapper.FromObject (tmp);

            // at this point - we should have VariantValue object with "Enabled" field.
            // This can then be serialized using

            Streams::MemoryStream<Byte>   tmpStream;
            JSON::Writer ().Write (v, tmpStream);

            if (kWrite2FileAsWell_) {
                String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"5.txt";
                JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream (fileName));
                SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::FileInputStream (fileName)));
            }

            // THEN deserialized, and mapped back to C++ object form
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
            VerifyTestResult (tmp2 == tmp);
        }
    }
}







namespace   {
    void    DoRegressionTests_DurationsDateTime_6_ ()
    {
        using   namespace Traversal;
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            Duration fDuration1;
            DateTime fDateTime1;
            DateTime fDate1;
            TimeOfDay fTimeOfDay1;

            SharedContactsConfig_ ()
                : fDuration1 (chrono::milliseconds (200))
                , fDateTime1 ()
                , fDate1 ()
                , fTimeOfDay1 ()
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return
                    fDuration1 == rhs.fDuration1
                    and fDateTime1 == rhs.fDateTime1
                    and fDate1 == rhs.fDate1
                    and fTimeOfDay1 == rhs.fTimeOfDay1
                    ;
            }
        };

        ObjectVariantMapper mapper;
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<SharedContactsConfig_> ({
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fDuration1), L"fDuration1" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fDateTime1), L"fDateTime1" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fDate1), L"fDate1" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fTimeOfDay1), L"fTimeOfDay1" },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

        SharedContactsConfig_   tmp;
        tmp.fDate1 = Date (Time::Year (2001), Time::MonthOfYear::eFebruary, Time::DayOfMonth::e12);
        tmp.fDateTime1 = DateTime (Date (Time::Year (2001), Time::MonthOfYear::eFebruary, Time::DayOfMonth::e12), Time::TimeOfDay::Parse (L"3pm", locale::classic ()));
        tmp.fTimeOfDay1 = tmp.fDateTime1.GetTimeOfDay ();
        tmp.fTimeOfDay1 = TimeOfDay (tmp.fTimeOfDay1.GetAsSecondsCount () + 60);
        VariantValue v = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::MemoryStream<Byte>   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"6.txt";
            JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream (fileName));
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::FileInputStream (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}






namespace   {
    void    DoRegressionTests_VariantValue_7_ ()
    {
        using   namespace Traversal;
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            VariantValue fVV1;

            SharedContactsConfig_ ()
                : fVV1 ()
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return
                    fVV1 == rhs.fVV1
                    ;
            }
        };

        ObjectVariantMapper mapper;
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<SharedContactsConfig_> ({
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fVV1), L"fVV1" },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

        SharedContactsConfig_   tmp;
        tmp.fVV1 = Date (Time::Year (2001), Time::MonthOfYear::eFebruary, Time::DayOfMonth::e12);
        VariantValue v = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::MemoryStream<Byte>   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"7.txt";
            JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream (fileName));
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::FileInputStream (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}












namespace   {
    void    DoRegressionTests_MakeCommonSerializer_8_ ()
    {
        using   namespace Traversal;
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            int                     fInt1;
            Memory::Optional<int>   fInt2;
            Mapping<int, int>       fMapping1;
            Sequence<int>           fSequence1;
            int                     fBasicArray1[5];
            Set<int>                fSet1_;
            vector<int>             fVector1_;

            SharedContactsConfig_ ()
                : fInt1 (3)
            {
                memset (&fBasicArray1, 0, sizeof (fBasicArray1));
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                if (memcmp (fBasicArray1, rhs.fBasicArray1, sizeof (fBasicArray1)) != 0) {
                    return false;
                }
                return
                    fInt1 == rhs.fInt1 and
                    fInt2 == rhs.fInt2 and
                    fMapping1 == rhs.fMapping1 and
                    fSequence1 == rhs.fSequence1 and
                    fSet1_ == rhs.fSet1_ and
                    fVector1_ == rhs.fVector1_
                    ;
            }
        };

        ObjectVariantMapper mapper;

        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<Memory::Optional<int>> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<Mapping<int, int>> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<Sequence<int>> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<vector<int>> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<Set<int>> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<int[5]> ());

        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<SharedContactsConfig_> ({
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fInt1), L"fInt1" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fInt2), L"fInt2" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fMapping1), L"fMapping1" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fSequence1), L"fSequence1" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fBasicArray1), L"fBasicArray1" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fSet1_), L"fSet1_" },
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (SharedContactsConfig_, fVector1_), L"fVector1_" },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");


        SharedContactsConfig_   tmp;
        tmp.fInt1 = 4;
        tmp.fInt2 = 6;
        tmp.fSequence1.Append (5);
        tmp.fMapping1.Add (3, 5);
        tmp.fBasicArray1[3] = 5;
        tmp.fSet1_.Add (193);
        tmp.fVector1_.push_back (3);
        tmp.fVector1_.push_back (-91);
        VariantValue v = mapper.FromObject (tmp);

        Streams::MemoryStream<Byte>   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"8.txt";
            JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream (fileName));
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::FileInputStream (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}










namespace   {
    void    DoRegressionTests_Subclass_9_ ()
    {
        using   namespace Traversal;

        struct BaseObj_ {
            int fVV1 {};
            bool operator== (const BaseObj_& rhs) const
            {
                return fVV1 == rhs.fVV1;
            }
        };

        struct Derived_ : BaseObj_ {
            int fVV2 {};
            bool operator== (const Derived_& rhs) const
            {
                return BaseObj_::operator== (rhs) and fVV2 == rhs.fVV2;
            }
        };

        ObjectVariantMapper mapper;
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<BaseObj_> ({
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (BaseObj_, fVV1), L"fVV1" },
        });
        mapper.AddSubClass<Derived_, BaseObj_> ({
            ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Derived_, fVV2), L"fVV2" },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

        Derived_   tmp;
        tmp.fVV1 = 55;
        tmp.fVV2 = 345;
        VariantValue v = mapper.FromObject (tmp);

        Streams::MemoryStream<Byte>   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        // THEN deserialized, and mapped back to C++ object form
        Derived_    tmp2 = mapper.ToObject<Derived_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}







namespace   {
    void    DoRegressionTests_ ()
    {
        DoRegressionTests_BasicDataRoundtrips_1_::DoAll ();
        DoRegressionTests_SimpleMapToFromJSON_2_ ();
        DoRegressionTests_SimpleMapToFromJSON_3_ ();
        DoRegressionTests_SimpleMapRangeTypes_4_ ();
        DoRegressionTests_SimpleEnumTypes_5_ ();
        DoRegressionTests_DurationsDateTime_6_ ();
        DoRegressionTests_VariantValue_7_ ();
        DoRegressionTests_MakeCommonSerializer_8_ ();
        DoRegressionTests_Subclass_9_ ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

