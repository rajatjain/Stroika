/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Tally_inl_
#define _Stroika_Foundation_Containers_Tally_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"
#include    "Tally.h"

#include    "Concrete/Tally_Array.h"  // needed for default constructor


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             */
            template    <typename T>
            class  Tally<T>::TallyMutator::IRep : public Iterator<TallyEntry<T> >::IRep {
            protected:
                IRep ();

            public:
                virtual void    RemoveCurrent ()                =   0;
                virtual void    UpdateCount (size_t newCount)   =   0;
            };


            /**
             * Protected helper class to convert from an iterator of TallyEntries
             * to an iterator over individual items.
             *
             *  @todo   BUG - _TallyEntryToItemIterator should stop N times where N is the value of count
             *          in the source iterator!
             *
             *          But document and verify the calling code is expecting this.
             */
            template    <typename T>
            class  Tally<T>::_TallyEntryToItemIterator : public Iterator<T>::IRep {
            public:
                _TallyEntryToItemIterator (const Iterator<TallyEntry<T>>& fDelegateTo);

                virtual bool                                    More (T* current, bool advance) override;
                virtual shared_ptr<typename Iterator<T>::IRep>  Clone () const override;
                virtual bool                                    StrongEquals (typename const Iterator<T>::IRep* rhs) const override;

            private:
                Iterator<TallyEntry<T>> fDelegateTo_;
            };


            /*
             ********************************************************************************
             ****************************** TallyEntry<T> ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  TallyEntry<T>::TallyEntry (T item)
                : fItem (item)
                , fCount (1)
            {
            }
            template    <typename T>
            inline  TallyEntry<T>::TallyEntry (T item, size_t count)
                : fItem (item)
                , fCount (count)
            {
            }
            template    <typename T>
            inline bool   TallyEntry<T>::operator== (const TallyEntry<T>& rhs)  const
            {
                return (fCount == rhs.fCount and fItem == rhs.fItem);
            }
            template    <typename T>
            bool   operator== (const TallyEntry<T>& lhs, const TallyEntry<T>& rhs)
            {
                if (not (lhs.fItem == rhs.fItem))  {
                    return false;
                }
                return (bool (lhs.fCount == rhs.fCount));
            }
            template    <typename T>
            inline bool   TallyEntry<T>::operator!= (const TallyEntry<T>& rhs)  const
            {
                return not (operator== (rhs));
            }


            /*
             ********************************************************************************
             ********************** Tally<T>::TallyMutator::IRep ****************************
             ********************************************************************************
             */
            template    <typename T>
            inline Tally<T>::TallyMutator::IRep::IRep ()
                : Iterator<TallyEntry<T>>::IRep ()
            {
            }


            /*
            ********************************************************************************
            ************************* Tally<T>::TallyMutator *******************************
            ********************************************************************************
            */
            template    <typename T>
            inline  Tally<T>::TallyMutator::TallyMutator (IRep* it)
                : Iterator<TallyEntry<T>> (SharedByValueRepType (it))
            {
            }
            template    <typename T>
            inline  void    Tally<T>::TallyMutator::RemoveCurrent ()
            {
                dynamic_cast<IRep&> (GetRep ()).RemoveCurrent ();
            }
            template    <typename T>
            inline  void    Tally<T>::TallyMutator::UpdateCount (size_t newCount)
            {
                dynamic_cast<IRep&> (GetRep ()).UpdateCount (newCount);
            }


            /*
             ********************************************************************************
             **************************** Tally<T>::_IRep ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Tally<T>::_IRep::_IRep ()
            {
            }


            /*
             ********************************************************************************
             *********************************** Tally<T> ***********************************
             ********************************************************************************
             */
            template    <typename T>
            Tally<T>::Tally ()
                : inherited (Concrete::Tally_Array<T> ())
            {
            }
            template    <typename T>
            inline  Tally<T>::Tally (const Tally<T>& src)
                : inherited (src)
            {
            }
            template    <typename T>
            inline  Tally<T>::Tally (_IRep* rep)
                : inherited (typename Iterable<TallyEntry<T>>::_SharedByValueRepType (rep))
            {
            }
            template    <typename T>
            Tally<T>::Tally (const T* start, const T* end)
                : inherited (Concrete::Tally_Array<T> (start, end))
            {
            }
            template    <typename T>
            Tally<T>::Tally (const TallyEntry<T>* start, const TallyEntry<T>* end)
                : inherited (Concrete::Tally_Array<T> (start, end))
            {
            }
            template    <typename T>
            void   Tally<T>::RemoveAll (T item)
            {
                Remove (item, TallyOf (item));
            }
            template    <typename T>
            size_t Tally<T>::TotalTally () const
            {
                size_t sum = 0;
                for (auto i = begin (); i != end (); ++i) {
                    sum += (*i).fCount;
                }
                return sum;
            }
            template    <typename T>
            Tally<T>&  Tally<T>::operator+= (const Tally<T>& t)
            {
                for (auto i = t.begin (); i != t.end (); ++i) {
                    Add (i->fItem, i->fCount);
                }
                return (*this);
            }
            template    <typename T>
            void   Tally<T>::Add (const T* begin, const T* end)
            {
                for (const T* i = begin; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T>
            inline  bool    Tally<T>::Contains (T item) const
            {
                return (_GetRep ().Contains (item));
            }
            template    <typename T>
            inline  void    Tally<T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T>
            inline  void    Tally<T>::Compact ()
            {
                _GetRep ().Compact ();
            }
            template    <typename T>
            inline  Iterator<T>    Tally<T>::MakeBagIterator () const
            {
                Iterator<T> tmp =   Iterator<T> (typename Iterator<T>::SharedByValueRepType (const_cast<Tally<T>*> (this)->_GetRep ().MakeBagIterator ()));
                //tmphack - must fix to have iteratorrep dont proerply and not need to init owning itgerator object
                tmp++;
                return tmp;
            }
            template    <typename T>
            inline  Iterator<T>    Tally<T>::bagbegin () const
            {
                return MakeBagIterator ();
            }
            template    <typename T>
            inline  Iterator<T>       Tally<T>::bagend () const
            {
                return (Iterator<T>::GetEmptyIterator ());
            }
            template    <typename T>
            inline  Iterator<TallyEntry<T>>    Tally<T>::begin () const
            {
                return inherited::begin ();
            }
            template    <typename T>
            inline  Iterator<TallyEntry<T>>    Tally<T>::end () const
            {
                return inherited::end ();
            }
            template    <typename T>
            inline  typename Tally<T>::TallyMutator    Tally<T>::begin ()
            {
                TallyMutator    tmp = _GetRep ().MakeTallyMutator ();
                //tmphack - must fix to have iteratorrep dont proerply and not need to init owning itgerator object
                tmp++;
                return tmp;
            }
            template    <typename T>
            inline  typename Tally<T>::TallyMutator    Tally<T>::end ()
            {
                class   RepSentinal_ : public Tally<T>::TallyMutator::IRep  {
                public:
                    virtual bool    More (TallyEntry<T>* current, bool advance) override {
                        return false;
                    }
                    virtual shared_ptr<typename Iterator<TallyEntry<T>>::IRep>    Clone () const override {
                        RequireNotReached ();
                        return nullptr;
                    }
                    virtual bool    StrongEquals (typename const Iterator<TallyEntry<T>>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }
                    virtual void    RemoveCurrent () override {
                        RequireNotReached ();
                    }
                    virtual void    UpdateCount (size_t newCount) override {
                        RequireNotReached ();
                    }
                };
                static  TallyMutator kSentinal = TallyMutator (new RepSentinal_ ());
                return kSentinal;
            }
            template    <typename T>
            inline  void    Tally<T>::Add (T item)
            {
                _GetRep ().Add (item, 1);
            }
            template    <typename T>
            inline  void    Tally<T>::Add (T item, size_t count)
            {
                _GetRep ().Add (item, count);
            }
            template    <typename T>
            inline  void    Tally<T>::Remove (T item)
            {
                _GetRep ().Remove (item, 1);
            }
            template    <typename T>
            inline  void    Tally<T>::Remove (T item, size_t count)
            {
                _GetRep ().Remove (item, count);
            }
            template    <typename T>
            inline  size_t  Tally<T>::TallyOf (T item) const
            {
                return (_GetRep ().TallyOf (item));
            }
            template    <typename T>
            inline  Tally<T>&   Tally<T>::operator+= (T item)
            {
                _GetRep ().Add (item, 1);
                return (*this);
            }
            template    <typename T>
            inline  typename const Tally<T>::_IRep&  Tally<T>::_GetRep () const
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&inherited::_GetRep (), Tally<T>::_IRep);
                return *static_cast<const Tally<T>::_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  typename Tally<T>::_IRep&        Tally<T>::_GetRep ()
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&inherited::_GetRep (), Tally<T>::_IRep);
                return *static_cast<Tally<T>::_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline bool    operator!= (const Tally<T>& lhs, const Tally<T>& rhs)
            {
                return (not operator== (lhs, rhs));
            }
            template    <typename T>
            bool   operator== (const Tally<T>& lhs, const Tally<T>& rhs)
            {
                if (&lhs._GetRep () == &rhs._GetRep ()) {
                    return (true);
                }
                if (lhs.GetLength () != rhs.GetLength ()) {
                    return (false);
                }
                for (auto i = rhs.begin (); i != rhs.end (); ++i) {
                    if (i->fCount != rhs.TallyOf (i->fItem)) {
                        return (false);
                    }
                }
                return (true);
            }


            /*
             ********************************************************************************
             ********************** Tally<T>::_TallyEntryToItemIterator *********************
             ********************************************************************************
             */
            template    <typename T>
            Tally<T>::_TallyEntryToItemIterator::_TallyEntryToItemIterator (const Iterator<TallyEntry<T>>& delegateTo)
                : fDelegateTo_ (delegateTo)
            {
            }
            template    <typename T>
            inline bool    Tally<T>::_TallyEntryToItemIterator::More (T* current, bool advance)
            {
                if (current == nullptr) {
                    return not fDelegateTo_.Done ();
                }
                else {
                    fDelegateTo_++;
                    bool done = fDelegateTo_.Done ();
                    if (current != nullptr and not done) {
                        *current = (*fDelegateTo_).fItem;
                    }
                    return (not done);
                }
            }
            template    <typename T>
            inline shared_ptr<typename Iterator<T>::IRep>  Tally<T>::_TallyEntryToItemIterator::Clone () const
            {
                return shared_ptr<typename Iterator<T>::IRep> (new _TallyEntryToItemIterator (Iterator<TallyEntry<T>> (fDelegateTo_)));
            }
            template    <typename T>
            inline  bool    Tally<T>::_TallyEntryToItemIterator::StrongEquals (typename const Iterator<T>::IRep* rhs) const
            {
                AssertNotImplemented ();
                return false;
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Tally_inl_ */


