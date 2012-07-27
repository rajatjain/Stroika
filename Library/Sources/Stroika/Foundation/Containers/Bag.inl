/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_inl_
#define _Stroika_Foundation_Containers_Bag_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/Bag_Array.h"  // needed for default constructor



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

            // class Bag<T>
            template    <typename T>
            inline  Bag<T>::Bag (const Bag<T>& bag)
                : fRep_ (bag.fRep_)
            {
                RequireNotNull (bag.fRep_.get ());
            }
            template    <typename T>
            inline  Bag<T>::Bag (typename Bag<T>::_IRep* rep)
                : fRep_ (rep)
            {
                RequireNotNull (rep);
            }
            template    <typename T>
            Bag<T>::Bag ()
                : fRep_ ()
            {
                *this = Concrete::Bag_Array<T> ();
                AssertNotNull (fRep_.get ());
            }
            template    <typename T>
            Bag<T>::Bag (const T* start, const T* end)
                : fRep_ ()
            {
                *this = Concrete::Bag_Array<T> (start, end);
                AssertNotNull (fRep_.get ());
            }
            template    <typename T>
            inline  Bag<T>& Bag<T>::operator= (const Bag<T>& bag)
            {
				RequireNotNull (bag.fRep_.GetPointer ());
				if (fRep_.GetPointer () != bag.fRep_.GetPointer ()) {
					fRep_ = bag.fRep_;
				}
                return (*this);
            }
            template    <typename T>
            inline  size_t  Bag<T>::GetLength () const
            {
				EnsureNotNull (fRep_.GetPointer ());
                return (fRep_->GetLength ());
            }
            template    <typename T>
            inline  bool    Bag<T>::IsEmpty () const
            {
                return (bool (GetLength () == 0));
            }
            template    <typename T>
            inline  bool    Bag<T>::Contains (T item) const
            {
				EnsureNotNull (fRep_.GetPointer ());
                return (fRep_->Contains (item));
            }
            template    <typename T>
            inline  void    Bag<T>::RemoveAll ()
            {
				EnsureNotNull (fRep_.GetPointer ());
                fRep_->RemoveAll ();
            }
            template    <typename T>
            inline  void    Bag<T>::Compact ()
            {
				EnsureNotNull (fRep_.GetPointer ());
                fRep_->Compact ();
            }
            template    <typename T>
            inline  Iterator<T> Bag<T>::MakeIterator () const
            {
				EnsureNotNull (fRep_.GetPointer ());
                Iterator<T> it (const_cast<Bag<T> *> (this)->fRep_->MakeIterator ());
                ++it;
                return it;
            }
            template    <typename T>
            inline  Iterator<T>    Bag<T>::begin () const
            {
                return MakeIterator ();
            }
            template    <typename T>
            inline  Iterator<T>    Bag<T>::end () const
            {
                return (Iterator<T>::GetSentinal ());
            }
            template    <typename T>
            inline  typename Bag<T>::Mutator    Bag<T>::begin ()
            {
                return MakeMutator ();
            }
            template    <typename T>
            inline  typename Bag<T>::Mutator    Bag<T>::end ()
            {
                return (Bag<T>::Mutator (nullptr));
            }
            template    <typename T>
            inline  Bag<T>& Bag<T>::operator+= (T item)
            {
                Add (item);
                return (*this);
            }
            template    <typename T>
            inline  Bag<T>& Bag<T>::operator+= (const Bag<T>& items)
            {
                Add (items);
                return (*this);
            }
            template    <typename T>
            inline  Bag<T>& Bag<T>::operator-= (T item)
            {
                Remove (item);
                return (*this);
            }
            template    <typename T>
            inline  Bag<T>& Bag<T>::operator-= (const Bag<T>& items)
            {
                Remove (items);
                return (*this);
            }
            template    <typename T>
            inline   typename   Bag<T>::Mutator  Bag<T>::MakeMutator ()
            {
				EnsureNotNull (fRep_.GetPointer ());
                Bag<T>::Mutator it = Bag<T>::Mutator (fRep_->MakeBagMutator ());
                ++it;
                return it;
            }
            template    <typename T>
            inline  void    Bag<T>::Add (T item)
            {
                _GetRep ().Add (item);
                Ensure (not IsEmpty ());
            }
            template    <typename T>
            void  Bag<T>::Add (const Bag<T>& items)
            {
                if (this == &items) {
                    // Copy - so we don't update this while we are copying from it...
                    Bag<T>  copiedItems =   items;
                    For (it, copiedItems) {
                        _GetRep ().Add (it.Current ());
                    }
                }
                else {
                    For (it, items) {
                        _GetRep ().Add (it.Current ());
                    }
                }
            }
            template    <typename T>
            void  Bag<T>::Add (const T* begin, const T* end)
            {
                for (const T* i = begin; i != end; i++) {
                    Add (*i);
                }
            }
            template    <typename T>
            inline  void  Bag<T>::Remove (T item)
            {
                _GetRep ().Remove (item);
            }
            template    <typename T>
            void  Bag<T>::Remove (const Bag<T>& items)
            {
                if (&_GetRep () == &items._GetRep ()) {
                    RemoveAll ();
                }
                else {
                    For (it, items) {
                        _GetRep ().Remove (it.Current ());
                    }
                }
            }
            template    <typename T>
            size_t    Bag<T>::TallyOf (T item) const
            {
                size_t count = 0;
                For (it, (*this)) {
                    if (it.Current () == item) {
                        count++;
                    }
                }
                return (count);
            }
            template    <typename T>
            typename Bag<T>::_IRep*  Bag<T>::Clone_ (const typename Bag<T>::_IRep& rep)
            {
                return (rep.Clone ());
            }





            /*
             ********************************************************************************
             ************************************ operators *********************************
             ********************************************************************************
             */
            template    <typename T>
            bool  operator== (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                if (&lhs._GetRep () == &rhs._GetRep ()) {
                    return (true);
                }
                if (lhs.GetLength () != rhs.GetLength ()) {
                    return (false);
                }

                // n^2 algorithm!!!
                For (it, lhs) {
                    if (lhs.TallyOf (it.Current ()) != rhs.TallyOf (it.Current ())) {
                        return (false);
                    }
                }
                return (true);
            }
            template    <typename T>
            Bag<T>    operator+ (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                Bag<T>  temp    = lhs;
                temp += rhs;
                return (temp);
            }
            template    <typename T>
            Bag<T>    operator- (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                Bag<T>  temp    = lhs;
                temp -= rhs;
                return (temp);
            }
            template    <typename T>
            inline  bool    operator!= (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                return (not operator== (lhs, rhs));
            }



            // class Bag<T>::_IMutatorRep<T>
            template    <typename T>
            inline  Bag<T>::_IMutatorRep::_IMutatorRep () :
                Iterator<T>::Rep ()
            {
            }


            // class Bag<T>::Mutator
            template    <typename T>
            inline  Bag<T>::Mutator::Mutator (typename Bag<T>::_IMutatorRep* it) :
                Iterator<T> (it)
            {
            }
            template    <typename T>
            inline  typename    Bag<T>::_IMutatorRep* Bag<T>::Mutator::GetMutatorRep_ ()
            {
                /*
                 * Because of the way we construct Mutators, it is guaranteed that
                 * this cast is safe. We could have kept an extra var of the right
                 * static type, but this would have been a waste of time and memory.
                 */
                return &(dynamic_cast<_IMutatorRep&> (*this->fIterator));
            }
            template    <typename T>
            inline  void    Bag<T>::Mutator::RemoveCurrent ()
            {
                GetMutatorRep_ ()->RemoveCurrent ();
            }
            template    <typename T>
            inline  void    Bag<T>::Mutator::UpdateCurrent (T newValue)
            {
                GetMutatorRep_ ()->UpdateCurrent (newValue);
            }





            // class _IRep<T>
            template    <typename T>
            inline  Bag<T>::_IRep::_IRep ()
            {
            }
            template    <typename T>
            inline  Bag<T>::_IRep::~_IRep ()
            {
            }
            template    <typename T>
            inline  const typename  Bag<T>::_IRep&    Bag<T>::_GetRep () const
            {
				EnsureNotNull (fRep_.GetPointer ());
				return *fRep_;
            }
            template    <typename T>
            inline  typename    Bag<T>::_IRep&  Bag<T>::_GetRep ()
            {
				EnsureNotNull (fRep_.GetPointer ());
				return *fRep_;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_Bag_inl_ */
