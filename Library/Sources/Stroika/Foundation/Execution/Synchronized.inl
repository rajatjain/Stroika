/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_inl_
#define _Stroika_Foundation_Execution_Synchronized_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             ******************************** Synchronized_Traits ***************************
             ********************************************************************************
             */
            template <typename MUTEX>
            inline void Synchronized_Traits<MUTEX>::LOCK_SHARED (MutexType& m)
            {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx{L"Synchronized_Traits<MUTEX>::LOCK_SHARED"};
                DbgTrace (L"(&m=%p)", &m);
#endif
                m.lock ();
            }
            template <typename MUTEX>
            inline void Synchronized_Traits<MUTEX>::UNLOCK_SHARED (MutexType& m)
            {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx{L"Synchronized_Traits<MUTEX>::UNLOCK_SHARED"};
                DbgTrace (L"(&m=%p)", &m);
#endif
                m.unlock ();
            }
#if !qCompilerAndStdLib_shared_mutex_module_Buggy
            template <>
            inline void Synchronized_Traits<shared_timed_mutex>::LOCK_SHARED (shared_timed_mutex& m)
            {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx{L"Synchronized_Traits<shared_timed_mutex>::LOCK_SHARED"};
                DbgTrace (L"(&m=%p)", &m);
#endif
                m.lock_shared ();
            }
            template <>
            inline void Synchronized_Traits<shared_timed_mutex>::UNLOCK_SHARED (shared_timed_mutex& m)
            {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx{L"Synchronized_Traits<shared_timed_mutex>::UNLOCK_SHARED"};
                DbgTrace (L"(&m=%p)", &m);
#endif
                m.unlock_shared ();
            }
