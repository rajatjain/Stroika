/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Timezone_inl_
#define _Stroika_Foundation_Time_Timezone_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace Time {

            /*
             ********************************************************************************
             *********************************** Timezone ***********************************
             ********************************************************************************
             */
            inline constexpr Timezone::Timezone (TZ_ tz) noexcept
                : fTZ_ (tz)
                , fBiasInMinutesFromUTC_ (0)
            {
            }
            inline constexpr Timezone::Timezone (BiasInMinutesFromUTCType biasInMinutesFromUTC)
                : fTZ_ (TZ_::eFixedOffsetBias)
                , fBiasInMinutesFromUTC_ (biasInMinutesFromUTC)
            {
            }
            inline Timezone::BiasInMinutesFromUTCType Timezone::GetBiasInMinutesFromUTCType (const Date& date, const TimeOfDay& tod) const
            {
                switch (fTZ_) {
                    case TZ_::eUTC:
                        return 0;
                    case TZ_::eFixedOffsetBias:
                        return fBiasInMinutesFromUTC_;
                    case TZ_::eLocalTime:
                        return static_cast<BiasInMinutesFromUTCType> (-GetLocaltimeToGMTOffset (IsDaylightSavingsTime (date, tod)));
                    default:
                        AssertNotReached ();
                        return 0;
                }
            }
            inline make_signed<time_t>::type Timezone::GetOffset (const Date& date, const TimeOfDay& tod) const
            {
                return 60 * GetBiasInMinutesFromUTCType (date, tod);
            }
            inline constexpr bool Timezone::operator== (const Timezone& rhs) const
            {
                return fTZ_ == rhs.fTZ_ and fBiasInMinutesFromUTC_ == rhs.fBiasInMinutesFromUTC_;
            }
            inline constexpr bool Timezone::operator!= (const Timezone& rhs) const
            {
                return fTZ_ != rhs.fTZ_ or fBiasInMinutesFromUTC_ != rhs.fBiasInMinutesFromUTC_;
            }
#if !qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
            constexpr const Timezone                   Timezone::kUTC{Timezone::TZ_::eUTC};
            constexpr const Timezone                   Timezone::kLocalTime{Timezone::TZ_::eLocalTime};
            constexpr const Memory::Optional<Timezone> Timezone::kUnknown{};
#endif
        }
    }
}
#endif /*_Stroika_Foundation_Time_Timezone_inl_*/
