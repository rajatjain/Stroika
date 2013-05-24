/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/DataStructures/DoublyLinkedList.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Sequence_DoublyLinkedList<T>::Rep_ : public Sequence<T>::_IRep {
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
                    Private::ContainerRepLockDataSupport_               fLockSupport_;
                    Private::DataStructures::DoublyLinkedList_Patch<T>  fData_;
                    friend  class Sequence_DoublyLinkedList<T>::IteratorRep_;
                };


                template    <typename T>
                class  Sequence_DoublyLinkedList<T>::IteratorRep_ : public Iterator<T>::IRep {
                private:
                    typedef typename    Iterator<T>::IRep   inherited;

                public:
                    explicit IteratorRep_ (typename Sequence_DoublyLinkedList<T>::Rep_& owner)
                        : inherited ()
                        , fIterator_ (owner.fData_) {
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
                    mutable Private::DataStructures::DoublyLinkedListMutator_Patch<T>    fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ******************* Sequence_DoublyLinkedList<T>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Sequence_DoublyLinkedList<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Sequence_DoublyLinkedList<T>::Rep_::Rep_ (const Rep_& from)
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
                typename Iterable<T>::_SharedPtrIRep  Sequence_DoublyLinkedList<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Sequence_DoublyLinkedList<T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    Iterator<T> tmp = Iterator<T> (typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS)));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                size_t  Sequence_DoublyLinkedList<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  Sequence_DoublyLinkedList<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void      Sequence_DoublyLinkedList<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Sequence_DoublyLinkedList<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                T    Sequence_DoublyLinkedList<T>::Rep_::GetAt (size_t i) const
                {
                    Require (not IsEmpty ());
                    Require (i == kBadSequenceIndex or i < GetLength ());
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        if (i == kBadSequenceIndex) {
                            i = GetLength () - 1;
                        }
                        return fData_.GetAt (i);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_DoublyLinkedList<T>::Rep_::SetAt (size_t i, const T& item)
                {
                    Require (i < GetLength ());
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.SetAt (item, i);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                size_t    Sequence_DoublyLinkedList<T>::Rep_::IndexOf (const Iterator<T>& i) const
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_DoublyLinkedList<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_DoublyLinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return mir.fIterator_.CurrentIndex ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_DoublyLinkedList<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_DoublyLinkedList<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_DoublyLinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator_.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_DoublyLinkedList<T>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_DoublyLinkedList<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_DoublyLinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator_.UpdateCurrent (newValue);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_DoublyLinkedList<T>::Rep_::Insert (size_t at, const T* from, const T* to)
                {
                    Require (at == kBadSequenceIndex or at <= GetLength ());
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        if (at == kBadSequenceIndex) {
                            at = fData_.GetLength ();
                        }
                        // quickie poor impl
                        // See Stroika v1 - much better - handling cases of remove near start or end of linked list
                        if (at == 0) {
                            size_t len = to - from;
                            for (size_t i = (to - from); i > 0; --i) {
                                fData_.Prepend (from[i - 1]);
                            }
                        }
                        else if (at == fData_.GetLength ()) {
                            for (const T* p = from; p != to; ++p) {
                                fData_.Append (*p);
                            }
                        }
                        else {
                            size_t index = at;
                            T tmphack;
                            for (Private::DataStructures::DoublyLinkedListMutator_Patch<T> it (fData_); it.More (&tmphack, true); ) {
                                if (--index == 0) {
                                    for (const T* p = from; p != to; ++p) {
                                        it.AddBefore (*p);
                                    }
                                    break;
                                }
                            }
                            //Assert (not it.Done ());      // cuz that would mean we never added
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_DoublyLinkedList<T>::Rep_::Remove (size_t from, size_t to)
                {
                    // quickie poor impl
                    // See Stroika v1 - much better - handling cases of remove near start or end of linked list
                    size_t index = from;
                    size_t amountToRemove = (to - from);
                    T tmphack;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (Private::DataStructures::DoublyLinkedListMutator_Patch<T> it (fData_); it.More (&tmphack, true); ) {
                            if (index-- == 0) {
                                while (amountToRemove-- != 0) {
                                    it.RemoveCurrent ();
                                }
                                break;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ********************* Sequence_DoublyLinkedList<T> *****************************
                ********************************************************************************
                */
                template    <typename T>
                Sequence_DoublyLinkedList<T>::Sequence_DoublyLinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T>
                inline  Sequence_DoublyLinkedList<T>::Sequence_DoublyLinkedList (const Sequence_DoublyLinkedList<T>& s)
                    : inherited (s)
                {
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T>
                inline  Sequence_DoublyLinkedList<T>::Sequence_DoublyLinkedList (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    InsertAll (0, s);
                }
                template    <typename T>
                template    <typename COPY_FROM_ITERATOR>
                inline Sequence_DoublyLinkedList<T>::Sequence_DoublyLinkedList (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Append (start, end);
                }
                template    <typename T>
                inline  Sequence_DoublyLinkedList<T>&   Sequence_DoublyLinkedList<T>::operator= (const Sequence_DoublyLinkedList<T>& s)
                {
                    inherited::operator= (s);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_inl_ */
