/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_
#define _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_  1

#include    "../../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Private {


                /*
                 ********************************************************************************
                 *IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR,PATCHABLE_CONTAINER_VALUE> *
                 ********************************************************************************
                 */
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                inline  IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::IteratorImplHelper_ (IteratorOwnerID owner, PATCHABLE_CONTAINER* data)
                    : inherited ()
                    , fIterator (owner, data)
                {
                    RequireNotNull (data);
                    fIterator.More (static_cast<DataStructureImplValueType_*> (nullptr), true);   //tmphack cuz current backend iterators require a first more() - fix that!
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                typename Iterator<T>::SharedIRepPtr IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::Clone () const
                {
                    AssertNotNull (fIterator.fPatchableContainer);
                    CONTAINER_LOCK_HELPER_START (fIterator.fPatchableContainer->fLockSupport) {
                        return typename Iterator<T>::SharedIRepPtr (new IteratorImplHelper_ (*this));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                IteratorOwnerID IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::GetOwner () const
                {
                    return fIterator.GetOwner ();
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                void    IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::More (Memory::Optional<T>* result, bool advance)
                {
                    RequireNotNull (result);
                    More_SFINAE_ (result, advance);
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                template    <typename CHECK_KEY>
                inline  void    IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::More_SFINAE_ (Memory::Optional<T>* result, bool advance, typename std::enable_if<is_same<T, CHECK_KEY>::value>::type*)
                {
                    RequireNotNull (result);
                    AssertNotNull (fIterator.fPatchableContainer);
                    CONTAINER_LOCK_HELPER_START (fIterator.fPatchableContainer->fLockSupport) {
                        fIterator.More (result, advance);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                template    <typename CHECK_KEY>
                inline  void    IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::More_SFINAE_ (Memory::Optional<T>* result, bool advance, typename std::enable_if < !is_same<T, CHECK_KEY>::value >::type*)
                {
                    RequireNotNull (result);
                    AssertNotNull (fIterator.fPatchableContainer);
                    CONTAINER_LOCK_HELPER_START (fIterator.fPatchableContainer->fLockSupport) {
                        Memory::Optional<DataStructureImplValueType_> tmp;
                        fIterator.More (&tmp, advance);
                        if (tmp.IsPresent ()) {
                            *result = *tmp;
                        }
                        else {
                            result->clear ();
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                bool    IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::Equals (const typename Iterator<T>::IRep* rhs) const
                {
                    RequireNotNull (rhs);
                    using   ActualIterImplType_ =   IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>;
                    RequireMember (rhs, ActualIterImplType_);
                    const ActualIterImplType_* rrhs =   dynamic_cast<const ActualIterImplType_*> (rhs);
                    AssertNotNull (rrhs);
                    AssertNotNull (fIterator.fPatchableContainer);
                    // @todo - FIX to use lock-2-at-a-time lock stuff on LHS and RHS
                    CONTAINER_LOCK_HELPER_START (fIterator.fPatchableContainer->fLockSupport) {
                        return fIterator.Equals (rrhs->fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


            }
        }
    }
}


#endif /* _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_ */
