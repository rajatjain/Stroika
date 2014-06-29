/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Range_inl_
#define _Stroika_Foundation_Traversal_Range_inl_

#include    "../Debug/Assertions.h"
#include    "../Math/Overlap.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
             ********************************************************************************
             RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
             ********************************************************************************
             */
#if     qCompilerAndStdLib_constexpr_StaticDataMember_Buggy
            template    <typename T, T MIN, T MAX , Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kLowerBound   =   MIN;
            template    <typename T, T MIN, T MAX , Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kUpperBound   =   MAX;
#endif


            /*
             ********************************************************************************
             ****************** RangeTraits::DefaultRangeTraits<T> **************************
             ********************************************************************************
             */
#if     qCompilerAndStdLib_constexpr_StaticDataMember_Buggy
            template    <typename T>
            const T RangeTraits::DefaultRangeTraits<T>::kLowerBound   =   numeric_limits<T>::lowest ();
            template    <typename T>
            const T RangeTraits::DefaultRangeTraits<T>::kUpperBound   =   numeric_limits<T>::max ();
#endif


            /*
             ********************************************************************************
             ***************************** Range<T, TRAITS> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range ()
                : Range (TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness)
            {
                Ensure (empty ());
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const T& begin, const T& end)
                : Range (begin, end, TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness)
            {
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                : Range (begin.IsPresent () ? *begin : TRAITS::kLowerBound, end.IsPresent () ? *end : TRAITS::kUpperBound, TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness)
            {
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (Openness lhsOpen, Openness rhsOpen)
                : fBegin_ (TRAITS::kUpperBound)
                , fEnd_ (TRAITS::kLowerBound)
                , fBeginOpenness_ (lhsOpen)
                , fEndOpenness_ (rhsOpen)
            {
                Ensure (empty ());
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const T& begin, const T& end, Openness lhsOpen, Openness rhsOpen)
                : fBegin_ (begin)
                , fEnd_ (end)
                , fBeginOpenness_ (lhsOpen)
                , fEndOpenness_ (rhsOpen)
            {
                Require  (TRAITS::kLowerBound <= TRAITS::kUpperBound);    // always required for class
                Require (TRAITS::kLowerBound <= begin);
                Require (begin <= end);
                Require (end <= TRAITS::kUpperBound);
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end, Openness lhsOpen, Openness rhsOpen)
                : Range (begin.IsPresent () ? *begin : TRAITS::kLowerBound, end.IsPresent () ? *end : TRAITS::kUpperBound, lhsOpen, rhsOpen)
            {
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>    Range<T, TRAITS>::FullRange ()
            {
                return Range<T, TRAITS> (
                           typename TraitsType::kLowerBound, typename TraitsType::kUpperBound,
                           typename TraitsType::kLowerBoundOpenness, typename TraitsType::kUpperBoundOpenness
                       );
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::empty () const
            {
                if (fBegin_ > fEnd_) {
                    // internal hack done in Range<T, TRAITS>::Range() - empty range - otherwise not possible to create this situation
                    return true;
                }
                else if (fBegin_ == fEnd_) {
                    return fBeginOpenness_ == Openness::eOpen and fEndOpenness_ == Openness::eOpen;
                }
                return false;
            }
            template    <typename T, typename TRAITS>
            inline  typename TRAITS::UnsignedDifferenceType    Range<T, TRAITS>::GetDistanceSpanned () const
            {
                if (empty ()) {
                    return static_cast<typename TRAITS::UnsignedDifferenceType> (0);
                }
                return fEnd_ - fBegin_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::GetMidpoint () const
            {
                Require (not empty ());
                return GetLowerBound () + GetDistanceSpanned () / 2;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Contains (const T& r) const
            {
                if (empty ()) {
                    return false;
                }
                if (fBegin_ < r and r < fEnd_) {
                    return true;
                }
                if (fBeginOpenness_ == Openness::eClosed and r == fBegin_) {
                    return true;
                }
                if (fEndOpenness_ == Openness::eClosed and r == fEnd_) {
                    return true;
                }
                return false;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Equals (const Range<T, TRAITS>& rhs) const
            {
                if (empty ()) {
                    return rhs.empty ();
                }
                return fBegin_ == rhs.fBegin_ and fEnd_ == rhs.fEnd_ and fBeginOpenness_ == rhs.fBeginOpenness_ and fBeginOpenness_ == rhs.fBeginOpenness_;
            }
#if 0
            template    <typename T, typename TRAITS>
            bool    Range<T, TRAITS>::Overlaps (const Range<T, TRAITS>& rhs) const
            {
                /*
                 *  @todo   RETHINK - because Range has semantics of exclude end - make sure overlap usuage
                 *          here is correct??? Unsure -- LGP 2013-07-05
                 */
                return Math::Overlaps (
                           pair<T, T> (fBegin_, fEnd_),
                           pair<T, T> (rhs.fBegin_, rhs.fEnd_)
                       );
            }
