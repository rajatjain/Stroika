/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_h_
#define _Stroika_Foundation_Containers_MultiSet_h_ 1

#include    "../StroikaPreComp.h"

#include    <set>

#include    "../Common/Compare.h"
#include    "../Common/CountedValue.h"
#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Execution/Synchronized.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"
#include    "DefaultTraits/MultiSet.h"
#include    "Common.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-428 - See how to map EqualsComparer to CountedValue we use
 *              probably added template param to CountedValue - TRAITS.
 *
 *              MAYBE add    using   MultisetEntryType                       =       CountedValue<T>; to
 *              TRAITS object. Then use throughout.... Not sure this is worth parameterizing, but
 *              COULD help address unifying the EQUALS support!
 *
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   IMPORTANT - FIX TRAITS support like I did for Mapping/Set<> - Sorted...
 *              see git commit # 3c5bf0ecd686af850ff77761cf94142a33f48588
 *
 *              Key is adding MultiSetTraitsType to the traits and making generic base class
 *              for MultiSet<T> - its traits - same as wtih SortedTraits.
 *
 *              Also likewise key for MultiSet_stdmap<> - cuz now you cannot assign MultiSet_stdmap<> to
 *              MultiSet<T>!!!!
 *
 *      @todo   Fix MultiSet<> CTOR overload taking pointers (sb iterators) - overload so both cases -
 *              Iterator<T> and Iterator<CountedValue<T>>. Ise enableOf(isconvertible....)
 *
 *      @todo   Consider rewriting all MultiSet<> concrete types using Mapping<T,counttype> concrete impl?
 *              Might not work easily but document why... (Add () semantics - but maybe).
 *
 *      @todo   AddAll() and CTOR for MultiSet (and SortedMultiSet and concrete types) is confused by having
 *              overload taking T* and CountedValue<T>*. Issue is that we cannot do templated iterator
 *              and templated objhect CTOR while these are iteratored (without mcuh better partial
 *              template specializaiton - I THINK????). Maybe use different method for one or the other
 *              to distinguish?
 *
 *              USE SFINAE stuff we used in Mapping<> etc. Simplify AddAll and do the magic in Add.
 *
 *      @todo   Current DOCS for MultiSet::Remove() say that for variant T, count, the value MUST be present.
 *              But I think this is not in the spirit we've used elsewhere, due to multithreading.
 *              Better to allow them to not be present, else hard to synchonize (check and remove)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Configuration::ArgByValueType;
            using   Common::CountedValue;
            using   Traversal::Iterable;
            using   Traversal::Iterator;


            /**
             *  A MultiSet<T, TRAITS> a collection of T elements, but where each time you add something, the tally
             *  tracks the number of times that thing has been entered. This is not a commonly used class,
             *  but handy when you want to count things.
             *
             *  MultiSet<T, TRAITS> inherits from Iterable<CountedValue<T>> instead of Iterable<T> because if you are
             *  using a MultiSet, you probably want access to the counts as you iterate - not just the
             *  unique elements (though we make it easy to get that iterator too with Elements () or
             *  UniqueElements ()).
             *
             *  A MultiSet<T, TRAITS> makes no promises about ordering of elements in iteration.
             *
             *  @see    http://en.wikipedia.org/wiki/Multiset_(abstract_data_type)#Multiset
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \note   \em Aliases         Tally (Stroika 1.0), Bag (from SmallTalk-80)
             *
             *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
             */
            template    <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
            class   MultiSet : public Iterable<CountedValue<T>> {
            private:
                using   inherited   =   Iterable<CountedValue<T>>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using       ArchetypeContainerType      =   MultiSet<T, TRAITS>;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of MultiSet<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using       TraitsType                  =   TRAITS;

            public:
                /**
                 *  Just a short-hand for the EqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using       EqualsCompareFunctionType   =   typename TraitsType::EqualsCompareFunctionType;

            public:
                /**
                 *      \brief  MultiSetOfElementType is just a handly copy of the *T* template type which this
                 *              MultiSet<T, TRAITS> parameterizes counting.
                 */
                using   MultiSetOfElementType  =   T;

            public:
                /**
                 *      \brief
                 */
                using   CounterType  =   typename TraitsType::CounterType;

            protected:
                class   _IRep;

            protected:
                using   _SharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 */
                MultiSet ();
                MultiSet (const MultiSet<T, TRAITS>& src) noexcept;
                MultiSet (MultiSet<T, TRAITS>&& src) noexcept;
                MultiSet (const initializer_list<T>& src);
                MultiSet (const initializer_list<CountedValue<T>>& src);
                template    < typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if < Configuration::IsIterableOfT<CONTAINER_OF_T, T>::value and not std::is_convertible<const CONTAINER_OF_T*, const MultiSet<T, TRAITS>*>::value >::type >
                MultiSet (const CONTAINER_OF_T& src);
                MultiSet (const T* start, const T* end);
                MultiSet (const CountedValue<T>* start, const CountedValue<T>* end);

            protected:
                explicit MultiSet (const _SharedPtrIRep& rep) noexcept;
                explicit MultiSet (_SharedPtrIRep&& rep) noexcept;

#if     qDebug
            public:
                ~MultiSet ();
#endif

            public:
                nonvirtual  MultiSet&   operator= (const MultiSet& rhs) = default;
                nonvirtual  MultiSet&   operator= (MultiSet&& rhs) = default;

            public:
                /**
                 *  Contains (item) is equivalent to OccurrencesOf (item) != 0, but maybe faster (since it doesn't need to compute
                 *  the fully tally).
                 */
                nonvirtual  bool    Contains (ArgByValueType<T> item) const;

            public:
                /**
                 */
                nonvirtual  void    Add (ArgByValueType<T> item);
                nonvirtual  void    Add (ArgByValueType<T> item, CounterType count);
                nonvirtual  void    Add (const CountedValue<T>& item);

            public:
                /**
                 */
                nonvirtual  void    AddAll (const T* start, const T* end);
                nonvirtual  void    AddAll (const CountedValue<T>* start, const CountedValue<T>* end);
                template    <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if < Configuration::has_beginend<CONTAINER_OF_T>::value>::type >
                nonvirtual  void    AddAll (const CONTAINER_OF_T& src);

            public:
                /**
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is removed.
                 *
                 *  If using the item/count or just item overloads, then MultiSet<> requires that the removed items are present.
                 */
                nonvirtual  void    Remove (ArgByValueType<T> item);
                nonvirtual  void    Remove (ArgByValueType<T> item, CounterType count);
                nonvirtual  void    Remove (const Iterator<CountedValue<T>>& i);

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();
                nonvirtual  void    RemoveAll (ArgByValueType<T> item);

            public:
                /**
                 * if newCount == 0, equivalent to Remove(i). Require not i.Done () - so it must point to a given item.
                 */
                nonvirtual  void    UpdateCount (const Iterator<CountedValue<T>>& i, CounterType newCount);

            public:
                /**
                 *  It's perfectly legal for i to be missing before or after.
                 */
                nonvirtual  void    SetCount (ArgByValueType<T> i, CounterType newCount);

            public:
                /**
                 *  OccurrencesOf() returns the number of occurences of 'item' in the tally. The items are compared with operator==.
                 *
                 *  If there are no copies of item in the MultiSet, 0 is returned.
                 */
                nonvirtual  CounterType  OccurrencesOf (ArgByValueType<T> item) const;

            public:
                /**
                 *  Returns the sum of all tallys of all contained elements. This is equivalent
                 *  to Elements ().size ().
                 */
                nonvirtual  CounterType  TotalOccurrences () const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 * This is like the MultiSet was a Bag<T>. If something is in there N times,
                 *  it will show up in iteration N times. No guarnatee is made as to order of iteration.
                 *
                 *  \par Example Usage
                 *      \code
                 *      MultiSet<T> t;
                 *      for (T i : t.Elements ()) {
                 *      }
                 *      \endcode
                 *
                 *  Elements () makes no guarantess about whether or not modifications to the underlying MultiSet<> will
                 *  appear in the Elements() Iterable<T>.
                 *
                 *  @see UniqueElements
                 */
                nonvirtual  Iterable<T>   Elements () const;

            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *      MultiSet<T> t;
                 *      for (T i : t.UniqueElements ()) {
                 *      }
                 *      \endcode
                 *
                 *  UniqueElements () makes no guarantess about whether or not modifications to the underlying MultiSet<>
                 *  will appear in the UniqueElements() Iterable<T>.
                 */
                nonvirtual  Iterable<T>   UniqueElements () const;

            public:
                /*
                 *  Two MultiSet are considered equal if they contain the same elements (by comparing them with operator==)
                 *  with the same count. In short, they are equal if OccurrencesOf() each item in the LHS equals the OccurrencesOf()
                 *  the same item in the RHS.
                 *
                 *  Equals is commutative.
                 *
                 *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
                 */
                nonvirtual  bool    Equals (const MultiSet<T, TRAITS>& rhs) const;

            public:
                /**
                 *  Synonym for Add (), or AddAll() (depending on argument);
                 */
                nonvirtual  MultiSet<T, TRAITS>&   operator+= (ArgByValueType<T> item);
                nonvirtual  MultiSet<T, TRAITS>&   operator+= (const MultiSet<T, TRAITS>& t);

            protected:
                /**
                 */
                template    <typename T2>
                using   _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<T2>;

            protected:
                /**
                 */
                template    <typename T2>
                using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

            protected:
                nonvirtual  void    _AssertRepValidType () const;
            };


            using   Traversal::IteratorOwnerID;


            /**
             */
            template    <typename T, typename TRAITS>
            class   MultiSet<T, TRAITS>::_IRep
                : public Iterable<CountedValue<T>>::_IRep
