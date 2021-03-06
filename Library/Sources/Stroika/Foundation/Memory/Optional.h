/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_h_
#define _Stroika_Foundation_Memory_Optional_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <optional>
#include <shared_mutex>

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/Empty.h"
#include "../Containers/Adapters/Adder.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Execution/NullMutex.h"
#include "BlockAllocated.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 *      NOTE TO SUGGEST TO C++ standards - 
 *          Things I miss most about my Optional versus std::optional
 *              >   Value () - what they call value_or - should take T{} as default argument. About 25% of teh time
 *                  thats what I want, and its much more clear/terse.
 *
 *              >   Accumulate method, and operator +=, operator-= etc overloads calling Accumulate(). Much simpler
 *                  and more elegant code with those methods.
 *
 *      @todo   BIG ITEM - MOSTLY get rid of this. Maybe not totally. But std::optional is pretty good, and sensible
 *              to use in Stroika. If I can find a way to extend it compatibly (say by subclassing and object slicing)
 *              then perhaps keep using this class, but change all my API calls to reference std::optional.
 *
 *
 *      @todo   Consider if we should maintain thread unsfafe peek() method.
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-456 opertor= cleanups (typename U U&&;
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-557 - Use (lock_guard<MutexBase_> { from },..) in mem-initializers (constexpr functions problem)
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-553 better integrate Optional with std::optional - so easy to go back and forth
 *
 *      @todo   See if I can get operator* working with ConstHolder_ (maybe more efficient). Or could return const&
 *              in release builds and T in DEBUG builds (so we can do context based debug lock/check).
 *
 *      @todo   FIX operator<, etc to match what we did for operator== and operator!=, and document!!!
 *              since COMPARE is part of traits we do NOT want to allow compare with differnt traits (so MUST FIX EUQalas as well)
 *
 *      @todo   COULD make operator==, operator<, etc compares more efficeint when comparing with T
 *              by adding a slew (3x) more overloads. (DID TO SOME EXTENT BUT COULD IMPROVE); And could improve iterop
 *              by adding operator XXX (where XXX is ==, etc) with (T, OPTIONAL);
 */

namespace Stroika::Foundation::Characters {
    class String;
    template <typename T>
    String ToString (const T&);
}

namespace Stroika::Foundation::Memory {

    using std::byte;

    /**
     *  This storage style is the default, and is usually fastest. However, it requires the
     *  sizeof (T) be know at the time Optional<T> is used (so not forward declared).
     *
     *  \note   This is NOT meant to be used by itself. This is only to be used as a template argument to Optional<>
     *
     *  \note   \em Implementation Detail
     *              We put fValue_ ahead of fBuffer_ because the data is nearly always accessed through
     *              fValue_ and putting it first probably makes its offset zero which may make for a faster
     *              reference (deref without offset)
     */
    template <typename T>
    struct Optional_Traits_Inplace_Storage {
        static constexpr bool kIncludeDebugExternalSync = qDebug and not is_literal_type<T>::value;

        /*
         *  To make Optional<> work with constexpr construction (by value) - we may need eliminate the Optional
         *  destructor (to satisfy the rules for LiteralType in C++14). But there doesnt appear to be a direct
         *  SFINAE way to conditionally provide a destructor, besides providing alternates for the base class (or member classes)
         *  and selecting whole base (or member) clases. Thats what this StorageType_<HAS_DESTRUCTOR> does.
         */
        template <typename TT, bool HAS_DESTRUCTOR>
        struct StorageType_;
        template <typename TT>
        struct StorageType_<TT, false> {
        private:
            /*
             *  Use union - so we can initialize CTOR for constexpr right part of union, and have no DTOR.
             *  Use fEngaged bool instead of pointer - because C++ (at least g++/clang++) don't allow pointer to
             *  constexpr value, where fEngagedValue_ not clearly marked constexpr (maybe no matter what).
             */
            union {
                Configuration::Empty fEmpty_;
                T                    fEngagedValue_;
            };
            bool fEngaged_{false};

        public:
            constexpr StorageType_ () noexcept;
            constexpr StorageType_ (const T& src);
            constexpr StorageType_ (T&& src);
            constexpr StorageType_ (const StorageType_& src);
            constexpr StorageType_ (StorageType_&& src);

            nonvirtual StorageType_& operator= (const T& rhs);
            nonvirtual StorageType_& operator= (T&& rhs);
            nonvirtual StorageType_& operator= (const StorageType_& rhs);
            nonvirtual StorageType_& operator= (StorageType_&& rhs);

            nonvirtual void destroy ();
            nonvirtual T*                 peek ();
            nonvirtual constexpr const T* peek () const;

        private:
            template <typename ARGT, typename USE_T = T, enable_if_t<is_copy_assignable_v<USE_T>>* = nullptr>
            nonvirtual void Assign_ (ARGT&& arg);
            template <typename ARGT, typename USE_T = T, enable_if_t<not is_copy_assignable_v<USE_T>>* = nullptr>
            nonvirtual void Assign_ (ARGT&& arg /*, const T_IS_NOT_ASSIGNABLE* = nullptr*/);
        };
        template <typename TT>
        struct StorageType_<TT, true> {
        private:
            T* fValue_{nullptr};
            DISABLE_COMPILER_MSC_WARNING_START (4324) // structure was padded due to alignment specifier
            union {
                Configuration::Empty fEmpty_;

#if qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy
                // VERY weirdly - alignas(alignment_of<T>)   - though WRONG (needs ::value - and that uses alignas) works
#else
                alignas (T)
#endif
                byte fBuffer_[sizeof (T)]; // intentionally uninitialized
            };
            DISABLE_COMPILER_MSC_WARNING_END (4324)

        public:
            constexpr StorageType_ () noexcept;
            StorageType_ (const T& src);
            StorageType_ (T&& src);
            StorageType_ (const StorageType_& src);
            StorageType_ (StorageType_&& src);
            ~StorageType_ ();

            nonvirtual StorageType_& operator= (const T& rhs);
            nonvirtual StorageType_& operator= (T&& rhs);
            nonvirtual StorageType_& operator= (const StorageType_& rhs);
            nonvirtual StorageType_& operator= (StorageType_&& rhs);

            nonvirtual void destroy ();
            nonvirtual T*    peek ();
            nonvirtual const T* peek () const;

        private:
            template <typename ARGT, typename USE_T = T, enable_if_t<is_copy_assignable_v<USE_T>>* = nullptr>
            nonvirtual void Assign_ (ARGT&& arg);
            template <typename ARGT, typename USE_T = T, enable_if_t<not is_copy_assignable_v<USE_T>>* = nullptr>
            nonvirtual void Assign_ (ARGT&& arg /*, const T_IS_NOT_ASSIGNABLE* = nullptr*/);
        };
        using StorageType = StorageType_<T, not is_trivially_destructible<T>::value>;
    };

    /**
     *  This storage is usually somewhat slower than Optional_Traits_Inplace_Storage (except
     *  if doing lots of moves).
     *
     *  Its main advantage is that it doesn't require the sizeof (T) be know at the time Optional<T>
     *  is used (so T can be forward declared).
     *
     *  \note   This is NOT meant to be used by itself. This is only to be used as a template argument to Optional<>
     */
    template <typename T>
    struct Optional_Traits_Blockallocated_Indirect_Storage {
        static constexpr bool kIncludeDebugExternalSync = qDebug;
        struct StorageType {
        private:
            T* fValue_{nullptr};

        public:
            constexpr StorageType () = default;
            StorageType (const T&);
            StorageType (T&& src);
            StorageType (const StorageType& src);
            StorageType (StorageType&& src) noexcept;
            ~StorageType ();

            nonvirtual StorageType& operator= (const T& rhs);
            nonvirtual StorageType& operator= (T&& rhs);
            nonvirtual StorageType& operator= (const StorageType& rhs);
            nonvirtual StorageType& operator= (StorageType&& rhs) noexcept;

            nonvirtual void destroy ();
            nonvirtual T*    peek ();
            nonvirtual const T* peek () const;
        };
    };

    /**
     *  By default, Optional (and Optional_Traits_Default) use  Optional_Traits_Inplace_Storage
     *  since its probably generally more efficient, and more closely aligned with
     *  std::optional<> behavior. Also - only Optional_Traits_Inplace_Storage works with constexpr.
     */
    template <typename T>
    using Optional_Traits_Default = Optional_Traits_Inplace_Storage<T>;

    template <typename T, typename TRAITS>
    class Optional;

    namespace Private_ {

        template <typename, typename>
        struct IsOptional_impl_ : false_type {
        };
        template <typename T, typename TRAITS>
        struct IsOptional_impl_<Optional<T, TRAITS>, TRAITS> : true_type {
        };

        template <typename T, typename TRAITS>
        struct IsOptional_ : public IsOptional_impl_<std::remove_cv_t<std::remove_reference_t<T>>, TRAITS> {
        };
    }

    /**
     *  Optional<T> can be used to store an object which may or may not be present. This can be
     *  used in place of sentinal values (for example if no obvious sentinal value presents itself),
     *  and instead of explicitly using pointers and checking for null all over.
     *
     *  When dereferencing an 'IsMissing' value, there are three plausible interpretions:
     *      -   Return the default value T()
     *      -   throw bad_alloc()
     *      -   Assertion error
     *
     *  Because the 'default value' isn't always well defined, and because throwing bad_alloc
     *  runs the risk of producing surprising exceptions, we treat dereferencing an 'IsMissing'
     *  Optional<T> as an Assertion Error.
     *
     *  However, see @ref Value()
     *
     *  \note   Design Note: Stroika Optional versus std::optional
     *          o   First, std::optional doesn't exist yet, and probably wont enter the language until c++17.
     *          o   Optional<> integrates with and leverages Stroika's assertion mechanism, making
     *              it much easier to find bugs with code using Optional
     *          o   Debug::AssertExternallySynchronizedLock, and other stratgies and API structure
     *              to assure (in debug releases) that the Stroika Optional class is used in an threadsafe manner.
     *          o   multiple trait-based storage strategies for implementing optional
     *              o   Optional_Traits_Inplace_Storage
     *                  Nearly identical to what is used by std::optional, and probably fastest
     *              o   Optional_Traits_Blockallocated_Indirect_Storage
     *                  Fastest for when you do lots of copies of Optional, and allows use
     *                  of Optional with forward-declared structs (so doesnt need to know size
     *                  at declare time)
     *          o   Stroika's operator*() -> T, wheras std::optional::operator* -> T&. This choice
     *              makes it slightly safer (more easily detectable race issues), but slightly less
     *              conveneint and certainly incompatible with std::optional<>.
     *          o   std::optional doesn't allow assign of optional<T> if assign to <T> wasn't allowed.
     *              But semantically, this is the same as destroy followed by create, so Stroika allows
     *              this in either case and selects the T::operator=() if available (in implementation) but
     *              use destroy, and T::T (const T& or T&&) if needed.
     *
     *  \note   lifetime example
     *          Optional<pair<int,int>> x;
     *          Synchronized<Optional<pair<int,int>>> sx;
     *
     *          x->first = 3;   // safe: temporeray holder created by operator->, which contains internal pointer, and keeps lock
     *                          // and deref and assigment happens, and then holder temporary destroyed at end of statement
     *                          // which releases the lock
     *
     *          sx.rwget ()->second = 4;
     *                          // works with threads, in that Synchronized<>::operator-> returns a temporary proxy object
     *                          // which owns a mutex lock and then the rest is just as above.
     *                          // Note - the synchronized proxy is created first, and destroyed last, so it prevents
     *                          // any changes during setup/elipogue of Optional handling.
     *
     *  \note   To use Optional with un-copyable things, use:
     *          Optional<NotCopyable>   n2 (move (NotCopyable ()));    // use r-value reference to move
     *
     *  \note   \em Design-Note - why not SharedByValue<T>
     *      -   We considered using the SharedByValue<T> template which would be more efficient
     *          when we copy Optional objects of type T, where T is expensive to copy. But for
     *          T where T is cheap to copy, just using BlockAllocated<T> is probably cheaper
     *          to copy. Which is more common? Who knows, so this probably doesn't matter much.
     *          Also - consider perhaps if there is a way to use a TRAITS argument?
     *          If we chose to use SharedByValue<T>, then non-const methods like 'operator* ()'
     *          would just 'BreakReferences'.
     *
     *  \note   \em Design-Note - Internal pointers
     *      -   The only internal pointers returned (without checks) are in the peek () method. For this reason, we are
     *          considering losing the peek() method.
     *
     *  \note   \em Design-Note - why no operator T()
     *      -   We considered having an operator T () method. This has advantages, in that
     *          it makes more seemless replacing use of a T with an Optional<T>. But it has
     *          the disadvantage that, when coupled with the Optional<T> (T) CTOR, you can
     *          get overloading problems.
     *      -   Plus, one must carefully check each use of a variable of type T, being converted
     *          to type Optional<T>, so being forced to say "*" first isn't totally unreasonable.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#ExternallySynchronized">ExternallySynchronized</a>
     *          Const methods can be used by multiple readers at a time.
     *
     *  \note   Tried using Configuration::ArgByValueType<T>, but for reasons I don't fully understand, this doesn't
     *          work (errors with forward optional of forward declared class, which we try to support wtih Optional_Indirect_Storage
     *          and using Configuration::ArgByValueType<> breaks that.
     *
     *  \note   \em Design-Note - Optional<void>
     *          I considered supporting Optional<void>, but there seemed no natural way to 'assign' something to it
     *          to make it present. I found this might be useful, for situations like:
     *              struct VariantFoo {
     *                  Optional<infoa> a;
     *                  Optional<extraInfoB> b;
     *                  Optional<void>  c;
     *              };
     *              where VariantFoo was basically a variant union, where the variant present
     *              was the one where the optional was there. But
     *              this can be done - I think - just as easily using 'bool c' for the variants with
     *              no extra data.
     *
     *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     */
    template <typename T, typename TRAITS = Optional_Traits_Default<T>>
    class Optional : private conditional_t<TRAITS::kIncludeDebugExternalSync, Debug::AssertExternallySynchronizedLock, Execution::NullMutex> {
    private:
        using inherited = conditional_t<TRAITS::kIncludeDebugExternalSync, Debug::AssertExternallySynchronizedLock, Execution::NullMutex>;

    private:
        using MutexBase_ = conditional_t<TRAITS::kIncludeDebugExternalSync, Debug::AssertExternallySynchronizedLock, Execution::NullMutex>;

    public:
        using TraitsType = TRAITS;

    public:
        /**
         */
        using value_type = T;

    public:
        /**
         *  \note   We have the two overloads of CTOR doing conversion - one where the types are essentially the same, and one where
         *          they are not. The only difference between the two is an explicit cast in the implementation and whether the conversion
         *          is implicit or explicit.
         *
         *  \note  @see http://en.cppreference.com/w/cpp/utility/optional/optional
         */
        constexpr Optional () = default;
        constexpr Optional (nullopt_t);
        template <typename T1>
        constexpr Optional (const optional<T1>& src)
            : Optional (src.has_value () ? Optional<T>{*src} : Optional<T>{})
        {
        }
        constexpr Optional (const Optional& from);
        constexpr Optional (Optional&& from);
        /*
         *  Note - in C++17, std::optional checks
         *      is_convertible_v<const T2&, T>
         *      (or std::is_convertible_v < T2 &&, T > in the rvalue-ref overload)
         *  to see if we make this constructor explicit.
         *
         *  In stroika - instead - we use
         *      is_same_v<typename std::decay<T>::type, typename std::common_type<T, T2>::type>
         *
         *  We may want to revisit this question, but the key is:
         *      Optional<double> d;
         *      Optional<uint64_t> t1   =   d ;                     // should generate warning or fail to compile (no explit match)
         *      Optional<uint64_t> t2   =   Optional<uint64_t> (d); // should not generate warning
         *
         *  using is_same/common_type does this a little better (at least for VS2k17RC1)
         */
        template <
            typename T2,
            typename TRAITS2,
            typename SFINAE_SAFE_CONVERTIBLE = enable_if_t<
                not Private_::IsOptional_<T2, TRAITS2>::value and
                is_constructible_v<T, const T2&> and
                is_same_v<typename decay<T>::type, typename common_type<T, T2>::type>>>
        Optional (const Optional<T2, TRAITS2>& from);
        template <
            typename T2,
            typename TRAITS2,
            typename SFINAE_UNSAFE_CONVERTIBLE = enable_if_t<
                not Private_::IsOptional_<T2, TRAITS2>::value and
                is_constructible_v<T, const T2&> and
                not is_same_v<typename decay<T>::type, typename common_type<T, T2>::type>>>
        explicit Optional (const Optional<T2, TRAITS2>& from, SFINAE_UNSAFE_CONVERTIBLE* = nullptr);
        template <
            typename T2,
            typename TRAITS2,
            typename SFINAE_SAFE_CONVERTIBLE = enable_if_t<
                not Private_::IsOptional_<T2, TRAITS2>::value and
                is_constructible_v<T, T2&&> and
                is_same_v<typename decay<T>::type, typename common_type<T, T2>::type>>>
        Optional (Optional<T2, TRAITS2>&& from);
        template <
            typename T2,
            typename TRAITS2,
            typename SFINAE_UNSAFE_CONVERTIBLE = enable_if_t<
                not Private_::IsOptional_<T2, TRAITS2>::value and
                is_constructible_v<T, T2&&> and
                not is_same_v<typename decay<T>::type, typename common_type<T, T2>::type>>>
        explicit Optional (Optional<T2, TRAITS2>&& from, SFINAE_UNSAFE_CONVERTIBLE* = nullptr);
        // @todo  more SFINAE checks needed
        template <
            typename U                       = T,
            typename SFINAE_SAFE_CONVERTIBLE = enable_if_t<
                not is_same_v<Optional<T, TRAITS>, U> and
                is_constructible_v<T, U&&> and
                is_convertible_v<U&&, T>>>
        constexpr Optional (U&& from);
        template <
            typename U                         = T,
            typename SFINAE_UNSAFE_CONVERTIBLE = enable_if_t<
                not is_same_v<Optional<T, TRAITS>, U> and
                is_constructible_v<T, U&&> and
                not is_convertible_v<U&&, T>>>
        constexpr explicit Optional (U&& from, SFINAE_UNSAFE_CONVERTIBLE* = nullptr);

    public:
        /**
         *  \req &rhs != this (see https://stroika.atlassian.net/browse/STK-556 and
         *      from 17.6.4.9 - http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3690.pdf
         *          � If a function argument binds to an rvalue reference parameter, the implementation may assume that
         *          this parameter is a unique reference to this argument.
         */
        // @todo see http://en.cppreference.com/w/cpp/utility/optional/optional - INCOMPLETE set of overloads
        nonvirtual Optional& operator= (nullopt_t);
        nonvirtual Optional& operator= (const Optional& rhs);
        nonvirtual Optional& operator= (Optional&& rhs);
        template <
            typename U                       = T,
            typename SFINAE_SAFE_CONVERTIBLE = enable_if_t<
                not Private_::IsOptional_<U, typename U::TraitsType>::value and
                is_constructible_v<T, U> and
                is_assignable<T&, U>::value and
                (is_scalar<T>::value or not is_same_v<decay_t<U>, T>)>>
        nonvirtual Optional& operator= (U&& rhs);

    public:
        /**
         *  'Constructor' taking const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* is to allow easier interoperability
         *  with code that uses null-pointers to mean 'is-missing': nullptr means missing, and if non null,
         *  derefrence and copy.
         *
         *  \par Example Usage
         *      \code
         *      float*  d1  =   nullptr;
         *      double* d2  =   nullptr;
         *      Assert (not Optional<double>::OptionalFromNullable (d1).has_value ());
         *      Assert (not Optional<double>::OptionalFromNullable (d2).has_value ());
         *      \endcode
         *
         *  \note   I tried making this an Optional<T> constructor overload, but it lead to dangerous confusion with things like
         *          URL url = URL (L"dyn:/StyleSheet.css?ThemeName=Cupertino", URL::eStroikaPre20a50BackCompatMode);
         *          VerifyTestResult (url.GetScheme () == L"dyn");
         *          // wchar_t* overload is optional gets STRING with value "d";
         */
        template <typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, typename SFINAE_SAFE_CONVERTIBLE = enable_if_t<Configuration::is_explicitly_convertible<RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, T>::value>>
        static Optional<T, TRAITS> OptionalFromNullable (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* from);

    public:
        // see if we can do this without getting in troublke with ambiguity - it will help make transiation to using optional in
        // Stroika APIs easier.
        template <typename T1>
        /*explicit*/ operator optional<T1> () const
        {
            return has_value () ? value () : optional<T1>{};
        }

    public:
        /**
         *  Erases (destroys) any present value for this Optional<T> instance. After calling clear (),
         *  the has_value () will return false.
         *
         *  @see reset ();
         */
        [[deprecated ("use =nullopt instead, since Stroika v2.1d4")]] nonvirtual void clear ();

    public:
        /**
         *  Erases (destroys) any present value for this Optional<T> instance. After calling reset (),
         *  the has_value () will return false.
         *
         *  \note   This named method mimics the shared_ptr<>::reset () API, which has analagous functionality
         *          and is the reason for this named method.
         *
         *  @see clear ();
         */
        nonvirtual void reset ();

    public:
        /**
         *  Returns true iff the Optional<T> has no valid value. Attempts to access the value of
         *  an Optional<T> (eg. through operator* ()) will result in an assertion error.
         */
        [[deprecated ("use not has_value() to match std::optional since Stroika v2.1d4")]] nonvirtual constexpr bool IsMissing () const noexcept; // means no value (it is optional!)

    public:
        /**
         *  Returns true iff the Optional<T> has a valid value (not IsMissing ());
         */
        [[deprecated ("use has_value() to match std::optional since Stroika v2.1d4")]] nonvirtual constexpr bool IsPresent () const noexcept;

    public:
        /**
         *  Returns true iff the Optional<T> has a valid value (not IsMissing ());
         *
         *  \note alias for std::optional<>::has_value ()
         */
        nonvirtual constexpr bool has_value () const noexcept;

    public:
        /**
         *  Return true iff has_value ().
         *
         *  \note   Design Note:
         *          I've long disliked type punning, and consider it a frequenty source of confusion. However,
         *          the compelling simplicity (and it appears to me clarity and commonness of):
         *              if (Optional<T> i = getValue ()) {
         *                  use_value (*i);
         *              }
         *          outweighs this concern.
         */
        nonvirtual explicit operator bool () const noexcept;

    public:
        /**
         *  Always safe to call. If !has_value, returns argument 'default' or 'sentinal' value.
         *
         *  @see CopyToIf
         *  @see CheckedValue
         *  @see OptionalValue ()
         *
         *  Notably differnt from @see http://en.cppreference.com/w/cpp/utility/optional/value - see CheckedValue () for optional::value() equivilent
         *
         *  \note 1-arg version of this is equivilent to optional<>::value_or (defaultValue);
         */
        [[deprecated ("use member value_or () or non-member ValueOrDefault - since Stroika v2.1d4")]] nonvirtual T Value (T defaultValue = T{}) const;

    public:
        // mimic stdc++ API
        nonvirtual T value_or (T defaultValue = T{}) const;

    public:
        /**
         * \brief return one of *this, or o, with first preference for which is engaged, and second preference for left-to-right.
         *
         *  So Equivilent to this->has_value ()? *this : o;
         *
         *  This is VERY similar to Value () - except that the default maybe optional, and this therefore returns an Optional<T>
         *
         *  @see Value ()
         */
        nonvirtual Optional<T, TRAITS> OptionalValue (const Optional<T, TRAITS>& o) const;

    public:
        /**
         *  Always safe to call. If !has_value, throws argument exception2ThrowIfMissing.
         *
         *  @see CopyToIf
         *  @see Value
         *
         *  @todo when we have std::optional support, make this default to std::bad_optional_access
         *
         *  This is roughly equivilent to @see http://en.cppreference.com/w/cpp/utility/optional/value
         */
        template <typename THROW_IF_MISSING_TYPE>
        nonvirtual T CheckedValue (const THROW_IF_MISSING_TYPE& exception2ThrowIfMissing = THROW_IF_MISSING_TYPE ()) const;

    public:
        /**
         *  Assign the value held by this optional if one is present to destination argument (pointer). Assigns from left to right.
         *
         *  The point of this to to faciltate a common idiom, where you want to maintain an existing value unless you
         *  get an update. This function is ANALAGOUS to
         *      if (o.has_value()) {
         *          destArgVal = *o;
         *      }
         *
         *  but can be done in a single line.
         *
         *  \par Example Usage
         *      \code
         *      int curValue = 3;
         *      Optional<long>  oVal = someMap.Lookup (KEY_VALUE);
         *      oVal.CopyToIf (&curValue);
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *      Optional<int> curValue;
         *      Optional<long>  oVal = someMap.Lookup (KEY_VALUE);
         *      oVal.CopyToIf (&curValue);      // curValue retains its value from before CopyToIf if oVal was missing
         *      \endcode
         *
         *  @see Value
         */
        template <typename CONVERTABLE_TO_TYPE = T>
        nonvirtual void CopyToIf (CONVERTABLE_TO_TYPE* to) const;

    public:
        /**
         *  EXPERIEMNTAL - 2015-04-24
         *  \brief  AccumulateIf () add in the argument value to this optional, but if both were missing leave 'this'
         *          as still missing, and if only RHS available, assign it to the left.
         *
         *  \par Example Usage
         *      \code
         *      Optional<int>   accumulator;
         *      Optional<int>   SomeFunctionToGetOptionalValue();
         *
         *      if (accumulator or (tmp = SomeFunctionToGetOptionalValue())) {
         *              accumulator = accumulator.Value () + tmp;
         *      }
         *      \endcode
         *      VERSUS
         *      \code
         *      accumulator.AccumulateIf (SomeFunctionToGetOptionalValue ());
         *      \endcode
         *
         *      \note   ITS CONFUSING direction of if-test for this versus CopyToIf
         */
        nonvirtual void AccumulateIf (const Optional<T>& rhsOptionalValue, const function<T (T, T)>& op = [](T lhs, T rhs) { return lhs + rhs; });

    private:
        /*
            *  Implementation detail to allow returning 'short lived' internal pointers to optional which preserve our
            *  Debug::AssertExternallySynchronizedLock locking (so we can detect data races).
            */
        struct ConstHolder_ {
            const Optional* fVal;
#if qDebug
            shared_lock<const MutexBase_> fCritSec_;
#endif
            ConstHolder_ (const ConstHolder_&) = delete;
            ConstHolder_ (const Optional* p);
            ConstHolder_ (ConstHolder_&& from);
            ConstHolder_& operator= (const ConstHolder_&) = delete;
            ConstHolder_& operator= (T) = delete;
            const T*      operator-> () const;
            operator const T& () const;
            const T& operator* () const;
        };
        struct MutableHolder_ {
            Optional* fVal;
#if qDebug
            unique_lock<MutexBase_> fCritSec_;
#endif
            MutableHolder_ (const MutableHolder_&) = delete;
            MutableHolder_ (Optional* p);
            MutableHolder_ (MutableHolder_&& from);
            MutableHolder_& operator= (const MutableHolder_&) = delete;
            MutableHolder_& operator= (T) = delete;
            T*              operator-> ();
            operator T& ();
            T& operator* ();
        };

    public:
        /**
         *  Returns nullptr if value is missing
         *
         *  \warning
         *      This method returns a pointer internal to (owned by) Optional<T>, and its lifetime
         *      is only guaranteed until the next non-const method call on this Optional<T> instance.
         */
        nonvirtual T*    peek ();
        nonvirtual const T* peek () const;

    public:
        /**
         *  \pre (has_value ())
         *
         *  \warning
         *      This method returns a pointer internal to (owned by) Optional<T>, and its lifetime
         *      is only guaranteed until the next method call on this Optional<T> instance.
         *
         *  This is really just syntactic sugar equivalent to peek () - except that it requires
         *  not-null - but more convenient since it allows the use of an optional to
         *  syntactically mirror dereferencing a pointer.
         *
         *  Another important difference is that it CHECKS lifetime/locking rules using
         *  @Debug::AssertExternallySynchronizedLock in debug builds.
         */
        nonvirtual MutableHolder_ operator-> ();
        nonvirtual ConstHolder_ operator-> () const;

    public:
        /**
         *  \pre (has_value ())
         *
         *  \note   We chose to return T, instead of const T&, because for smaller objects this works as
         *          well or better, and because it allows us to control the lifetime of the underlying memory,
         *          and so we can apply assertions etc to assure valid lifetime management.
         */
        nonvirtual T operator* () const;

    public:
        /**
         *  @see Characters::ToString()
         */
        template <typename STRING_TYPE = Characters::String>
        nonvirtual STRING_TYPE ToString () const;

    public:
        /**
         *  Each of these methods (+=, -=, *=, /= are defined iff the underlying operator is defined on T.
         *
         *  These are aliases for calls to AccumulateIf () - with the appropirate operator. So its OK if the RHS or LHS is optional.
         *  @see AccumulateIf
         */
        nonvirtual Optional& operator+= (const Optional& rhs);
        nonvirtual Optional& operator-= (const Optional& rhs);
        nonvirtual Optional& operator*= (const Optional& rhs);
        nonvirtual Optional& operator/= (const Optional& rhs);

    public:
        /**
         *  Return true if *this logically equals rhs. Note if either side 'has_value()'
         *  is different, then they compare as not Equals()
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Equals (const Optional& rhs, const EQUALS_COMPARER& equalsComparer = {}) const;
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Equals (T rhs, const EQUALS_COMPARER& equalsComparer = {}) const;

    public:
        /**
         *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
         *  Somewhat arbitrarily, treat NOT-PROVIDED (!has_value) as < any value of T
         */
        template <typename THREEWAY_COMPARER = Common::ThreeWayCompare<T>>
        nonvirtual int Compare (const Optional& rhs, const THREEWAY_COMPARER& comparer = {}) const;
        template <typename THREEWAY_COMPARER = Common::ThreeWayCompare<T>>
        nonvirtual int Compare (T rhs, const THREEWAY_COMPARER& comparer = {}) const;

    public:
        /**
         *  Mimmic (except for now for particular exception thrown) value() api, and don't support non-const variation (for now).
         */
        nonvirtual T value () const;

    private:
        typename TRAITS::StorageType fStorage_;

    private:
        template <typename T2, typename TRAITS2>
        friend class Optional;
    };

    /**
     *  EXPERIEMNTAL - 2015-04-24
     *  \brief  AccumulateIf () add in the rhs argument value to lhs optional, but if both were missing leave 'lhs'
     *          as still missing, and if only RHS available, assign it to the left.
     *
     *  \par Example Usage
     *      \code
     *          optional<int>   accumulator;
     *          optional<int>   SomeFunctionToGetOptionalValue();
     *          if (accumulator or (tmp = SomeFunctionToGetOptionalValue())) {
     *              accumulator = accumulator.Value () + tmp;
     *          }
     *      \endcode
     *      VERSUS
     *      \code
     *          accumulator.AccumulateIf (SomeFunctionToGetOptionalValue ());
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          optional<Sequence<InternetAddress>>>   addresses;
     *          Memory::AccumulateIf (&addresses, IO::Network::InternetAddress{connection.GET ().GetDataTextInputStream ().ReadAll ().Trim ()});
     *      \endcode
     *
     *
     *      \note   ITS CONFUSING direction of if-test for this versus CopyToIf
     *
     *      \note
     *          typical OP arguments would be:
     *              std::plus{} **the default**
     *              std::minus{}
     *              std::multiplies{}
     *              std::divides{}
     */
    template <typename T, typename CONVERTIBLE_TO_T, typename OP = plus<T>, enable_if_t<is_convertible_v<CONVERTIBLE_TO_T, T> and is_convertible_v<OP, function<T (T, T)>>>* = nullptr>
    void AccumulateIf (optional<T>* lhsOptionalValue, const optional<CONVERTIBLE_TO_T>& rhsOptionalValue, const OP& op = OP{});
    template <typename T, typename OP = plus<T>, enable_if_t<is_convertible_v<OP, function<T (T, T)>>>* = nullptr>
    void AccumulateIf (optional<T>* lhsOptionalValue, const T& rhsValue, const OP& op = OP{});
    template <typename T, template <typename> typename CONTAINER, enable_if_t<is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>>* = nullptr>
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const optional<T>& rhsOptionalValue);
    template <typename T, template <typename> typename CONTAINER, enable_if_t<is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>>* = nullptr>
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const T& rhsValue);

    /**
     *  Assign the value held by this optional if one is present to destination argument (pointer). Assigns from left to right.
     *
     *  The point of this to to faciltate a common idiom, where you want to maintain an existing value unless you
     *  get an update. This function is ANALAGOUS to
     *      if (o.has_value()) {
     *          destArgVal = *o;
     *      }
     *
     *  but can be done in a single line.
     *
     *  \par Example Usage
     *      \code
     *      int curValue = 3;
     *      Optional<long>  oVal = someMap.Lookup (KEY_VALUE);
     *      oVal.CopyToIf (&curValue);
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *      Optional<int> curValue;
     *      Optional<long>  oVal = someMap.Lookup (KEY_VALUE);
     *      oVal.CopyToIf (&curValue);      // curValue retains its value from before CopyToIf if oVal was missing
     *      \endcode
     *
     *  @see Value
     *
     *  \note - the overload with the second arg optional<CONVERTABLE_TO_OPTIONAL_OF_TYPE> is just to silence an msvc compiler warning.
     */
    template <typename T, typename CONVERTABLE_TO_TYPE>
    void CopyToIf (const optional<T>& lhs, CONVERTABLE_TO_TYPE* to);
    template <typename T, typename CONVERTABLE_TO_OPTIONAL_OF_TYPE>
    void CopyToIf (const optional<T>& lhs, optional<CONVERTABLE_TO_OPTIONAL_OF_TYPE>* to);
    template <typename T, typename CONVERTABLE_TO_TYPE>
    void CopyToIf (const Optional<T>& lhs, CONVERTABLE_TO_TYPE* to);

    /**
     *  Always safe to call. If has_value returns it, else returns argument 'default'. Like value_or () but has default value for default value.
     *
     *  \note This is handy because there is no default argument for std::optional<>::value_or () - there should be (like this).
     */
    template <typename T>
    T ValueOrDefault (const optional<T>& o, T defaultValue = T{});
    template <typename T>
    T ValueOrDefault (const Optional<T>& o, T defaultValue = T{});

    /**
     * \brief return one of *this, or o, with first preference for which is engaged, and second preference for left-to-right.
     *
     *  So Equivilent to this->has_value ()? *this : o;
     *
     *  This is VERY similar to ValueOrDefault () - except that the default maybe optional, and this therefore returns an Optional<T>
     *
     *  @see Value ()
     */
    template <typename T>
    optional<T> OptionalValue (const optional<T>& l, const optional<T>& r);

    /**
     *  Simple overloaded operator which calls @Optional<T>::Compare (const Optional<T>& rhs)
     */
    template <typename T, typename TRAITS>
    bool operator< (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
    template <typename T, typename TRAITS, typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
    bool operator< (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::Compare (const Optional<T>& rhs)
     */
    template <typename T, typename TRAITS>
    bool operator<= (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
    template <typename T, typename TRAITS, typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
    bool operator<= (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::Equals (const Optional<T>& rhs)
     */
    template <typename T, typename TRAITS>
    bool operator== (const Optional<T, TRAITS>& lhs, T rhs);
    template <typename T, typename TRAITS>
    bool operator== (T lhs, const Optional<T, TRAITS>& rhs);
    template <typename T, typename TRAITS>
    bool operator== (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
    template <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK = enable_if_t<is_constructible_v<T, RHS_CONVERTABLE_TO_OPTIONAL>>>
    bool operator== (const Optional<T, TRAITS>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::Equals (const Optional<T>& rhs)
     */
    template <typename T, typename TRAITS>
    bool operator!= (const Optional<T, TRAITS>& lhs, T rhs);
    template <typename T, typename TRAITS>
    bool operator!= (T lhs, const Optional<T, TRAITS>& rhs);
    template <typename T, typename TRAITS>
    bool operator!= (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
    template <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK = enable_if_t<is_constructible_v<Optional<T, TRAITS>, RHS_CONVERTABLE_TO_OPTIONAL>>>
    bool operator!= (const Optional<T, TRAITS>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::Compare (const Optional<T>& rhs)
     */
    template <typename T, typename TRAITS>
    bool operator>= (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
    template <typename T, typename TRAITS, typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
    bool operator>= (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::Compare (const Optional<T>& rhs)
     */
    template <typename T, typename TRAITS>
    bool operator> (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
    template <typename T, typename TRAITS, typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
    bool operator> (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator+= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T, typename TRAITS>
    Optional<T, TRAITS> operator+ (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
    template <typename T>
    optional<T> operator+ (const optional<T>& lhs, const optional<T>& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator-= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T, typename TRAITS>
    Optional<T, TRAITS> operator- (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
    template <typename T>
    optional<T> operator- (const optional<T>& lhs, const optional<T>& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator*= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T, typename TRAITS>
    Optional<T, TRAITS> operator* (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
    template <typename T>
    optional<T> operator* (const optional<T>& lhs, const optional<T>& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator/= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T, typename TRAITS>
    Optional<T, TRAITS> operator/ (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
    template <typename T>
    optional<T> operator/ (const optional<T>& lhs, const optional<T>& rhs);

    /**
     *  \brief Optional_Indirect_Storage<T> is a type alias for Optional<> but using Optional_Traits_Blockallocated_Indirect_Storage for use in forward declared (no size known) types
     */
    template <typename T>
    using Optional_Indirect_Storage = Optional<T, Optional_Traits_Blockallocated_Indirect_Storage<T>>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Optional.inl"

#endif /*_Stroika_Foundation_Memory_Optional_h_*/
