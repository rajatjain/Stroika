/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Collection_inl_
#define _Stroika_Foundation_Containers_Collection_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"

#include    "Concrete/Collection_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ******************************** Collection<T> *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Collection<T>::Collection ()
                : inherited (move (Concrete::Collection_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Collection<T>::Collection (const Collection<T>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Collection<T>::Collection (const _SharedPtrIRep& rep)
                : inherited (static_cast<const typename inherited::_SharedPtrIRep&> (rep))
            {
                RequireNotNull (rep);
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Collection<T>::Collection (_SharedPtrIRep&& rep)
                : inherited (move (rep))
            {
                RequireNotNull (rep);
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Collection<T>::Collection (const initializer_list<T>& src)
                : inherited (move (Concrete::Collection_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Collection<T>::Collection (const CONTAINER_OF_T& src)
                : inherited (move (Concrete::Collection_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
#if     qDebug
            template    <typename T>
            Collection<T>::~Collection ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename T>
            inline  const typename  Collection<T>::_IRep&    Collection<T>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            bool    Collection<T>::Contains (T item) const
            {
                for (auto i : *this) {
                    if (EQUALS_COMPARER::Equals (i, item)) {
                        return true;
                    }
                }
                return false;
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Collection<T>::AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Collection<T>::AddAll (const CONTAINER_OF_T& c)
            {
                /*
                 * Because adding items to a Collection COULD result in those items appearing in a running iterator,
                 * for the corner case of s.AddAll(s) - we want to assure we don't infinite loop.
                 */
                if (static_cast<const void*> (this) == static_cast<const void*> (std::addressof (c))) {
                    CONTAINER_OF_T  tmp =   c;
                    AddAll (std::begin (tmp), std::end (tmp));
                }
                else {
                    AddAll (std::begin (c), std::end (c));
                }
            }
            template    <typename T>
            inline  void    Collection<T>::Add (T item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (item);
                Ensure (not this->IsEmpty ());
            }
            template    <typename T>
            inline  void    Collection<T>::Update (const Iterator<T>& i, T newValue)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Update (i, newValue);
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            inline  void  Collection<T>::Remove (T item)
            {
                for (Iterator<T> i = this->begin (); i != this->end (); ++i) {
                    if (EQUALS_COMPARER::Equals (*i, item)) {
                        _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (i);
                        return;
                    }
                }
            }
            template    <typename T>
            inline  void    Collection<T>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp { this };
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T, typename EQUALS_COMPARER>
            void    Collection<T>::RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove<EQUALS_COMPARER> (*i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T, typename EQUALS_COMPARER>
            inline  void    Collection<T>::RemoveAll (const CONTAINER_OF_T& c)
            {
                if (static_cast<const void*> (this) == static_cast<const void*> (std::addressof (c))) {
                    RemoveAll ();
                }
                else {
                    RemoveAll (std::begin (c), std::end (c));
                }
            }
            template    <typename T>
            inline  void    Collection<T>::Remove (const Iterator<T>& i)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (i);
            }
            template    <typename T>
            inline  void    Collection<T>::clear ()
            {
                RemoveAll ();
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            inline  void    Collection<T>::erase (T item)
            {
                Remove<EQUALS_COMPARER> (item);
            }
            template    <typename T>
            inline  void    Collection<T>::erase (const Iterator<T>& i)
            {
                Remove (i);
            }
            template    <typename T>
            Collection<T>    Collection<T>::EachWith (const function<bool(const T& item)>& doToElement) const
            {
                Collection<T>   result;
                for (T i : *this) {
                    if (doToElement (i)) {
                        result.Add (i);
                    }
                }
                return result;
            }
            template    <typename T>
            inline  Collection<T>& Collection<T>::operator+= (T item)
            {
                Add (item);
                return *this;
            }
            template    <typename T>
            inline  Collection<T>& Collection<T>::operator+= (const Iterable<T>& items)
            {
                AddAll (items);
                return *this;
            }
            template    <typename T>
            inline  void    Collection<T>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_Collection_inl_ */
