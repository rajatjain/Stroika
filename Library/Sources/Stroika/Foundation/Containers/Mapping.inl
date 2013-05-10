/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"

#include    "Concrete/Mapping_Factory.h"

#ifndef _Stroika_Foundation_Containers_Mapping_inl_
#define _Stroika_Foundation_Containers_Mapping_inl_

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ******************************** Mapping<Key,T> ********************************
             ********************************************************************************
             */
            template    <typename Key, typename T>
            Mapping<Key, T>::Mapping ()
                : inherited (Concrete::mkMapping_Default<Key, T> ())
            {
            }
            template    <typename Key, typename T>
            inline  Mapping<Key, T>::Mapping (const Mapping<Key, T>& m)
                : inherited (m)
            {
            }
            template    <typename Key, typename T>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Mapping<Key, T>::Mapping (const CONTAINER_OF_PAIR_KEY_T& cp)
                : inherited (Concrete::mkMapping_Default<Key, T> ())
            {
                AddAll (cp);
            }
            template    <typename Key, typename T>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            Mapping<Key, T>::Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (Concrete::mkMapping_Default<Key, T> ())
            {
                AddAll (start, end);
            }
            template    <typename Key, typename T>
            inline  Mapping<Key, T>::Mapping (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedByValueRepType (rep))
            {
                RequireNotNull (rep);
            }
#if     !qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
            template    <typename Key, typename T>
            inline  Mapping<Key, T>& Mapping<Key, T>::operator= (const Mapping<Key, T>& src)
            {
                inherited::operator= (src);
                return *this;
            }
#endif
            template    <typename Key, typename T>
            inline  const typename  Mapping<Key, T>::_IRep&    Mapping<Key, T>::_GetRep () const
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename Key, typename T>
            inline  typename    Mapping<Key, T>::_IRep&     Mapping<Key, T>::_GetRep ()
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename Key, typename T>
            inline  Iterable<Key>    Mapping<Key, T>::Keys () const
            {
                return _GetRep ().Keys ();
            }
            template    <typename Key, typename T>
            inline  bool    Mapping<Key, T>::Lookup (Key key, T* item) const
            {
                return _GetRep ().Lookup (key, item);
            }
            template    <typename Key, typename T>
            inline  Memory::Optional<T>    Mapping<Key, T>::Lookup (Key key) const
            {
                // @todo change virutal API so this trick not needed
                T   r;  // find way so we dont require a default CTOR - probably change virtual AP
                if (_GetRep ().Lookup (key, &r)) {
                    return r;
                }
                return Memory::Optional<T> ();
            }
            template    <typename Key, typename T>
            inline  bool    Mapping<Key, T>::ContainsKey (Key key) const
            {
                return _GetRep ().Lookup (key, nullptr);
            }
            template    <typename Key, typename T>
            inline  bool    Mapping<Key, T>::ContainsValue (T v) const
            {
                for (T t : *this) {
                    if (t.second == v) {
                        return true;
                    }
                }
                return false;
            }
            template    <typename Key, typename T>
            inline  void    Mapping<Key, T>::Add (Key key, T newElt)
            {
                _GetRep ().Add (key, newElt);
            }
            template    <typename Key, typename T>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            void    Mapping<Key, T>::AddAll (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (i->first, i->second);
                }
            }
            template    <typename Key, typename T>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            void    Mapping<Key, T>::AddAll (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                for (auto i : items) {
                    Add (i.first, i.second);
                }
            }
            template    <typename Key, typename T>
            inline  void    Mapping<Key, T>::Remove (Key key)
            {
                _GetRep ().Remove (key);
            }
            template    <typename Key, typename T>
            inline  void    Mapping<Key, T>::Remove (const Iterator<pair<Key, T>>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename Key, typename T>
            inline  void    Mapping<Key, T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename Key, typename T>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            void    Mapping<Key, T>::RemoveAll (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                for (auto i : items) {
                    Remove (i.first);
                }
            }
            template    <typename Key, typename T>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            void    Mapping<Key, T>::RemoveAll (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (i->first);
                }
            }
            template    <typename Key, typename T>
            inline  void    Mapping<Key, T>::clear ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename Key, typename T>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Mapping<Key, T>&    Mapping<Key, T>::operator+= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                Add (items);
                return *this;
            }
            template    <typename Key, typename T>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Mapping<Key, T>&    Mapping<Key, T>::operator-= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                Remove (items);
                return *this;
            }


            /*
             ********************************************************************************
             ************************** Mapping<Key, T>::_IRep ******************************
             ********************************************************************************
             */
            template    <typename Key, typename T>
            inline  Mapping<Key, T>::_IRep::_IRep ()
            {
            }
            template    <typename Key, typename T>
            inline  Mapping<Key, T>::_IRep::~_IRep ()
            {
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Mapping_inl_ */

//     See note at the end of Concrete/Mapping_Factory.h about why this is here
#include    "Concrete/Mapping_Factory.inl"
