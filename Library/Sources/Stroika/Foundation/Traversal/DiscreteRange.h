/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DiscreteRange_h_
#define _Stroika_Foundation_Traversal_DiscreteRange_h_  1

#include    "../StroikaPreComp.h"

#include    "Iterable.h"
#include    "Range.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *      @todo   Consider if clearer having DiscreteRange aggregate Elements() instead of inheriting from
 *              Iterable. Would make construction, and coipying etc cheaper (esp when that feature
 *              is not used).


 >>>> DOCUMENT WHY DsicreateRange<> no longer Iterabkle<?>
    >>> avoids conflict on empty, and provides clearer 'getlength'.
    >>> avoids shared_ptr stuff (REP) for iterablke unles/until used.
    >>> this should allow constexpr discrete_rage!!!
    >>> and see other comments on why - in progress switchover...
    >>> (NOTE - downside is
        -            for (auto i : DiscreteRange<Color> (Optional<Color> (), Optional<Color> ())) {
        +            for (auto i : DiscreteRange<Color> (Optional<Color> (), Optional<Color> ()).Elements ()) {


 *
 *      @todo   Understand what you must do DiscreteRange<int> (1,10) instead of just DiscreteRange (1,10),
 *              and if something reasonable to be done about it - do so (compiler bug or ambiguity due to optional)
 *              or just that template inference of types doesnt work as well as I thought...
 *
 *      @todo   GetNext() not quite right - using 'int'.
 *
 *      @todo   DefaultDiscreteRangeTraits_Enum  : ExplicitDiscreteRangeTraits<T, T::eSTART, T::eLAST, int, unsigned int> {
 *              should use  underlying_type - but not sure why it didnt work easily.
 *
 *      @todo   Try to rewrite using Generator<> code... See if that simplifies things...
 *
 *      @todo   DefaultDiscreteRangeTraits_Enum and DefaultDiscreteRangeTraits_Integral<> should do better
 *              auto-computing the 'differnce' type
 */



#if     qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy
#include    <climits>
#endif



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            namespace   RangeTraits {


                /**
                 */
                template    <typename T, T MIN, T MAX, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
                struct  ExplicitDiscreteRangeTraits  :
                        ExplicitRangeTraits_Integral<T, MIN, MAX, Openness::eClosed, Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
                    static T GetNext (T n)
                    {
                        return static_cast<T> (static_cast<int> (n) + 1);
                    }
                    using       RangeTraitsType     =   ExplicitRangeTraits_Integral<T, MIN, MAX, Openness::eClosed, Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>;
                };


                /**
                 *  DefaultDiscreteRangeTraits_Enum<> can be used to generate an automatic traits object (with bounds)
                 *  if you've applied the Stroika_Define_Enum_Bounds() macro to the given enumeration.
                 */
                template    <typename T>
                struct  DefaultDiscreteRangeTraits_Enum  : ExplicitDiscreteRangeTraits<T, T::eSTART, T::eLAST, int, unsigned int> {
                };


                /**
                 */
#if     !qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy
                template    <typename T>
                struct  DefaultDiscreteRangeTraits_Integral  : ExplicitDiscreteRangeTraits < T, numeric_limits<T>::lowest (), numeric_limits<T>::max (), decltype (T() - T()), typename make_unsigned < decltype (T() - T()) >::type > {
                };
#else
                template    <typename T>
                struct  DefaultDiscreteRangeTraits_Integral  : ExplicitDiscreteRangeTraits < T, 0, 1, decltype (T() - T()), make_unsigned < decltype (T() - T()) >> {
                        };
                template    <>
                struct  DefaultDiscreteRangeTraits_Integral<char>  : ExplicitDiscreteRangeTraits < char, CHAR_MIN, CHAR_MAX, char, unsigned char> {
                };
                template    <>
                struct  DefaultDiscreteRangeTraits_Integral<unsigned char>  : ExplicitDiscreteRangeTraits < unsigned char, 0, UCHAR_MAX, unsigned char, unsigned char> {
                };
                template    <>
                struct  DefaultDiscreteRangeTraits_Integral<short>  : ExplicitDiscreteRangeTraits < short, SHRT_MIN, SHRT_MAX, short, unsigned short> {
                };
                template    <>
                struct  DefaultDiscreteRangeTraits_Integral<unsigned short>  : ExplicitDiscreteRangeTraits < unsigned short, 0, USHRT_MAX, unsigned short, unsigned short> {
                };
                template    <>
                struct  DefaultDiscreteRangeTraits_Integral<int>  : ExplicitDiscreteRangeTraits < int, INT_MIN, INT_MAX, int, unsigned int> {
                };
                template    <>
                struct  DefaultDiscreteRangeTraits_Integral<unsigned int>  : ExplicitDiscreteRangeTraits < unsigned int, 0, UINT_MAX, unsigned int, unsigned int> {
                };
                template    <>
                struct  DefaultDiscreteRangeTraits_Integral<long>  : ExplicitDiscreteRangeTraits < long, LONG_MIN, LONG_MAX, long, unsigned long> {
                };
                template    <>
                struct  DefaultDiscreteRangeTraits_Integral<unsigned long>  : ExplicitDiscreteRangeTraits < unsigned long, 0, ULONG_MAX, unsigned long, unsigned long> {
                };
                template    <>
                struct  DefaultDiscreteRangeTraits_Integral<long long>  : ExplicitDiscreteRangeTraits < long long, LLONG_MIN, LLONG_MAX, long long, unsigned long long> {
                };
                template    <>
                struct  DefaultDiscreteRangeTraits_Integral<unsigned long long>  : ExplicitDiscreteRangeTraits < unsigned long long, 0, ULLONG_MAX, unsigned long long, unsigned long long> {
                };
#endif


                /**
                 */
                template    <typename T>
                struct  DefaultDiscreteRangeTraits : conditional <
                        is_enum<T>::value,
                        DefaultDiscreteRangeTraits_Enum<T>,
                        DefaultDiscreteRangeTraits_Integral<T>
                        >::type {
                };


            }


            /**
             *  \par Example Usage
             *      \code
             *      vector<int> v = DiscreteRange<int> (1,10).As<vector<int>> ();
             *      // equiv to vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
             *      \endcode
             *
             *  \par OR
             *      \code
             *      for (auto i : DiscreteRange<int> (1,10)) {
             *          ...i = 1..10
             *      }
             *      \endcode
             */
            template    <typename T, typename TRAITS = RangeTraits::DefaultDiscreteRangeTraits<T>>
            class   DiscreteRange : public Range<T, typename TRAITS::RangeTraitsType> {
            private:
                using       inherited     =   Range<T, typename TRAITS::RangeTraitsType>;

            public:
                /**
                 */
                using   ElementType =   typename inherited::ElementType;

            public:
                /**
                 */
                using   SignedDifferenceType      =   typename inherited::SignedDifferenceType;

            public:
                /**
                 */
                using   UnsignedDifferenceType      =   typename inherited::UnsignedDifferenceType;

            private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                struct   MyIteratorRep_;

            public:
                /**
                 *  DiscreteRange () with no arguments produces an empty sequence.
                 *
                 *  \req begin <= end (after substitution of optional values)
                 */
                explicit DiscreteRange () = default;
                explicit DiscreteRange (T begin, T end);
                explicit DiscreteRange (const Memory::Optional<T>& begin, const Memory::Optional<T>& end);
                explicit DiscreteRange (const Range<T, typename TRAITS::RangeTraitsType>& r);

            public:
                /**
                 *  Like Range<>::FullRange () but returing a DiscreteRange<> type.
                 */
                static  DiscreteRange<T, TRAITS> FullRange ();

            public:
                /**
                 *  Like Range<>::Intersection (), but returing a DiscreteRange<> type.
                 */
                nonvirtual  Range<T, TRAITS>            Intersection (const Range<T, TRAITS>& rhs) const;
                nonvirtual  DiscreteRange<T, TRAITS>    Intersection (const DiscreteRange<T, TRAITS>& rhs) const;

            public:
                /**
                 *  Like Range<>::UnionBounds (), but returing a DiscreteRange<> type.
                 */
                nonvirtual  Range<T, TRAITS>            UnionBounds (const Range<T, TRAITS>& rhs) const;
                nonvirtual  DiscreteRange<T, TRAITS>    UnionBounds (const DiscreteRange<T, TRAITS>& rhs) const;

            public:
                /**
                 *  This returns the number of points from lower bound to upperbound inclusive.
                 *  This equals GetDistancespanned () + 1 (roughly).
                 *  If (empty ()) .... this returns 0;
                 */
                nonvirtual  UnsignedDifferenceType GetNumberOfContainedPoints () const;

            public:
                /**
                 *  \req not empty
                 *  \req the DiscreteRange produced by applying the given offset to *this remains valid with respect to the constraints on this DiscreteRange.
                 */
                nonvirtual  DiscreteRange Offset (SignedDifferenceType o) const;

            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *      DisjointDiscreteRange<DiscreteRange<int>> t;
                 *      for (T i : t.Elements ()) {
                 *      }
                 *      \endcode
                 *
                 *  Elements () makes no guarantess about whether or not modifications to the underlying DisjointDiscreteRange<> will
                 *  appear in the Elements() Iterable<T>.
                 */
                nonvirtual  Iterable<T>   Elements () const;

            public:
                /**EXPERIEMNTAL as of 2.0a73
                */
                operator Iterable<T> () const;
                /**EXPERIEMNTAL as of 2.0a73
                */
                Iterator<T> begin () const;
                /**EXPERIEMNTAL as of 2.0a73
                */
                Iterator<T> end () const;

            private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                struct  MyIterable_;
            };


            /**
             *  Intersection ()
             */
            template    <typename T, typename TRAITS>
            DiscreteRange<T, TRAITS>   operator^ (const DiscreteRange<T, TRAITS>& lhs, const DiscreteRange<T, TRAITS>& rhs);


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
