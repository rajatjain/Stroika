/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <vector>

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/DataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Sequence_stdvector<T>::Rep_ : public Sequence<T>::_IRep {
                private:
                    typedef typename    Sequence<T>::_IRep  inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);
                    NO_ASSIGNMENT_OPERATOR(Rep_);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override;
                    virtual Iterator<T>                             MakeIterator () const override;
                    virtual size_t                                  GetLength () const override;
                    virtual bool                                    IsEmpty () const override;
                    virtual void                                    Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                             ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Sequence<T>::_IRep overrides
                public:
                    virtual T       GetAt (size_t i) const override;
                    virtual void    SetAt (size_t i, const T& item) override;
                    virtual size_t  IndexOf (const Iterator<T>& i) const override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Insert (size_t at, const T* from, const T* to) override;
                    virtual void    Remove (size_t from, size_t to) override;

                private:
                    Private::ContainerRepLockDataSupport_                               fLockSupport_;
                    Private::DataStructures::Patching::STLContainerWrapper<vector<T>>   fData_;

                private:
                    friend  class Sequence_stdvector<T>::IteratorRep_;
                };


                template    <typename T>
                class  Sequence_stdvector<T>::IteratorRep_ : public Iterator<T>::IRep {
                private:
                    typedef typename    Iterator<T>::IRep   inherited;

                public:
                    explicit IteratorRep_ (typename Sequence_stdvector<T>::Rep_& owner)
                        : inherited ()
                        , fIterator_ (&owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override {
                        return typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*this));
                    }
                    virtual bool                                More (T* current, bool advance) override {
                        return (fIterator_.More (current, advance));
                    }
                    virtual bool                                StrongEquals (const typename Iterator<T>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }

                private:
                    mutable typename Private::DataStructures::Patching::STLContainerWrapper<vector<T>>::BasicForwardIterator   fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                *********************** Sequence_stdvector<T>::Rep_ ****************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Sequence_stdvector<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Sequence_stdvector<T>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                typename Iterable<T>::_SharedPtrIRep  Sequence_stdvector<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Sequence_stdvector<T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    Iterator<T> tmp = Iterator<T> (typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS)));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                size_t  Sequence_stdvector<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.size ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  Sequence_stdvector<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.empty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void      Sequence_stdvector<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Sequence_stdvector<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                T    Sequence_stdvector<T>::Rep_::GetAt (size_t i) const
                {
                    Require (not IsEmpty ());
                    Require (i == kBadSequenceIndex or i < GetLength ());
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        if (i == kBadSequenceIndex) {
                            i = fData_.size () - 1;
                        }
                        return fData_[i];
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::SetAt (size_t i, const T& item)
                {
                    Require (i < GetLength ());
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_[i] = item;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                size_t    Sequence_stdvector<T>::Rep_::IndexOf (const Iterator<T>& i) const
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_stdvector<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_stdvector<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return mir.fIterator_.CurrentIndex ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_stdvector<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_stdvector<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator_.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_stdvector<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_stdvector<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        *mir.fIterator_.fStdIterator = newValue;
                        fData_.Invariant ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::Insert (size_t at, const T* from, const T* to)
                {
                    Require (at == kBadSequenceIndex or at <= GetLength ());
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        if (at == kBadSequenceIndex) {
                            at = fData_.size ();
                        }
                        // quickie poor impl
                        for (auto i = from; i != to; ++i) {
                            fData_.insert_toVector_WithPatching (fData_.begin () + at, *i);
                            at++;
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::Remove (size_t from, size_t to)
                {
                    // quickie poor impl
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (size_t i = from; i < to; ++i) {
                            fData_.PatchBefore_erase (fData_.begin () + from);
                            fData_.erase (fData_.begin () + from);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                **************************** Sequence_stdvector<T> *****************************
                ********************************************************************************
                */
                template    <typename T>
                Sequence_stdvector<T>::Sequence_stdvector ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                inline  Sequence_stdvector<T>::Sequence_stdvector (const Sequence_stdvector<T>& s)
                    : inherited (s)
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T>
                inline  Sequence_stdvector<T>::Sequence_stdvector (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    InsertAll (0, s);
                }
                template    <typename T>
                template    <typename COPY_FROM_ITERATOR>
                inline Sequence_stdvector<T>::Sequence_stdvector (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    Append (start, end);
                }
                template    <typename T>
                inline  Sequence_stdvector<T>&   Sequence_stdvector<T>::operator= (const Sequence_stdvector<T>& s)
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    inherited::operator= (s);
                    return *this;
                }
                template    <typename T>
                inline  const typename Sequence_stdvector<T>::Rep_&  Sequence_stdvector<T>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T>
                inline  typename Sequence_stdvector<T>::Rep_&    Sequence_stdvector<T>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T>
                inline  void    Sequence_stdvector<T>::Compact ()
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().fData_.reserve (GetRep_ ().fData_.size ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                inline  size_t  Sequence_stdvector<T>::GetCapacity () const
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        return (GetRep_ ().fData_.capacity ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                inline  void    Sequence_stdvector<T>::SetCapacity (size_t slotsAlloced)
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().fData_.reserve (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_ */
