/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/LinkedList.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ******************* Set_LinkedList<T, TRAITS>::Rep_ ****************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   Set_LinkedList<T, TRAITS>::Rep_ : public Set<T, TRAITS>::_IRep {
                private:
                    typedef typename    Set<T, TRAITS>::_IRep  inherited;

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

                    // Set<T, TRAITS>::_IRep overrides
                public:
                    virtual bool                Equals (const typename Set<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool                Contains (T item) const override;
                    virtual Memory::Optional<T> Lookup (T item) const override;
                    virtual void                RemoveAll () override;
                    virtual void                Add (T item) override;
                    virtual void                Remove (T item) override;
                    virtual void                Remove (const Iterator<T>& i) override;

                private:
                    typedef Private::PatchingDataStructures::LinkedList <
                    T,
                    Private::DataStructures::LinkedList_DefaultTraits<T, typename TRAITS::EqualsCompareFunctionType>
                    >       DataStructureImplType_;

                private:
                    typedef typename Private::IteratorImplHelper_<T, DataStructureImplType_>    IteratorRep_;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;
                };


                /*
                ********************************************************************************
                ************************ Set_LinkedList<T, TRAITS>::Rep_ ***********************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  Set_LinkedList<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  Set_LinkedList<T, TRAITS>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                typename Iterable<T>::_SharedPtrIRep  Set_LinkedList<T, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  Set_LinkedList<T, TRAITS>::Rep_::MakeIterator () const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (&NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    Iterator<T> tmp = Iterator<T> (tmpRep);
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T, typename TRAITS>
                size_t  Set_LinkedList<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Set_LinkedList<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void      Set_LinkedList<T, TRAITS>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     Set_LinkedList<T, TRAITS>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename TRAITS>
                bool    Set_LinkedList<T, TRAITS>::Rep_::Equals (const typename Set<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    Set_LinkedList<T, TRAITS>::Rep_::Contains (T item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return fData_.Lookup (item) != nullptr;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Memory::Optional<T> Set_LinkedList<T, TRAITS>::Rep_::Lookup (T item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        const T*    l = fData_.Lookup (item);
                        return (l == nullptr) ? Memory::Optional<T> () : Memory::Optional<T> (*l);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_::Add (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (it.Current (), item)) {
                                return;
                            }
                        }
                        fData_.Append (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_::Remove (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (it.Current (), item)) {
                                fData_.RemoveAt (it);
                                return;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ************************ Set_LinkedList<T, TRAITS> *****************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                Set_LinkedList<T, TRAITS>::Set_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T, typename TRAITS>
                inline  Set_LinkedList<T, TRAITS>::Set_LinkedList (const Set_LinkedList<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T>
                inline  Set_LinkedList<T, TRAITS>::Set_LinkedList (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    this->AddAll (s);
                }
                template    <typename T, typename TRAITS>
                inline  Set_LinkedList<T, TRAITS>&   Set_LinkedList<T, TRAITS>::operator= (const Set_LinkedList<T, TRAITS>& rhs)
                {
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }
                template    <typename T, typename TRAITS>
                inline  const typename Set_LinkedList<T, TRAITS>::Rep_&  Set_LinkedList<T, TRAITS>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T, typename TRAITS>
                inline  typename Set_LinkedList<T, TRAITS>::Rep_&    Set_LinkedList<T, TRAITS>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<Rep_&> (inherited::_GetRep ()));
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_ */
