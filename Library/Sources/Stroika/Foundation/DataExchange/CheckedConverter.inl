/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_CheckedConverter_inl_
#define _Stroika_Foundation_DataExchange_CheckedConverter_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    <type_traits>

#include    "../Characters/String_Constant.h"
#include    "../Math/Common.h"
#include    "../Execution/Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /*
             ********************************************************************************
             ***************************** DataExchange::Private_ ***************************
             ********************************************************************************
             */
            namespace Private_ {
                template    <typename T>
                inline  typename std::enable_if < !std::is_floating_point<T>::value, T >::type CheckedConverter_Range_Helper_Pinner_ (T t, T lower, T upper)
                {
                    return t;
                }
                template    <typename T>
                inline  typename std::enable_if<std::is_floating_point<T>::value, T>::type  CheckedConverter_Range_Helper_Pinner_ (T t, T lower, T upper)
                {
                    return Math::PinToSpecialPoint (Math::PinToSpecialPoint (t, lower), upper);
                }
            }


            /*
             ********************************************************************************
             ******************** DataExchange::CheckedConverter_Range **********************
             ********************************************************************************
             */
            template    <typename   RANGE_TYPE>
            RANGE_TYPE  CheckedConverter_Range (const typename RANGE_TYPE::ElementType& s, const typename RANGE_TYPE::ElementType& e)
            {
                using   Characters::String_Constant;
                typename    RANGE_TYPE::ElementType useS    =   Private_::CheckedConverter_Range_Helper_Pinner_ (s, RANGE_TYPE::TraitsType::kLowerBound, RANGE_TYPE::TraitsType::kUpperBound);
                typename    RANGE_TYPE::ElementType useE    =   Private_::CheckedConverter_Range_Helper_Pinner_ (e, RANGE_TYPE::TraitsType::kLowerBound, RANGE_TYPE::TraitsType::kUpperBound);
                // Note: these checks MUST use <= and >= and IGNORE openness, because the bounds need not be in the range.
                if (not (RANGE_TYPE::TraitsType::kLowerBound <= useS)) {
                    Execution::DoThrow (BadFormatException (String_Constant (L"Value < RangeType lower bounds")));
                }
                if (not (useS <= useE)) {
                    Execution::DoThrow (BadFormatException (String_Constant (L"Range start must be less than end")));
                }
                if (not (useE <= RANGE_TYPE::TraitsType::kUpperBound)) {
                    Execution::DoThrow (BadFormatException (String_Constant (L"Range end must be less than Range traits end")));
                }
                return RANGE_TYPE (useS, useE);
            }


            /*
             ********************************************************************************
             *************** DataExchange::CheckedConverter_ValueInRange ********************
             ********************************************************************************
             */
            template    <typename   RANGE_TYPE>
            typename RANGE_TYPE::ElementType  CheckedConverter_ValueInRange (typename RANGE_TYPE::ElementType val, const RANGE_TYPE& r)
            {
                using   Characters::String_Constant;
                typename    RANGE_TYPE::ElementType useVal    =   Private_::CheckedConverter_Range_Helper_Pinner_ (val, RANGE_TYPE::TraitsType::kLowerBound, RANGE_TYPE::TraitsType::kUpperBound);
                if (not (r.GetLowerBound () <= useVal)) {
                    Execution::DoThrow (BadFormatException (String_Constant (L"Value out of range (too low)")));
                }
                if (not (useVal <= r.GetUpperBound ())) {
                    Execution::DoThrow (BadFormatException (String_Constant (L"Value out of range (exceeds max)")));
                }
                return useVal;
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchange_CheckedConverter_inl_*/