#if     !qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                , public Traversal::IterableBase::enable_shared_from_this_SharedPtrImplementationTemplate<typename MultiSet<T, TRAITS>::_IRep>
#endif
            {
            private:
                using   inherited   =   typename Iterable<CountedValue<T>>::_IRep;

            protected:
                using   _SharedPtrIRep  =   typename MultiSet<T, TRAITS>::_SharedPtrIRep;

            public:
                using   CounterType =   typename MultiSet<T, TRAITS>::CounterType;

            protected:
                _IRep () = default;

            public:
                virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const                  =   0;
                virtual bool                Equals (const _IRep& rhs) const                                         =   0;
                virtual bool                Contains (ArgByValueType<T> item) const                                 =   0;
                virtual void                Add (ArgByValueType<T> item, CounterType count)                         =   0;
                virtual void                Remove (ArgByValueType<T> item, CounterType count)                      =   0;
                virtual void                Remove (const Iterator<CountedValue<T>>& i)                             =   0;
                virtual void                UpdateCount (const Iterator<CountedValue<T>>& i, CounterType newCount)  =   0;
                virtual CounterType         OccurrencesOf (ArgByValueType<T> item) const                            =   0;
                // Subtle point - shared rep argument to Elements() allows shared ref counting
                // without the cost of a clone or enable_shared_from_this
                virtual Iterable<T>         Elements (const _SharedPtrIRep& rep) const                              =   0;
                // Subtle point - shared rep argument to Elements() allows shared ref counting
                // without the cost of a clone or enable_shared_from_this
                virtual Iterable<T>         UniqueElements (const _SharedPtrIRep& rep) const                        =   0;
#if     qDebug
                virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const   =   0;
#endif

                /*
                 *  Reference Implementations (often not used except for ensure's, but can be used for
                 *  quickie backends).
                 *
                 *  Importantly, these are all non-virtual so not actually pulled in or even compiled unless
                 *  the sucblass refers to the method in a subclass virtual override.
                 */
            protected:
                nonvirtual bool         _Equals_Reference_Implementation (const _IRep& rhs) const;
                nonvirtual Iterable<T>  _Elements_Reference_Implementation (const _SharedPtrIRep& rep) const;
                nonvirtual Iterable<T>  _UniqueElements_Reference_Implementation (const _SharedPtrIRep& rep) const;

            private:
                struct  ElementsIteratorHelperContext_;
                struct  ElementsIteratorHelper_;

            protected:
                struct  _ElementsIterableHelper;

            private:
                struct  UniqueElementsIteratorHelperContext_;
                struct  UniqueElementsIteratorHelper_;

            protected:
                struct  _UniqueElementsHelper;
            };


            /**
             *  Synonym for Equals() (or !Equals());
             */
            template    <typename T, typename TRAITS>
            bool    operator== (const MultiSet<T, TRAITS>& lhs, const MultiSet<T, TRAITS>& rhs);
            template    <typename T, typename TRAITS>
            bool    operator!= (const MultiSet<T, TRAITS>& lhs, const MultiSet<T, TRAITS>& rhs);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "MultiSet.inl"

#endif  /*_Stroika_Foundation_Containers_MultiSet_h_ */
