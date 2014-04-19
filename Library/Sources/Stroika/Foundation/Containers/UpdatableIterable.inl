/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"

#ifndef _Stroika_Foundation_Containers_UpdatableIterable_inl_
#define _Stroika_Foundation_Containers_UpdatableIterable_inl_

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************************* UpdatableIterable<T> *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  UpdatableIterable<T>::UpdatableIterable (const _SharedPtrIRep& rep) noexcept
:
            inherited (rep)
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
            , fWriteMutex_ ()
#endif
            {
            }
            template    <typename T>
            inline  UpdatableIterable<T>::UpdatableIterable (const UpdatableIterable<T>& from) noexcept
:
            inherited (from)
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
            , fWriteMutex_ ()
#endif
            {
            }
            template    <typename T>
            inline  UpdatableIterable<T>::UpdatableIterable (UpdatableIterable<T>&& from) noexcept
:
            inherited (std::move (from))
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
            , fWriteMutex_ ()
#endif
            {
            }
            template    <typename T>
            inline  UpdatableIterable<T>::UpdatableIterable (_SharedPtrIRep&& rep) noexcept
:
            inherited (std::move (rep))
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
            , fWriteMutex_ ()
#endif
            {
            }
            template    <typename T>
            inline  UpdatableIterable<T>&    UpdatableIterable<T>::operator= (const UpdatableIterable<T>& rhs)
            {
                inherited::operator = (rhs);
                return *this;
            }


            /*
             ********************************************************************************
             *** UpdatableIterableHelper_<CONTAINER_OF_T, T>::_SafeReadWriteRepAccessor *****
             ********************************************************************************
             */
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  UpdatableIterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_SafeReadWriteRepAccessor (UpdatableIterable<T>* iterableEnvelope)
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
                : fEnvelopeWriteLock_ (iterableEnvelope->fWriteMutex_)
#endif
#if     qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
                , fAccessor_ (iterableEnvelope->fRep_)
                , fIterableEnvelope (iterableEnvelope)
#else
                , fRef_ (*static_cast<REP_SUB_TYPE*> (&iterableEnvelope->_GetRep ()))
#endif
            {
                RequireNotNull (iterableEnvelope);
            }
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  UpdatableIterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::~_SafeReadWriteRepAccessor ()
            {
#if     qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
                // @todo - CAREFUL ABOUT EXCEPTIONS HERE!
                //
                // Not as bad as it looks, since SharedByValue<>::operator= checks for no pointer change and does nothing
                fIterableEnvelope->fRep_ = fAccessor_;
#endif
            }
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  const REP_SUB_TYPE&    UpdatableIterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_ConstGetRep () const
            {
#if     qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
                EnsureMember (fAccessor_.get (), REP_SUB_TYPE);
                return static_cast<const REP_SUB_TYPE&> (*fAccessor_.get ());   // static cast for performance sake - dynamic cast in Ensure
#else
                return fRef_;
#endif
            }
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  REP_SUB_TYPE&    UpdatableIterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_GetWriteableRep ()
            {
#if     qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
                auto            uc  =   fAccessor_.use_count ();
                REP_SUB_TYPE*   r;
                if (uc == 1 or uc == 2) {
                    // static cast for performance sake - dynamic cast in Ensure() to be sure it was OK
                    r = const_cast<REP_SUB_TYPE*> (static_cast<const REP_SUB_TYPE*> (fAccessor_.cget ()));
                    if (uc == 2) {
                        /*
                         *  Not 100% sure this is right (explain bettter, think out more carefully)
                         *      -- LGP 2014-04-13
                         *
                         *  If base copy overwritten (that might give false positive and we might �add�
                         *  to other object but there was intrinsic race anyhow, and not exactly a bug on classlib part).
                         */
                        if (r != fIterableEnvelope->fRep_.cget ()) {
                            r = static_cast<REP_SUB_TYPE*> (fAccessor_.get (fIterableEnvelope));
                        }
                    }
                }
                else {
                    r = static_cast<REP_SUB_TYPE*> (fAccessor_.get (fIterableEnvelope));
                }
                EnsureMember (r, REP_SUB_TYPE);
                return *r;
#else
                return fRef_;
#endif
            }



        }
    }
}
#endif /* _Stroika_Foundation_Containers_UpdatableIterable_inl_ */
