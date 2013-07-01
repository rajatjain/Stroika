/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Bag_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/PatchingDataStructures/Array.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ************************ Bag_Array<T,BAG_TRAITS>::Rep_ *************************
                 ********************************************************************************
                 */
                template    <typename T, typename BAG_TRAITS>
                class   Bag_Array<T, BAG_TRAITS>::Rep_ : public Bag<T, BAG_TRAITS>::_IRep {
                private:
                    typedef typename    Bag<T, BAG_TRAITS>::_IRep   inherited;

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

                    // Bag<T,BAG_TRAITS>::_IRep overrides
                public:
                    virtual bool    Equals (const typename Bag<T, BAG_TRAITS>::_IRep& rhs) const override;
                    virtual bool    Contains (T item) const override;
                    virtual size_t  TallyOf (T item) const override;
                    virtual void    Add (T item) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Remove (T item) override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    RemoveAll () override;

                private:
                    //typedef Private::PatchingDataStructures::Array_Patch<T> ImplArrayType_;
                    typedef Private::DataStructures::Array_TraitsWithEqualsComparer<T, typename BAG_TRAITS::EqualsCompareFunctionType>   UseArrayComparerType_;
                    typedef Private::PatchingDataStructures::Array_Patch<T, UseArrayComparerType_>                                       ImplArrayType_;
                    Private::ContainerRepLockDataSupport_                                                                               fLockSupport_;
                    ImplArrayType_     fData_;
                    friend  class Bag_Array<T, BAG_TRAITS>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 ********************* Bag_Array<T,BAG_TRAITS>::IteratorRep_ ********************
                 ********************************************************************************
                 */
                template    <typename T, typename BAG_TRAITS>
                class  Bag_Array<T, BAG_TRAITS>::IteratorRep_ : public Iterator<T>::IRep {
                private:
                    typedef typename    Iterator<T>::IRep   inherited;
                public:
                    explicit IteratorRep_ (typename Bag_Array<T, BAG_TRAITS>::Rep_& owner)
                        : inherited ()
                        , fLockSupport_ (owner.fLockSupport_)
                        , fIterator_ (owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*this));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    More (T* current, bool advance) override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return (fIterator_.More (current, advance));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    StrongEquals (const typename Iterator<T>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }

                private:
                    Private::ContainerRepLockDataSupport_&                   fLockSupport_;
                    mutable typename Rep_::ImplArrayType_::ForwardIterator   fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ******************* Bag_Array<T,BAG_TRAITS>::Rep_ ******************************
                ********************************************************************************
                */
                template    <typename T, typename BAG_TRAITS>
                inline  Bag_Array<T, BAG_TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename BAG_TRAITS>
                inline  Bag_Array<T, BAG_TRAITS>::Rep_::Rep_ (const Rep_& from)
                    : fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename BAG_TRAITS>
                typename Iterable<T>::_SharedPtrIRep  Bag_Array<T, BAG_TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T, typename BAG_TRAITS>
                Iterator<T>  Bag_Array<T, BAG_TRAITS>::Rep_::MakeIterator () const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    Iterator<T> tmp = Iterator<T> (tmpRep);
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T, typename BAG_TRAITS>
                size_t  Bag_Array<T, BAG_TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename BAG_TRAITS>
                bool  Bag_Array<T, BAG_TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename BAG_TRAITS>
                void      Bag_Array<T, BAG_TRAITS>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename BAG_TRAITS>
                Iterator<T>     Bag_Array<T, BAG_TRAITS>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename BAG_TRAITS>
                bool    Bag_Array<T, BAG_TRAITS>::Rep_::Equals (const typename Bag<T, BAG_TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename BAG_TRAITS>
                bool    Bag_Array<T, BAG_TRAITS>::Rep_::Contains (T item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.Contains (item));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename BAG_TRAITS>
                size_t    Bag_Array<T, BAG_TRAITS>::Rep_::TallyOf (T item) const
                {
                    return this->_TallyOf_Reference_Implementation (item);
                }
                template    <typename T, typename BAG_TRAITS>
                void    Bag_Array<T, BAG_TRAITS>::Rep_::Add (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        // Appending is fastest
                        fData_.InsertAt (fData_.GetLength (), item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename BAG_TRAITS>
                void    Bag_Array<T, BAG_TRAITS>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Bag_Array<T, BAG_TRAITS>::IteratorRep_&       mir =   dynamic_cast<const typename Bag_Array<T, BAG_TRAITS>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.SetAt (mir.fIterator_, newValue);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename BAG_TRAITS>
                void    Bag_Array<T, BAG_TRAITS>::Rep_::Remove (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        /*
                         *  Iterate backwards since removing from the end of an array will be faster.
                         */
                        // DEBUG WHY USING BACKWARD ITERATOR GENERATES ERROR ON MSVC - COMPILE ERROR ABOUT TYPE MISMATCH-- WHEN I SWITCHED TO USING ImplArrayType_...
                        // NOT IMPORTANT NOW _ SO DEFER
                        //for (typename ImplArrayType_::BackwardIterator it (fData_); it.More (nullptr, true);) {
                        for (typename ImplArrayType_::ForwardIterator it (fData_); it.More (nullptr, true);) {
                            if (typename BAG_TRAITS::EqualsCompareFunctionType::Equals (it.Current (), item)) {
                                fData_.RemoveAt (it.CurrentIndex ());
                                return;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename BAG_TRAITS>
                void    Bag_Array<T, BAG_TRAITS>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Bag_Array<T, BAG_TRAITS>::IteratorRep_&       mir =   dynamic_cast<const typename Bag_Array<T, BAG_TRAITS>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAt (mir.fIterator_);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename BAG_TRAITS>
                void    Bag_Array<T, BAG_TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ********************** Bag_Array<T,BAG_TRAITS> *********************************
                ********************************************************************************
                */
                template    <typename T, typename BAG_TRAITS>
                Bag_Array<T, BAG_TRAITS>::Bag_Array ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T, typename BAG_TRAITS>
                Bag_Array<T, BAG_TRAITS>::Bag_Array (const Bag<T, BAG_TRAITS>& bag)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    SetCapacity (bag.GetLength ());
                    this->AddAll (bag);
                }
                template    <typename T, typename BAG_TRAITS>
                Bag_Array<T, BAG_TRAITS>::Bag_Array (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    if (start != end) {
                        SetCapacity (end - start);
                        this->AddAll (start, end);
                    }
                }
                template    <typename T, typename BAG_TRAITS>
                inline  Bag_Array<T, BAG_TRAITS>::Bag_Array (const Bag_Array<T, BAG_TRAITS>& bag)
                    : inherited (static_cast<const inherited&> (bag))
                {
                }
                template    <typename T, typename BAG_TRAITS>
                inline  Bag_Array<T, BAG_TRAITS>&   Bag_Array<T, BAG_TRAITS>::operator= (const Bag_Array<T, BAG_TRAITS>& bag)
                {
                    inherited::operator= (static_cast<const inherited&> (bag));
                    return *this;
                }
                template    <typename T, typename BAG_TRAITS>
                inline  const typename Bag_Array<T, BAG_TRAITS>::Rep_&  Bag_Array<T, BAG_TRAITS>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T, typename BAG_TRAITS>
                inline  typename Bag_Array<T, BAG_TRAITS>::Rep_&    Bag_Array<T, BAG_TRAITS>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T, typename BAG_TRAITS>
                inline  void    Bag_Array<T, BAG_TRAITS>::Compact ()
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().fData_.Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename BAG_TRAITS>
                inline  size_t  Bag_Array<T, BAG_TRAITS>::GetCapacity () const
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        return (GetRep_ ().fData_.GetCapacity ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename BAG_TRAITS>
                inline  void    Bag_Array<T, BAG_TRAITS>::SetCapacity (size_t slotsAlloced)
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().fData_.SetCapacity (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Bag_Array_inl_ */
