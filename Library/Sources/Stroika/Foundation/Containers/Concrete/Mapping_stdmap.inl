/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <map>

#include    "../../Memory/BlockAllocated.h"
#include    "../STL/Compare.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                ********************************************************************************
                ************ Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ ****************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep {
                private:
                    using   inherited   =   typename    Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep;

                public:
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using   _IteratorOwnerID = typename inherited::_IteratorOwnerID;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
#if     qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverT_Buggy
                    virtual typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep  Clone () const override
                    {
                        return Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep (new Rep_ (*this));
                    }
#else
                    virtual typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep     Clone () const override;
#endif
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        MakeIterator (_IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                                              GetLength () const override;
                    virtual bool                                                IsEmpty () const override;
                    virtual void                                                Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep overrides
                public:
                    virtual bool                Equals (const typename Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep& rhs) const override;
                    virtual void                RemoveAll () override;
                    virtual Iterable<KEY_TYPE>  Keys () const override;
                    virtual bool                Lookup (KEY_TYPE key, Memory::Optional<VALUE_TYPE>* item) const override;
                    virtual void                Add (KEY_TYPE key, VALUE_TYPE newElt) override;
                    virtual void                Remove (KEY_TYPE key) override;
                    virtual void                Remove (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> i) override;

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::STLContainerWrapper <map <KEY_TYPE, VALUE_TYPE, STL::less<KEY_TYPE, typename TRAITS::KeyWellOrderCompareFunctionType>>>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, VALUE_TYPE>, DataStructureImplType_>;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;
                };


                /*
                ********************************************************************************
                ************************** Mapping_stdmap<Key,T>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    fData_.Invariant ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_.Invariant ();
                        from.fData_.Invariant ();
                        fData_ = from.fData_;
                        from.fData_.Invariant ();
                        fData_.Invariant ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverT_Buggy
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::MakeIterator (_IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS = const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr (new IteratorRep_ (&NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> (tmpRep);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                size_t  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        return (fData_.size ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        return (fData_.empty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void      Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>     Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool    Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Equals (const typename Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.clear_WithPatching ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterable<KEY_TYPE>    Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Keys () const
                {
                    AssertNotImplemented ();
                    return *(Iterable<KEY_TYPE>*)nullptr;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool    Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Lookup (KEY_TYPE key, Memory::Optional<VALUE_TYPE>* item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        auto i = fData_.find (key);
                        if (i == fData_.end ()) {
                            if (item != nullptr) {
                                item->clear ();
                            }
                            return false;
                        }
                        else {
                            if (item != nullptr) {
                                *item = i->second;
                            }
                            return true;
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Add (KEY_TYPE key, VALUE_TYPE newElt)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        auto i = fData_.find (key);
                        if (i == fData_.end ()) {
                            i = fData_.insert (pair<KEY_TYPE, VALUE_TYPE> (key, newElt)).first;
                            // no need to patch map<>
                        }
                        else {
                            i->second = newElt;
                        }
                        fData_.Invariant ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Remove (KEY_TYPE key)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        auto i = fData_.find (key);
                        if (i != fData_.end ()) {
                            fData_.erase_WithPatching (i);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Remove (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> i)
                {
                    const typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::IRep&    ir = i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto    mir = dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                **************** Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> ******************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_stdmap ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_stdmap (const Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                inline  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& src)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (src);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>&   Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
                {
                    inherited::operator= (rhs);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_ */
