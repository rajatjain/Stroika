/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DiscreteRange_h_
#define _Stroika_Foundation_Traversal_DiscreteRange_h_  1

#include    "../StroikaPreComp.h"

#include    "Iterable.h"
#include    "Range.h"



/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  TODO:
 *      @todo   Conflict in names begin/end between Iterator and Range<> are INCONVENIENT and UNSESIRBALE.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


#if     qSupportTemplateParamterOfNumericLimitsMinMax
            template    <typename T, T MIN = numeric_limits<T>::min (), T MAX = numeric_limits<T>::max (), typename SIGNED_DIFF_TYPE = int, typename UNSIGNED_DIFF_TYPE = unsigned int>
            struct  DefaultDiscreteRangeTraits_Enum  : DefaultRangeTraits<T, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, MIN, MAX, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
#else
            template    <typename T, typename SIGNED_DIFF_TYPE = int, typename UNSIGNED_DIFF_TYPE = unsigned int>
            struct  DefaultDiscreteRangeTraits_Enum  : DefaultRangeTraits_Template_numericLimitsBWA<T, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, T::eSTART, T::eLAST, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
#endif
                static T GetNext (T n) {
                    return static_cast<T> (static_cast<int> (n) + 1);
                }
#if     qSupportTemplateParamterOfNumericLimitsMinMax
                typedef DefaultRangeTraits<T, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, MIN, MAX, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>   RangeTraitsType;
#else
                typedef DefaultRangeTraits_Template_numericLimitsBWA<T, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, T::eSTART, T::eLAST, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>  RangeTraitsType;
#endif
            };


            /**
             */
#if     qSupportTemplateParamterOfNumericLimitsMinMax
            template    <typename T, T MIN = numeric_limits<T>::min (), T MAX = numeric_limits<T>::max (), typename SIGNED_DIFF_TYPE = int, typename UNSIGNED_DIFF_TYPE = unsigned int>
            struct  DefaultDiscreteRangeTraits_Arithmetic : DefaultRangeTraits<T, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, MIN, MAX, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
                static T GetNext (T n) {
                    return static_cast<T> (static_cast<int> (n) + 1);
                }
                typedef DefaultRangeTraits<T, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, MIN, MAX, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>   RangeTraitsType;
            };
#else
            template    <typename T, typename SIGNED_DIFF_TYPE = int, typename UNSIGNED_DIFF_TYPE = unsigned int>
            struct  DefaultDiscreteRangeTraits_Arithmetic : DefaultRangeTraits<T, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
                static T GetNext (T n) {
                    return static_cast<T> (static_cast<int> (n) + 1);
                }
                typedef DefaultRangeTraits<T, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> RangeTraitsType;
            };
#endif



#if     qSupportTemplateParamterOfNumericLimitsMinMax
            template    <typename T, T MIN = numeric_limits<T>::min (), T MAX = numeric_limits<T>::max (), typename SIGNED_DIFF_TYPE = int, typename UNSIGNED_DIFF_TYPE = unsigned int>
            struct  DefaultDiscreteRangeTraits : conditional<is_enum<T>::value, DefaultDiscreteRangeTraits_Enum<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>, DefaultDiscreteRangeTraits_Arithmetic<T, MIN, MAX, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>>::type {
            };
#else
            template    <typename T, typename SIGNED_DIFF_TYPE = int, typename UNSIGNED_DIFF_TYPE = unsigned int>
            struct  DefaultDiscreteRangeTraits : conditional<is_enum<T>::value, DefaultDiscreteRangeTraits_Enum<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>, DefaultDiscreteRangeTraits_Arithmetic<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>>::type {
            };
#endif


            /**
             *  EXAMPLE USAGE:
             *      vector<int> v = DiscreteRange (1,10).As<vector<int>> ();
             *      // equiv to vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
             *
             *  OR:
             *      for (auto i : DiscreteRange (1,10)) {
             *          ...i = 1..10
             *      }
             */
            template    <typename T, typename TRAITS = DefaultDiscreteRangeTraits<T>>
            class  DiscreteRange : public Range<T, typename TRAITS::RangeTraitsType>, public Iterable<T> {
            private:
                typedef Range<T, typename TRAITS::RangeTraitsType>  inherited_RangeType;

            private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
            public:
#endif
                struct   MyIteratorRep_;
                struct   MyIteratableRep_;

            public:
                /**
                 */
                explicit DiscreteRange (const Memory::Optional<T>& begin, const Memory::Optional<T>& end);

            public:
                /**
                 */
                static  DiscreteRange<T, TRAITS> FullRange ();

            public:
                nonvirtual  bool empty () const;

            public:
                nonvirtual  Iterator<T> begin () const;
                nonvirtual  Iterator<T> end () const;
            };



        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "DiscreteRange.inl"

#endif  /*_Stroika_Foundation_Traversal_DiscreteRange_h_ */
