/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_h_
#define _Stroika_Foundation_Execution_Synchronized_h_    1

#include    "../StroikaPreComp.h"

#include    <mutex>
#if     !qCompilerAndStdLib_shared_mutex_module_Buggy
#include    <shared_mutex>
#endif

#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Debug/Assertions.h"         // while RequireNotNull etc in headers --LGP 2015-06-11

#include    "Common.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * Description:
 *
 * TODO:
 *      @todo   KEY TODO IS MAKE READABLE REFERENCE USE shared_lock iff available
 *              and WritableReference pass in lock not using base class, and have it construct writable lock
 *              But all in a way that works when shared_lock not avaialble and degrades gracefully with template SFINAE traits crap.
 *
 *      @todo   More operator<, and other operator overloads
 *
 *      @todo   Tons of cleanups, orthoganiality, docs, etc.
 *
 *      @todo   consider/doc choice on explicit operator T ()
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
            ***** SCRAP NOTES - SOME OF THIS TEXT APPLIES TO NEW SYNCHO... CLEANUP

            * @todo
            *
            *
             *  The idea behind any of these synchronized classes is that they can be used freely from
             *  different threads without worry of data corruption. It is almost as if each operation were
             *  preceeded with a mutex lock (on that object) and followed by an unlock.
             *
             *  Note - we say 'almost' because there is no guarantee of atomicity: just consistency and safety.
             *
             *  For example, an implementation of something like "Apply(function)" might do atomic locks
             *  between each functional call, or some other way. But corruption or logical inconsistency
             *  will be avoided.
             *
             *  If one thread does a Read operation on Synchronized<T> while another does a write (modification)
             *  operation on Synchronized<T>, the Read will always return a consistent reasonable value, from
             *  before the modification or afterwards, but never a distorted invalid value.
             *
             *  This is very much closely logically the java 'synchronized' attribute, except that its
             *  not a language extension/attribute here, but rather a class wrapper. Its also implemented
             *  in the library, not the programming language.
             *
             *  Its also in a way related to std::atomic<> - in that its a class wrapper on another type,
             *  but the semantics it implements are moderately different than those in std::atomic,
             *  which is really just intended to operate on integers, and integer type things, and not on
             *  objects with methods.
             *
             *  \note   LIKE JAVA SYNCHRONIZED
             *          This is SIMPLE to use like the Java (and .net) synchronized attribute(lock) mechanism.
             *          But why does it not suffer from the same performance deficiit?
             *
             *          Because with Java - you mixup exceptions and assertions. With Stroika, we have builtin
             *          checking for races (Debug::AssertExternallySynchronizedLock) in most objects, so
             *          you only use Synchronized<> (or some other more performant mechanism) in the few places
             *          you need it.
             */


            /**
             *  MUTEX:
             *      We chose to make the default MUTEX recursive_mutex - since most patterns of use will be supported
             *      by this safely.
             *
             *      To use timed-locks, use timed_recursive_mutex.
             *
             *      If recursion is not necessary, and for highest performance, SpinLock will often work best.
             *
             *  \par Example Usage
             *      \code
             *      Synchronized<String> n;                                                 // SAME
             *      Synchronized<String,Synchronized_Traits<>> n;                           // SAME
             *      Synchronized<String,Synchronized_Traits<recursive_mutex>>   n;          // SAME
             *      \endcode
             *
             *  or slightly faster, but possibly slower or less safe (depnding on usage)
             *      Synchronized<String,Synchronized_Traits<SpinLock>>   n;
             */
            template    <typename MUTEX = recursive_mutex>
            struct  Synchronized_Traits {
                using   MutexType   =   MUTEX;
            };


            /**
             *  \par Example Usage
             *      \code
             *      Synchronized<String> n;                                                 // SAME
             *      Synchronized<String,Synchronized_Traits<>> n;                           // SAME
             *      Synchronized<String,Synchronized_Traits<recursive_mutex>>       n;      // SAME
             *      \endcode
             *
             *  or slightly faster, but possibly slower or less safe (depnding on usage)
             *      \code
             *      Synchronized<String,Synchronized_Traits<SpinLock>>              n;
             *      \endcode
             *
             *  or to allow timed locks
             *      \code
             *      Synchronized<String,Synchronized_Traits<timed_recursive_mutex>> n;
             *      \endcode
             *
             *  or to read-modify-update in place
             *      \code
             *      //nb: lock lasts til end of enclosing scope
             *      auto    lockedConfigData = fConfig_.get ();
             *      lockedConfigData->SomeValueChanged = 1;
             *      \endcode
             *
             *  or to read-modify-update with explicit temporary
             *      \code
             *      //nb: lock lasts til end of enclosing scope
             *      auto    lockedConfigData = fConfig_.get ();
             *      T       value = lockedConfigData;
             *      value.SomeValueChanged = 1;
             *      lockedConfigData.store (value);
             *      \endcode
             *
             *  or, because Synchronized<> has lock/unlock methods, it can be used with a lock_guard/make_unique_lock (if associated mutex recursive), as in:
             *      \code
             *      auto    critSec { Execution::make_unique_lock (n) };
             *      // lock to make sure someone else doesn't change n after we made sure it looked good
             *      if (looks_good (n)) {
             *          String a = n;
             *          a = a + a;
             *          n.store (a);
             *      }
             *      \endcode
             */
            template    <typename   T, typename TRAITS = Synchronized_Traits<>>
            class   Synchronized {
            public:
                using   MutexType = typename TRAITS::MutexType;

            public:
                class   ReadableReference;

            public:
                class   WritableReference;

            public:
                /**
                 */
                template    <typename ...ARGUMENT_TYPES>
                Synchronized (ARGUMENT_TYPES&& ...args);
                Synchronized (const Synchronized& src);

            public:
                /**
                 */
                nonvirtual  Synchronized&   operator= (const Synchronized& rhs);
                nonvirtual  Synchronized&   operator= (const T& rhs);

            public:
                /**
                 *  Synonym for load ()
                 ****
                 ***    @todo DECIDE IF WE WANT TO USE EXPLICIT
                 ****           http://stackoverflow.com/questions/27573928/are-explicit-conversion-operators-allowed-in-braced-initializer-lists
                 *** VERY UNCELAR - FIUDDLE WITH THIS
                 */
                nonvirtual   /*explicit*/ operator T () const;

            public:
                /**
                 *  Note - unlike operator->, load () returns a copy of the internal data, and only locks while fetching it, so that the
                 *  lock does not persist while using the result.
                 *
                 *  \par Example Usage
                 *      \code
                 *      Synchronized<Thread> sharedData;
                 *      sharedData.load ().AbortAndWaitTilDone ();  // copies thread and doesnt maintain lock during wait
                 *      sharedData->AbortAndWaitTilDone ();         // works off internal copy of thread object, and maintains the lock while accessing
                 *      \endcode
                 */
                nonvirtual  T   load () const;

            public:
                /**
                 */
                nonvirtual  void    store (const T& v);

            public:
                /**
                \\\experimental
                 *  @see load ()
                 */
                nonvirtual  const ReadableReference cget () const;

            public:
                /**
                \\\experimental
                 */
                nonvirtual  WritableReference get ();

            public:
                /**
                 *  \par Example Usage
                 *      auto    lockedConfigData = fConfig_.GetReference ();
                 *      fCurrentCell_ = lockedConfigData->fCell.Value (Cell::Short);
                 *      fCurrentPressure_ = lockedConfigData->fPressure.Value (Pressure::Low);
                 *
                 *  This is equivilent (if using a recursive mutex) to (COUNTER_EXAMPLE):
                 *      lock_guard<Synchronized<T,TRAITS>>  critSec (fConfig_);
                 *      fCurrentCell_ = fConfig_->fCell.Value (Cell::Short);
                 *      fCurrentPressure_ = fConfig_->fPressure.Value (Pressure::Low);
                 *
                 *  Except that the former only does the lock once, and works even with a non-recursive mutex.
                 */
                nonvirtual  const WritableReference GetReference () const;
                nonvirtual  WritableReference GetReference ();

            public:
                /*
                 *UNCLEAR BUT MAYBE DEPRECATE??? CONFUSING? OR overload const readonly and no-const writable, but with other stuff
                 * like SharedByValue() I found that I accidentally got the wrong one alot.
                 *  @see load ()
                 */
                nonvirtual  const ReadableReference operator-> () const;
                nonvirtual  WritableReference operator-> ();

            public:
                /**
                 */
                nonvirtual  void    lock ();

            public:
                /**
                 */
                nonvirtual  void    unlock ();

            private:
                T                   fDelegate_;
                mutable MutexType   fLock_;
            };


            /**
            * @todo KEY TODO IS MAKE READABLE REFERENCE USE shared_lock iff available
             */
            template    <typename   T, typename TRAITS>
            class  Synchronized<T, TRAITS>::ReadableReference {
            public:
                /**
                 */
                ReadableReference (const T* t, MutexType* m);
                ReadableReference (const ReadableReference& src) = delete;      // must move because both src and dest cannot have the unique lock
                ReadableReference (ReadableReference&& src);
                const ReadableReference& operator= (const ReadableReference& rhs) = delete;

            protected:
                /*
                 *  Due to http://open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3797.pdf
                 *      131) If the value returned by the operator-> function has class type, this may result in
                 *      selecting and calling another operator->
                 *      function. The process repeats until an operator-> function returns a value of non-class type.
                 *
                 *  So operator-> chaining works.
                 */
                using _ConstOperatorArrowReturnType = typename conditional<Configuration::has_operatorarrow<T>::value, T, const T*>::type;

            public:
                /**
                 */
                nonvirtual  _ConstOperatorArrowReturnType operator-> () const;

            public:
                /**
                 */
                nonvirtual  operator const T& () const;

            public:
                /**
                 */
                nonvirtual  T   load () const;

            protected:
                const T*                fT;
                unique_lock<MutexType>  l;
            };


            /**
             */
            template    <typename   T, typename TRAITS>
            class  Synchronized<T, TRAITS>::WritableReference : public Synchronized<T, TRAITS>::ReadableReference {
            public:
                /**
                 */
                WritableReference (T* t, MutexType* m);
                WritableReference (const WritableReference& src) = delete;      // must move because both src and dest cannot have the unique lock
                WritableReference (WritableReference&& src);
                const WritableReference& operator= (const WritableReference& rhs) = delete;
                const WritableReference& operator= (T rhs);

            public:
                /**
                 */
                nonvirtual  T* operator-> ();
                nonvirtual  const T* operator-> () const;

            public:
                /**
                 */
                nonvirtual      void    store (const T& v);
            };


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator< (const Synchronized<T, TRAITS>& lhs, T rhs);
            template    <typename   T, typename TRAITS>
            bool    operator< (T lhs, const Synchronized<T, TRAITS>& rhs);
            template    <typename   T, typename TRAITS>
            bool    operator< (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator<= (const Synchronized<T, TRAITS>& lhs, T rhs);
            template    <typename   T, typename TRAITS>
            bool    operator<= (T lhs, const Synchronized<T, TRAITS>& rhs);
            template    <typename   T, typename TRAITS>
            bool    operator<= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator== (const Synchronized<T, TRAITS>& lhs, T rhs);
            template    <typename   T, typename TRAITS>
            bool    operator== (T lhs, const Synchronized<T, TRAITS>& rhs);
            template    <typename   T, typename TRAITS>
            bool    operator== (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator!= (const Synchronized<T, TRAITS>& lhs, T rhs);
            template    <typename   T, typename TRAITS>
            bool    operator!= (T lhs, const Synchronized<T, TRAITS>& rhs);
            template    <typename   T, typename TRAITS>
            bool    operator!= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator>= (const Synchronized<T, TRAITS>& lhs, T rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator> (const Synchronized<T, TRAITS>& lhs, T rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            auto    operator^ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} ^ T {});
            template    <typename   T, typename TRAITS>
            auto    operator^ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} ^ T {});
            template    <typename   T, typename TRAITS>
            auto    operator^ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} ^ T {});


            /**
             */
            template    <typename   T, typename TRAITS>
            auto    operator* (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} * T {});
            template    <typename   T, typename TRAITS>
            auto    operator* (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} * T {});
            template    <typename   T, typename TRAITS>
            auto    operator* (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} * T {});


            /**
             */
            template    <typename   T, typename TRAITS>
            auto    operator+ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} + T {});
            template    <typename   T, typename TRAITS>
            auto    operator+ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} + T {});
            template    <typename   T, typename TRAITS>
            auto    operator+ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} + T {});


            /**
             */
            template    <typename   T, typename TRAITS, typename RHSTYPE>
            auto    operator-= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs->operator-= (rhs));


            /**
             */
            template    <typename   T, typename TRAITS, typename RHSTYPE>
            auto    operator+= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs->operator+= (rhs));


            /**
             */
            template    <typename   T, typename TRAITS>
            auto    operator- (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} - T {});
            template    <typename   T, typename TRAITS>
            auto    operator- (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} - T {});
            template    <typename   T, typename TRAITS>
            auto    operator- (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} - T {});



            /**
             * @todo  prototype - think out
             *  maybe based on ifdef use mutex or SpinLock.
             *
             *  Tentative definition is quick non recursive. Use like 'atomic'.
             */
            template    <typename   T>
            using   QuickSynchronized = Synchronized<T, Synchronized_Traits<mutex>>;


            /**
             * @todo  prototype - think out
             */
#if     qCompilerAndStdLib_shared_mutex_module_Buggy
            template    <typename   T>
            using   RWSynchronized = Synchronized<T, Synchronized_Traits<recursive_mutex>>;
#else
            template    <typename   T>
            using   RWSynchronized = Synchronized<T, Synchronized_Traits<shared_timed_mutex>>;
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Synchronized.inl"

#endif  /*_Stroika_Foundation_Execution_Synchronized_h_*/