#endif

            /*
             ********************************************************************************
             **************************** Synchronized<T, TRAITS> ***************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            template <typename... ARGUMENT_TYPES>
            inline Synchronized<T, TRAITS>::Synchronized (ARGUMENT_TYPES&&... args)
                : fDelegate_ (std::forward<ARGUMENT_TYPES> (args)...)
            {
            }
            template <typename T, typename TRAITS>
            inline Synchronized<T, TRAITS>::Synchronized (const Synchronized& src)
                : fDelegate_ (src.load ())
            {
            }
            template <typename T, typename TRAITS>
            inline auto Synchronized<T, TRAITS>::operator= (const Synchronized& rhs) -> Synchronized&
            {
                if (&rhs != this) {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    fDelegate_ = rhs.load ();
                }
                return *this;
            }
            template <typename T, typename TRAITS>
            inline auto Synchronized<T, TRAITS>::operator= (const T& rhs) -> Synchronized&
            {
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
                fDelegate_ = rhs;
                return *this;
            }
            template <typename T, typename TRAITS>
            inline Synchronized<T, TRAITS>::operator T () const
            {
                return load ();
            }
            template <typename T, typename TRAITS>
            inline T Synchronized<T, TRAITS>::load () const
            {
                shared_lock<const Synchronized<T, TRAITS>> fromCritSec{*this}; // use shared_lock if possible
                return fDelegate_;
            }
            template <typename T, typename TRAITS>
            inline void Synchronized<T, TRAITS>::store (Configuration::ArgByValueType<T> v)
            {
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
                fDelegate_ = v;
            }
            template <typename T, typename TRAITS>
            inline auto Synchronized<T, TRAITS>::cget () const -> ReadableReference
            {
                return ReadableReference (&fDelegate_, &fLock_);
            }
            template <typename T, typename TRAITS>
            inline auto Synchronized<T, TRAITS>::rwget () -> WritableReference
            {
                return WritableReference (&fDelegate_, &fLock_);
            }
            template <typename T, typename TRAITS>
            inline auto Synchronized<T, TRAITS>::operator-> () const -> ReadableReference
            {
                return ReadableReference (&fDelegate_, &fLock_);
            }
            template <typename T, typename TRAITS>
            inline void Synchronized<T, TRAITS>::lock_shared () const
            {
                TRAITS::LOCK_SHARED (fLock_);
            }
            template <typename T, typename TRAITS>
            inline void Synchronized<T, TRAITS>::unlock_shared () const
            {
                TRAITS::UNLOCK_SHARED (fLock_);
            }
            template <typename T, typename TRAITS>
            inline void Synchronized<T, TRAITS>::lock () const
            {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::lock"};
                DbgTrace (L"(&fLock_=%p)", &fLock_);
#endif
                fLock_.lock ();
            }
            template <typename T, typename TRAITS>
            inline void Synchronized<T, TRAITS>::unlock () const
            {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::unlock"};
                DbgTrace (L"(&fLock_=%p)", &fLock_);
#endif
                fLock_.unlock ();
            }

            /*
             ********************************************************************************
             ***************** Synchronized<T, TRAITS>::ReadableReference *******************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline Synchronized<T, TRAITS>::ReadableReference::ReadableReference (const T* t, MutexType* m)
                : fT (t)
                , l (*m)
            {
                RequireNotNull (t);
                RequireNotNull (m);
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"ReadableReference::CTOR -- locks (&fLock_=%p)", &m);
#endif
            }
            template <typename T, typename TRAITS>
            inline Synchronized<T, TRAITS>::ReadableReference::ReadableReference (ReadableReference&& src)
                : fT (src.fT)
                , l{move (src.l)}
            {
                src.fT = nullptr;
            }
            template <typename T, typename TRAITS>
            inline Synchronized<T, TRAITS>::ReadableReference::~ReadableReference ()
            {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"ReadableReference::DTOR -- locks (&fLock_=%p)", l.mutex ());
#endif
            }
            template <typename T, typename TRAITS>
            inline const T* Synchronized<T, TRAITS>::ReadableReference::operator-> () const
            {
                EnsureNotNull (fT);
                return fT;
            }
            template <typename T, typename TRAITS>
            inline const T& Synchronized<T, TRAITS>::ReadableReference::cref () const
            {
                EnsureNotNull (fT);
                return *fT;
            }
            template <typename T, typename TRAITS>
            inline Synchronized<T, TRAITS>::ReadableReference::operator const T& () const
            {
                EnsureNotNull (fT);
                return *fT;
            }
            template <typename T, typename TRAITS>
            inline T Synchronized<T, TRAITS>::ReadableReference::load () const
            {
                EnsureNotNull (fT);
                return *fT;
            }

            /*
             ********************************************************************************
             *************** Synchronized<T, TRAITS>::WritableReference *********************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline Synchronized<T, TRAITS>::WritableReference::WritableReference (T* t, MutexType* m)
                : ReadableReference (t, m)
            {
            }
            template <typename T, typename TRAITS>
            inline Synchronized<T, TRAITS>::WritableReference::WritableReference (WritableReference&& src)
                : ReadableReference (move (src))
            {
            }
            template <typename T, typename TRAITS>
            inline auto Synchronized<T, TRAITS>::WritableReference::operator= (T rhs) -> const WritableReference&
            {
                RequireNotNull (this->fT);
                // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
                *const_cast<T*> (this->fT) = rhs;
                return *this;
            }
            template <typename T, typename TRAITS>
            inline T* Synchronized<T, TRAITS>::WritableReference::operator-> ()
            {
                // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
                EnsureNotNull (this->fT);
                return const_cast<T*> (this->fT);
            }
            template <typename T, typename TRAITS>
            inline const T* Synchronized<T, TRAITS>::WritableReference::operator-> () const
            {
                return ReadableReference::operator-> ();
            }
            template <typename T, typename TRAITS>
            inline T& Synchronized<T, TRAITS>::WritableReference::rwref ()
            {
                // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
                EnsureNotNull (this->fT);
                return *const_cast<T*> (this->fT);
            }
            template <typename T, typename TRAITS>
            inline void Synchronized<T, TRAITS>::WritableReference::store (Configuration::ArgByValueType<T> v)
            {
                // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
                *const_cast<T*> (this->fT) = v;
            }

            /*
             ********************************************************************************
             *********************************** operator< **********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline bool operator< (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () < rhs;
            }
            template <typename T, typename TRAITS>
            inline bool operator< (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs < rhs.load ();
            }
            template <typename T, typename TRAITS>
            inline bool operator< (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () < rhs.load ();
            }

            /*
             ********************************************************************************
             ********************************** operator<= **********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline bool operator<= (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () <= rhs;
            }
            template <typename T, typename TRAITS>
            inline bool operator<= (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs <= rhs.load ();
            }
            template <typename T, typename TRAITS>
            inline bool operator<= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () <= rhs.load ();
            }

            /*
             ********************************************************************************
             ********************************** operator== **********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline bool operator== (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () == rhs;
            }
            template <typename T, typename TRAITS>
            inline bool operator== (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs == rhs.load ();
            }
            template <typename T, typename TRAITS>
            inline bool operator== (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () == rhs.load ();
            }

            /*
             ********************************************************************************
             ********************************** operator!= **********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline bool operator!= (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () != rhs;
            }
            template <typename T, typename TRAITS>
            inline bool operator!= (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs != rhs.load ();
            }
            template <typename T, typename TRAITS>
            inline bool operator!= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () != rhs.load ();
            }

            /*
             ********************************************************************************
             ********************************** operator>= **********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline bool operator>= (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () >= rhs;
            }

            /*
             ********************************************************************************
             *********************************** operator> **********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline bool operator> (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () > rhs;
            }

            /*
             ********************************************************************************
             ********************************** operator^ ***********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline auto operator^ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} ^ T{})
            {
                return lhs.load () ^ rhs;
            }
            template <typename T, typename TRAITS>
            inline auto operator^ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} ^ T{})
            {
                return lhs ^ rhs.load ();
            }
            template <typename T, typename TRAITS>
            inline auto operator^ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} ^ T{})
            {
                return lhs.load () ^ rhs.load ();
            }

            /*
             ********************************************************************************
             ********************************** operator* ***********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline auto operator* (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} * T{})
            {
                return lhs.load () * rhs;
            }
            template <typename T, typename TRAITS>
            inline auto operator* (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} * T{})
            {
                return lhs * rhs.load ();
            }
            template <typename T, typename TRAITS>
            inline auto operator* (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} * T{})
            {
                return lhs.load () * rhs.load ();
            }

            /*
             ********************************************************************************
             ********************************** operator+ ***********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline auto operator+ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} + T{})
            {
                return lhs.load () + rhs;
            }
            template <typename T, typename TRAITS>
            inline auto operator+ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} + T{})
            {
                return lhs + rhs.load ();
            }
            template <typename T, typename TRAITS>
            inline auto operator+ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} + T{})
            {
                return lhs.load () + rhs.load ();
            }

            /*
             ********************************************************************************
             ********************************** operator-= **********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS, typename RHSTYPE>
            inline auto operator-= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs.rwget ()->operator-= (rhs))
            {
                return lhs.rwget ()->operator-= (rhs);
            }

            /*
             ********************************************************************************
             ********************************** operator+= **********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS, typename RHSTYPE>
            inline auto operator+= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs.rwget ()->operator+= (rhs))
            {
                return lhs.rwget ()->operator+= (rhs);
            }

            /*
             ********************************************************************************
             ********************************** operator- ***********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline auto operator- (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} - T{})
            {
                return lhs.load () + rhs;
            }
            template <typename T, typename TRAITS>
            inline auto operator- (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} - T{})
            {
                return lhs - rhs.load ();
            }
            template <typename T, typename TRAITS>
            inline auto operator- (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} - T{})
            {
                return lhs.load () - rhs.load ();
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_Synchronized_inl_*/