#endif
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::Intersection (const Range<T, TRAITS>& rhs) const
            {
                if (empty () or rhs.empty ()) {
                    return Range ();
                }
                T   l   =   max (fBegin_, rhs.fBegin_);
                T   r   =   min (fEnd_, rhs.fEnd_);
                if (l <= r) {
                    // lhs/rhs ends are closed iff BOTH lhs/rhs contains that point
                    Openness lhsO = Contains (l) and rhs.Contains (l) ? Openness::eClosed : Openness::eOpen;
                    Openness rhsO = Contains (r) and rhs.Contains (r) ? Openness::eClosed : Openness::eOpen;
                    return Range<T, TRAITS> (l, r, lhsO, rhsO);
                }
                else {
                    return Range ();
                }
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::UnionBounds (const Range<T, TRAITS>& rhs) const
            {
                if (empty ()) {
                    return rhs;
                }
                if (rhs.empty ()) {
                    return *this;
                }
                T   l   =   min (GetLowerBound (), rhs.GetLowerBound ());
                T   r   =   max (GetUpperBound (), rhs.GetUpperBound ());
                Range<T, TRAITS>   result;
                if (l <= r) {
                    // lhs/rhs ends are closed iff BOTH lhs/rhs contains that point
                    Openness lhsO = Contains (l) and rhs.Contains (l) ? Openness::eClosed : Openness::eOpen;
                    Openness rhsO = Contains (r) and rhs.Contains (r) ? Openness::eClosed : Openness::eOpen;
                    result = Range<T, TRAITS> (l, r, lhsO, rhsO);
                }
                Ensure (result.GetLowerBound () <= GetLowerBound ());
                Ensure (result.GetLowerBound () <= GetUpperBound ());
                Ensure (result.GetLowerBound () <= rhs.GetLowerBound ());
                Ensure (result.GetLowerBound () <= rhs.GetUpperBound ());
                Ensure (result.GetUpperBound () >= GetLowerBound ());
                Ensure (result.GetUpperBound () >= GetUpperBound ());
                Ensure (result.GetUpperBound () >= rhs.GetLowerBound ());
                Ensure (result.GetUpperBound () >= rhs.GetUpperBound ());
                return result;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::GetLowerBound () const
            {
                Require (not empty ());
                return fBegin_;
            }
            template    <typename T, typename TRAITS>
            inline  Openness    Range<T, TRAITS>::GetLowerBoundOpenness () const
            {
                return fBeginOpenness_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::GetUpperBound () const
            {
                Require (not empty ());
                return fEnd_;
            }
            template    <typename T, typename TRAITS>
            inline  Openness    Range<T, TRAITS>::GetUpperBoundOpenness () const
            {
                return fEndOpenness_;
            }
            template    <typename T, typename TRAITS>
            template    <typename... ARGS>
            inline  Characters::String  Range<T, TRAITS>::Format (ARGS&& ... args) const
            {
                if (GetLowerBound () == GetUpperBound ()) {
                    return GetLowerBound ().Format (forward<ARGS> (args)...);
                }
                else {
                    return GetLowerBound ().Format (forward<ARGS> (args)...) + L" - " + GetUpperBound  ().Format (forward<ARGS> (args)...);
                }
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::operator== (const Range<T, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::operator!= (const Range<T, TRAITS>& rhs) const
            {
                return not Equals (rhs);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Range_inl_ */
