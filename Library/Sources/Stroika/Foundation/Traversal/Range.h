/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Range_h_
#define _Stroika_Foundation_Traversal_Range_h_  1

#include    "../StroikaPreComp.h"

#include    <limits>

#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"

#include    "Iterator.h"



/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  NOTES:      INSPIRED BY READING ABOUT RUBY, but in the end, mostly
 *              based on HealthFrame's DateRangeType/DateTimeRangeType code.
 *
 *  TODO:
 *
 *          @todo   Consider possibly defining some default parameters for ExplicitRangeTraits<> - like
 *                  size/diff types - probably automatically computed from traits info.
 *
 *                  Or - at least - provide alternate template that has those two diff types autocomputed.
 *
 *          @todo   Carefully review intersection/unionbounds code for new open/closed parameters. Either make sure
 *                  it works or at least more carefully document in method headers the quirks of the
 *                  chosen definition.
 *
 *          @todo   Consider if we want to re-instate Range<T, TRAITS>::Overlaps - but think through and document
 *                  definition clearly.
 *
 *          @todo   Document why no operator< support (compare interface) - cuz no obvious well-ordering? Could well order on
 *                  LHS, and then when equal on RHS, but that wouldn't make sense for all applicaitons.
 *
 *          @todo   This this code with Date/Time/etc - and add specailizations if needed to make it work well.
 */



#if     !qSupportTemplateParamterOfNumericLimitsMinMax
#include    <climits>
#endif
namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *  Common defines for range-types.
             *
             *  \em Design Note
             *      We used base class instead of namespace so 'Openness' name can be injected directly into the used classes.
             *      There maybe a better way...
             */
            class   RangeBase {
            public:
                /**
                 *  Openness is used to define whether an end of a range is open or closed. Open means
                 *  not containing the endpoint, and closed means containing the endpoint.
                 */
                enum    class   Openness { eOpen, eClosed };
            };


            /**
             *  ExplicitRangeTraits<> can be used to specify in line line (type) all the details for the range functionality
             *  for a given type T.
             */
            template    <typename T, T MIN, T MAX, RangeBase::Openness BEGIN_OPEN, RangeBase::Openness END_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            struct  ExplicitRangeTraits : public RangeBase {
                typedef T                   ElementType;
                typedef SIGNED_DIFF_TYPE    SignedDifferenceType;
                typedef UNSIGNED_DIFF_TYPE  UnsignedDifferenceType;

                static  constexpr   Openness    kBeginOpenness  =   BEGIN_OPEN;
                static  constexpr   Openness    kEndOpenness    =   END_OPEN;

#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                static  constexpr T kMin    =   MIN;
                static  constexpr T kMax    =   MAX;
#else
                static  const T kMin;
                static  const T kMax;
#endif
            };


            /**
             *  DefaultRangeTraits<> is generally used automatically - when you construct a Range<> object without
             *  specifying traits.
             *
             *  This defaults to a half-open (lhs closed, rhs-open) range.
             */
#if     qSupportTemplateParamterOfNumericLimitsMinMax
            template    <typename T>
            struct  DefaultRangeTraits  : ExplicitRangeTraits<T, numeric_limits<T>::min (), numeric_limits<T>::max (), RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, int, unsigned int> {
            };
#else
            template    <typename T>
            struct  DefaultRangeTraits  : ExplicitRangeTraits<T, 0, 1, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, int, unsigned int> {
#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                static  const T kMin    =   numeric_limits<T>::min ();
                static  const T kMax    =   numeric_limits<T>::max ();
#else
                static  const T kMin;
                static  const T kMax;
#endif
            };
#endif


            /**
             *  A Range<> is analagous to a mathematical range. It's left and and its right side can
             *  be optionally open or closed.
             *  begin/end similar to Ruby range - except that end is always EXCLUDED (like C++ iterators -
             *  end refers to past the end).
             *
             *  Somewhat inspired by, and at least influenced by, the definition in
             *      http://ruby-doc.org/core-2.0/Range.html
             *  However - where Ruby has one type "Range" - we have "Range" and DiscreteRange" - and some ruby Range methods/properties
             *  are expressed only in DiscreteRange<>
             *
             *  Note - you can do Range<float>, but cannot do DiscreteRange<float> - but can do DiscreteRange<int>.
             *
             *  A note about an empty range. All empty ranges (of the same type) are equal to one another. It is illegal
             *  to ask for the start or end of an empty range. Empty ranges contain no points.
             *
             *  Since a range is half/open/closed by default, this means that
             *      Range<int> (1,1) == Range(3,3) would be true, since the are both empty.
             *
             */
            template    <typename T, typename TRAITS = DefaultRangeTraits<T>>
            class   Range : public RangeBase {
            public:
                /**
                 */
                typedef T   ElementType;

            public:
                /**
                 */
                typedef TRAITS   TraitsType;

            public:
                Range() {
                    *this = EmptyRange ();//tmphack - so works with objectvariantmapper -- LGP 2013-10-15
                }

            public:
                /**
                 *  Optional values - if omitted - are replaced with the TRAITS::kMin and TRAITS::kMax values.
                 *
                 *  \req begin <= end (after substitution of optional values)
                 *
                 *  Use @EmptyRange () to create an empty range object.
                 */
                explicit Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end);

            public:
                /**
                 */
                static  Range<T, TRAITS> EmptyRange ();

            public:
                /**
                 */
                static  Range<T, TRAITS> FullRange ();

            public:
                /**
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  end-begin, or distance from begin to end of the range. This is zero iff empty.
                 */
                nonvirtual  typename TRAITS::UnsignedDifferenceType    size () const;

            public:
                /**
                 *  UNCLEAR - THOUGH OUT DEF _ IF WE WANT TO CONTIN EDGES. ANSWER COULD BE DIFFERNT BETWEEN
                 *  DISCRETE versus NON-DISCRETE cases?
                 *
                 *  Numerically - what makes the most sense is to contain the edges - so assume yes.
                 */
                nonvirtual  bool    Contains (const T& r) const;

#if 0
            public:
                /**
                 */
                nonvirtual  bool    Overlaps (const Range<T, TRAITS>& rhs) const;
#endif

            public:
                /**
                 *  If two ranges are both empty, they will both be equal.
                 */
                nonvirtual  bool    Equals (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 */
                nonvirtual  Range<T, TRAITS> Intersection (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 * if two regions are disjoint, this can encompass a larger region than the actual union would
                 */
                nonvirtual  Range<T, TRAITS>    UnionBounds (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual  T    begin () const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual  T    end () const;

            public:
                /**
                 */
                static  Openness    GetBeginOpenness ();

            public:
                /**
                 */
                static  Openness    GetEndOpenness ();

            public:
                /**
                 */
                static  T    GetBeginMin ();

            public:
                /**
                 */
                static  T    GetEndMax ();

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Range<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator!= (const Range<T, TRAITS>& rhs) const;

            private:
                T           fBegin_;
                T           fEnd_;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Range.inl"

#endif  /*_Stroika_Foundation_Traversal_Range_h_ */
