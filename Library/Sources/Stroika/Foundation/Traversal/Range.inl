/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
             ***************************** Range<T, TRAITS> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            Range<T, TRAITS>::Range ()
                : fBegin_ (TRAITS::kMin)
                , fEnd_ (TRAITS::kMin)
            {
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                : fBegin_ (begin.IsPresent () ? *begin : TRAITS::kMin)
                , fEnd_ (end.IsPresent () ? *end : TRAITS::kMax)
            {
                Require (fBegin_ <= fEnd_);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Contains (const T& v) const
            {
                return fBegin_ <= v and v < fEnd_;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Equals (const Range<T, TRAITS>& v) const
            {
                if (empty ()) {
                    return v.empty ();
                }
                return fBegin_ == v.fBegin_ and fEnd_ == v.fEnd_;
            }
            template    <typename T, typename TRAITS>
            bool    Range<T, TRAITS>::Overlaps (const Range<T, TRAITS>& v) const
            {
                /*
                 *  @todo   RETHINK - because Range has semantics of exclude end - make sure overlap usuage
                 *          here is correct??? Unsure -- LGP 2013-07-05
                 */
                return Math::Overlaps (
                           pair<T, T> (fBegin_, fEnd_),
                           pair<T, T> (v.fBegin_, v.fEnd_)
                       );
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::Intersection (const Range<T, TRAITS>& v) const
            {
                T   l   =   max (fBegin_, v.fBegin_);
                T   r   =   min (fEnd_, v.fEnd_);
                if (l < r) {
                    return Range<T, TRAITS> (l, r);
                }
                else {
                    return Range<T, TRAITS> ();
                }
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::ExpandedUnion (const Range<T, TRAITS>& v) const
            {
                Range<T, TRAITS>    result  =   Range<T, TRAITS> (min (GetEffectiveMin (), v.GetEffectiveMin ()), max (GetEffectiveMax (), v.GetEffectiveMax ()));
                Ensure (result.GetEffectiveMin () <= GetEffectiveMin ());
                Ensure (result.GetEffectiveMin () <= GetEffectiveMax ());
                Ensure (result.GetEffectiveMin () <= v.GetEffectiveMin ());
                Ensure (result.GetEffectiveMin () <= v.GetEffectiveMax ());
                Ensure (result.GetEffectiveMax () >= GetEffectiveMin ());
                Ensure (result.GetEffectiveMax () >= GetEffectiveMax ());
                Ensure (result.GetEffectiveMax () >= v.GetEffectiveMin ());
                Ensure (result.GetEffectiveMax () >= v.GetEffectiveMax ());
                return result;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::begin () const
            {
                return fBegin_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::end () const
            {
                return fEnd_;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Range_inl_ */
