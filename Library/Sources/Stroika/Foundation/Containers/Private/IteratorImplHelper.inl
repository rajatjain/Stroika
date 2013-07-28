/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
                 *** IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR> **
                 ********************************************************************************
                 */
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR>
                inline  IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR>::IteratorImplHelper_ (ContainerRepLockDataSupport_* sharedLock, PATCHABLE_CONTAINER* data)
                    : inherited ()
                    , fLockSupport (*sharedLock)
                    , fIterator (data)
                {
                    RequireNotNull (sharedLock);
                    RequireNotNull (data);
                    fIterator.More (static_cast<T*> (nullptr), true);   //tmphack cuz current backend iterators require a first more() - fix that!
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR>
                typename Iterator<T>::SharedIRepPtr IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR>::Clone () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport) {
                        return typename Iterator<T>::SharedIRepPtr (new IteratorImplHelper_ (*this));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR>
                void    IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR>::More (Memory::Optional<T>* result, bool advance)
                {
                    RequireNotNull (result);
                    CONTAINER_LOCK_HELPER_START (fLockSupport) {
#if 1
                        fIterator.More (result, advance);
#else
                        // To fix THIS - we have to fix the CONCRETE ForwardIterator types to have a More() that returns optional!
                        T val_Must_Fix_So_Doesnt_Req_DEFCTOR;
                        if (fIterator.More (&val_Must_Fix_So_Doesnt_Req_DEFCTOR, advance)) {
                            *result = val_Must_Fix_So_Doesnt_Req_DEFCTOR;
                        }
                        else {
                            result->clear ();
                        }
#endif
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR>
                bool    IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR>::StrongEquals (const typename Iterator<T>::IRep* rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                }


            }
        }
    }
}


#endif /* _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_ */
