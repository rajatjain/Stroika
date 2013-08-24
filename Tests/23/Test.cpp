﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
//  TEST    Foundation::DataExchangeFormat::ObjectVariantMapper
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Configuration/Locale.h"
#include    "Stroika/Foundation/DataExchangeFormat/BadFormatException.h"
#include    "Stroika/Foundation/DataExchangeFormat/ObjectVariantMapper.h"
#include    "Stroika/Foundation/DataExchangeFormat/JSON/Reader.h"
#include    "Stroika/Foundation/DataExchangeFormat/JSON/Writer.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "Stroika/Foundation/IO/FileSystem/BinaryFileOutputStream.h"
#include    "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include    "Stroika/Foundation/Memory/VariantValue.h"
#include    "Stroika/Foundation/Streams/BasicBinaryInputOutputStream.h"
#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Time/DateTime.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;

using   Memory::Byte;
using   Memory::VariantValue;
using   Time::DateTime;





namespace   {
    namespace DoRegressionTests_BasicDataRoundtrips_1_ {
        template    <typename T>
        void    RoundTripTest_ (T v)
        {
            Memory::VariantValue mv =   v;
            VerifyTestResult (mv.As<T> () == v);
        }
        template    <typename T>
        void    RoundTripMinMax_ ()
        {
            RoundTripTest_ (numeric_limits<T>::min ());
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

            SharedContactsConfig_ ()
                : fEnabled (false)
                , fLastSynchronizedAt ()
                , fThisPHRsIDToSharedContactID () {
            }

            bool operator== (const SharedContactsConfig_& rhs) const {
                return fEnabled == rhs.fEnabled and
                       fLastSynchronizedAt == rhs.fLastSynchronizedAt and
                       fThisPHRsIDToSharedContactID == rhs.fThisPHRsIDToSharedContactID
                       ;
            }
        };

        ObjectVariantMapper mapper;

        // register each of your mappable (even private) types
#if     qCompilerAndStdLib_Supports_initializer_lists
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> ( {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fEnabled, L"Enabled"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fLastSynchronizedAt, L"Last-Synchronized-At"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fThisPHRsIDToSharedContactID, L"This-HR-ContactID-To-SharedContactID-Map"),
        }));
#else
        ObjectVariantMapper::StructureFieldInfo kInfo[] = {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fEnabled, L"Enabled"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fLastSynchronizedAt, L"Last-Synchronized-At"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fThisPHRsIDToSharedContactID, L"This-HR-ContactID-To-SharedContactID-Map"),
        };
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> (std::begin (kInfo), std::end (kInfo)));
#endif

        bool newEnabled = true;
        SharedContactsConfig_   tmp;
        tmp.fEnabled = newEnabled;
        tmp.fThisPHRsIDToSharedContactID.Add (L"A", L"B");
        tmp.fLastSynchronizedAt = DateTime (Time::Date (Time::Year (1998), Time::MonthOfYear::eApril, Time::DayOfMonth::e11), Time::TimeOfDay::Parse (L"3pm", locale::classic ()));

        VariantValue v = mapper.FromObject  (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::BasicBinaryInputOutputStream   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            IO::FileSystem::BinaryFileOutputStream tmp (String::FromTString (IO::FileSystem::WellKnownLocations::GetTemporary ()) + L"t.txt");
            JSON::Writer ().Write (v, tmp);
        }

        if (kWrite2FileAsWell_) {
            IO::FileSystem::BinaryFileInputStream tmp (String::FromTString (IO::FileSystem::WellKnownLocations::GetTemporary ()) + L"t.txt");
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmp));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        DoRegressionTests_BasicDataRoundtrips_1_::DoAll ();
        DoRegressionTests_SimpleMapToFromJSON_2_ ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

