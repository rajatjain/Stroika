/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_inl_
#define _Stroika_Foundation_Containers_SortedSet_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedSet_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ********************************** SortedSet<T> ********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  SortedSet<T, TRAITS>::SortedSet ()
                : inherited (move<inherited> (Concrete::SortedSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedSet<T, TRAITS>::SortedSet (const SortedSet<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedSet<T, TRAITS>::SortedSet (const _SharedPtrIRep& src)
                : inherited (static_cast<const typename inherited::_SharedPtrIRep&> (src))
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedSet<T, TRAITS>::SortedSet (_SharedPtrIRep&& src)
                : inherited (move<typename inherited::_SharedPtrIRep> (src))
            {
                //RequireNotNull (src); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedSet<T, TRAITS>::SortedSet (const initializer_list<T>& src)
                : inherited (move<inherited> (Concrete::SortedSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  SortedSet<T, TRAITS>::SortedSet (const CONTAINER_OF_T& src)
                : inherited (move<inherited> (Concrete::SortedSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  void    SortedSet<T, TRAITS>::_AssertRepValidType () const
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedSet_inl_ */
