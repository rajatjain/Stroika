/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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


                using   Traversal::IteratorOwnerID;


                /*
                 ********************************************************************************
                 **************** Set_LinkedList<T, TRAITS>::Rep_ExternalSync_ ******************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   Set_LinkedList<T, TRAITS>::Rep_ExternalSync_ : public Set<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    Set<T, TRAITS>::_IRep;

                public:
                    using   _IterableSharedPtrIRep  =   typename Iterable<T>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ExternalSync_ () = default;
                    Rep_ExternalSync_ (const Rep_ExternalSync_& from) = delete;
                    Rep_ExternalSync_ (const Rep_ExternalSync_* from, IteratorOwnerID forIterableEnvelope);

                public:
                    nonvirtual  Rep_ExternalSync_& operator= (const Rep_ExternalSync_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_ExternalSync_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep  Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual Iterator<T>             MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                  GetLength () const override;
                    virtual bool                    IsEmpty () const override;
                    virtual void                    Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>             FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // Set<T, TRAITS>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override;
                    virtual bool                Equals (const typename Set<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool                Contains (ArgByValueType<T> item) const override;
                    virtual Memory::Optional<T> Lookup (ArgByValueType<T> item) const override;
                    virtual void                Add (ArgByValueType<T> item) override;
                    virtual void                Remove (ArgByValueType<T> item) override;
                    virtual void                Remove (const Iterator<T>& i) override;
#if     qDebug
                    virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override;
#endif

                private:
                    using   DataStructureImplType_  =   DataStructures::LinkedList<T, DataStructures::LinkedList_DefaultTraits<T, typename TRAITS::EqualsCompareFunctionType>>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_ExternalSync_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 ***************** Set_LinkedList<T, TRAITS>::Rep_InternalSync_ *****************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   Set_LinkedList<T, TRAITS>::Rep_InternalSync_ : public Set<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    Set<T, TRAITS>::_IRep;

                public:
                    using   _IterableSharedPtrIRep  =   typename Iterable<T>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_InternalSync_ () = default;
                    Rep_InternalSync_ (const Rep_InternalSync_& from) = delete;
                    Rep_InternalSync_ (Rep_InternalSync_* from, IteratorOwnerID forIterableEnvelope);

                public:
                    nonvirtual  Rep_InternalSync_& operator= (const Rep_InternalSync_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_InternalSync_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep  Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual Iterator<T>             MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                  GetLength () const override;
                    virtual bool                    IsEmpty () const override;
                    virtual void                    Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>             FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // Set<T, TRAITS>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override;
                    virtual bool                Equals (const typename Set<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool                Contains (ArgByValueType<T> item) const override;
                    virtual Memory::Optional<T> Lookup (ArgByValueType<T> item) const override;
                    virtual void                Add (ArgByValueType<T> item) override;
                    virtual void                Remove (ArgByValueType<T> item) override;
                    virtual void                Remove (const Iterator<T>& i) override;
#if     qDebug
                    virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override;
#endif

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::LinkedList <T, Private::ContainerRepLockDataSupport_, DataStructures::LinkedList_DefaultTraits<T, typename TRAITS::EqualsCompareFunctionType>>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 ***************** Set_LinkedList<T, TRAITS>::Rep_ExternalSync_ *****************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::Rep_ExternalSync_ (const Rep_ExternalSync_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T, typename TRAITS>
                typename Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::_IterableSharedPtrIRep  Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    return Iterable<T>::template MakeSharedPtr<Rep_ExternalSync_> (this, forIterableEnvelope);
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    return Iterator<T> (Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &this->fData_));
                }
                template    <typename T, typename TRAITS>
                size_t  Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::GetLength () const
                {
                    return fData_.GetLength ();
                }
                template    <typename T, typename TRAITS>
                bool  Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::IsEmpty () const
                {
                    return fData_.IsEmpty ();
                }
                template    <typename T, typename TRAITS>
                void      Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                    // use iterator (which currently implies lots of locks) with this->_Apply ()
                    fData_.Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    using   RESULT_TYPE     =   Iterator<T>;
                    using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                    SHARED_REP_TYPE resultRep;
                    auto iLink = fData_.FindFirstThat (doToElement);
                    if (iLink == nullptr) {
                        return RESULT_TYPE::GetEmptyIterator ();
                    }
                    resultRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &this->fData_);
                    resultRep->fIterator.SetCurrentLink (iLink);
                    // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                    return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                }
                template    <typename T, typename TRAITS>
                typename Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::_SharedPtrIRep  Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::CloneEmpty (IteratorOwnerID forIterableEnvelope) const
                {
                    return Iterable<T>::template MakeSharedPtr<Rep_ExternalSync_> ();
                }
                template    <typename T, typename TRAITS>
                bool    Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::Equals (const typename Set<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::Contains (ArgByValueType<T> item) const
                {
                    return fData_.Lookup (item) != nullptr;
                }
                template    <typename T, typename TRAITS>
                Memory::Optional<T> Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::Lookup (ArgByValueType<T> item) const
                {
                    const T*    l = fData_.Lookup (item);
                    return (l == nullptr) ? Memory::Optional<T> () : Memory::Optional<T> (*l);
                }
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::Add (ArgByValueType<T> item)
                {
                    // safe to use UnpatchedForwardIterator cuz locked and no updates
                    for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                        if (TRAITS::EqualsCompareFunctionType::Equals (it.Current (), item)) {
                            return;
                        }
                    }
                    fData_.Prepend (item);
                }
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::Remove (ArgByValueType<T> item)
                {
                    using   Traversal::kUnknownIteratorOwnerID;
                    for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                        if (TRAITS::EqualsCompareFunctionType::Equals (it.Current (), item)) {
                            fData_.RemoveAt (it);
                            return;
                        }
                    }
                }
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto&   mir =   dynamic_cast<const IteratorRep_&> (ir);
                    fData_.RemoveAt (mir.fIterator);
                }
#if     qDebug
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_ExternalSync_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                {
                    fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                }
#endif


                /*
                ********************************************************************************
                ******************* Set_LinkedList<T, TRAITS>::Rep_InternalSync_ ***************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  Set_LinkedList<T, TRAITS>::Rep_InternalSync_::Rep_InternalSync_ (Rep_InternalSync_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T, typename TRAITS>
                typename Set_LinkedList<T, TRAITS>::Rep_InternalSync_::_IterableSharedPtrIRep  Set_LinkedList<T, TRAITS>::Rep_InternalSync_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<T>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                    }
                    CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  Set_LinkedList<T, TRAITS>::Rep_InternalSync_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                        Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                    }
                    CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    return Iterator<T> (tmpRep);
                }
                template    <typename T, typename TRAITS>
                size_t  Set_LinkedList<T, TRAITS>::Rep_InternalSync_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.GetLength ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Set_LinkedList<T, TRAITS>::Rep_InternalSync_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.IsEmpty ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void      Set_LinkedList<T, TRAITS>::Rep_InternalSync_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     Set_LinkedList<T, TRAITS>::Rep_InternalSync_::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    using   RESULT_TYPE     =   Iterator<T>;
                    using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                    SHARED_REP_TYPE resultRep;
                    CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                        auto iLink = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        resultRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetCurrentLink (iLink);
                    }
                    CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                    return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                }
                template    <typename T, typename TRAITS>
                typename Set_LinkedList<T, TRAITS>::Rep_InternalSync_::_SharedPtrIRep  Set_LinkedList<T, TRAITS>::Rep_InternalSync_::CloneEmpty (IteratorOwnerID forIterableEnvelope) const
                {
                    if (fData_.HasActiveIterators ()) {
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<T>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    }
                    else {
                        return Iterable<T>::template MakeSharedPtr<Rep_InternalSync_> ();
                    }
                }
                template    <typename T, typename TRAITS>
                bool    Set_LinkedList<T, TRAITS>::Rep_InternalSync_::Equals (const typename Set<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    Set_LinkedList<T, TRAITS>::Rep_InternalSync_::Contains (ArgByValueType<T> item) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.Lookup (item) != nullptr;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Memory::Optional<T> Set_LinkedList<T, TRAITS>::Rep_InternalSync_::Lookup (ArgByValueType<T> item) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        const T*    l = fData_.Lookup (item);
                        return (l == nullptr) ? Memory::Optional<T> () : Memory::Optional<T> (*l);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_InternalSync_::Add (ArgByValueType<T> item)
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // safe to use UnpatchedForwardIterator cuz locked and no updates
                        for (typename DataStructureImplType_::UnpatchedForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (it.Current (), item)) {
                                return;
                            }
                        }
                        fData_.Prepend (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_InternalSync_::Remove (ArgByValueType<T> item)
                {
                    using   Traversal::kUnknownIteratorOwnerID;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (it.Current (), item)) {
                                fData_.RemoveAt (it);
                                return;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_InternalSync_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto&   mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     qDebug
                template    <typename T, typename TRAITS>
                void    Set_LinkedList<T, TRAITS>::Rep_InternalSync_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                 ********************************************************************************
                 ************************ Set_LinkedList<T, TRAITS> *****************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                Set_LinkedList<T, TRAITS>::Set_LinkedList (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (containerUpdateSafetyPolicy == ContainerUpdateIteratorSafety::eInternal ? _SharedPtrIRep (inherited::template MakeSharedPtr<Rep_InternalSync_> ()) : _SharedPtrIRep (inherited::template MakeSharedPtr<Rep_ExternalSync_> ()))
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  Set_LinkedList<T, TRAITS>::Set_LinkedList (const Set_LinkedList<T, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  Set_LinkedList<T, TRAITS>::Set_LinkedList (const initializer_list<T>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Set_LinkedList (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T, typename ENABLE_IF>
                inline  Set_LinkedList<T, TRAITS>::Set_LinkedList (const CONTAINER_OF_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Set_LinkedList (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_OF_T>
                inline Set_LinkedList<T, TRAITS>::Set_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Set_LinkedList (containerUpdateSafetyPolicy)
                {
                    AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  Set_LinkedList<T, TRAITS>&   Set_LinkedList<T, TRAITS>::operator= (const Set_LinkedList<T, TRAITS>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertRepValidType_ ();
                    return *this;
                }
                template    <typename T, typename TRAITS>
                inline  void    Set_LinkedList<T, TRAITS>::AssertRepValidType_ () const
                {
#if     qDebug
                    // @todo - FIX!!!
                    //typename inherited::template _SafeReadRepAccessor<Rep_InternalSync_> tmp { this };   // for side-effect of AssertMember
#endif
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_ */
