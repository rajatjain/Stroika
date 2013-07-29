/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Tally_h_
#define _Stroika_Foundation_Containers_Tally_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Redo code so operator+= () is an cosmetic alias and AddAll() is primary.
 *
 *      @todo   Consider if MakeBagIterator/bagbegin/bagend should  be replaced with
 *              As<Bag<T>>(), and then As<Bag<T>>().begin ()? Or some such?
 *              OR
 *              just look at the Elements() method. That maybe good enough? Maybe take param to say includes dups?
 *
 *      @todo   Maybe get rid of TallyEntry<T> and instead repalce with pair<T,count>. If not
 *              then change Mapping to use a named type like  this...
 *
 *      @todo   Need Tally_stdmap<T> implementation (re-use SortedTally_stdmap they way we did
 *              for mapping).
 *
 *      @todo   Consider rewriting all Tally<> concrete types using Mapping<T,counttype> concrete impl?
 *              Probably wont work but document why... (Add () semantics - but maybe).
 *
 *      @todo   AddAll() and CTOR for Tally (and SortedTally and concrete types) is confused by having
 *              overload taking T* and TallyEntry<T>*. Issue is that we cannot do templated iterator
 *              and templated objhect CTOR while these are iteratored (without mcuh better partial
 *              template specializaiton - I THINK????). Maybe use differnt method for one or the other
 *              to distinguish?
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
            struct   Tally_DefaultTraits {
                /**
                 */
                typedef EQUALS_COMPARER EqualsCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
            };


            template    <typename T>
            class   TallyEntry {
            public:
                TallyEntry (T item);
                TallyEntry (T item, size_t count);

                nonvirtual  bool    operator== (const TallyEntry<T>& rhs) const;
                nonvirtual  bool    operator!= (const TallyEntry<T>& rhs) const;

                T       fItem;
                size_t  fCount;
            };


            /**
             *  A Tally<T, TRAITS> a collection of T elements, but where each time you add something, the tally
             *  tracks the number of times that thing has been entered. This is not a commonly used class,
             *  but handy when you want to count things.
             *
             *  Tally<T, TRAITS> inherits from Iterable<TallyEntry<T>> instead of Iterable<T> because if you are
             *  using a Tally, you probably want access to the counts as you iterate - not just the
             *  unique elements (though we make it easy to get that iterator too with MakeBagIterator()).
             *
             *  A Tally<T, TRAITS> makes no promises about ordering of elements in iteration.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T, typename TRAITS = Tally_DefaultTraits<T>>
            class  Tally : public Iterable<TallyEntry<T>> {
            private:
                typedef Iterable<TallyEntry<T>> inherited;

            public:
                /**
                 *      \brief  TallyOfElementType is just a handly copy of the *T* template type which this
                 *              Tally<T, TRAITS> parameterizes counting.
                 */
                typedef T   TallyOfElementType;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                Tally ();
                Tally (const Tally<T, TRAITS>& src);
                template    <typename CONTAINER_OF_T>
                explicit Tally (const CONTAINER_OF_T& src);
                Tally (const T* start, const T* end);
                Tally (const TallyEntry<T>* start, const TallyEntry<T>* end);

            protected:
                explicit Tally (const _SharedPtrIRep& rep);

            public:
                /**
                 *  Contains (item) is equivilent to TallyOf (item) != 0, but maybe faster (since it doesn't need to compute
                 *  the fully tally).
                 */
                nonvirtual  bool    Contains (T item) const;

            public:
                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (T item, size_t count);
                nonvirtual  void    Add (const TallyEntry<T>& item);

            public:
                nonvirtual  void    AddAll (const T* begin, const T* end);
                nonvirtual  void    AddAll (const TallyEntry<T>* start, const TallyEntry<T>* end);
                template <typename CONTAINER_OF_T>
                nonvirtual  void    AddAll (const CONTAINER_OF_T& src);

            public:
                /**
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is removed.
                 *
                 *  If using the item/count or just item overloads, then Tally<> requires that the removed items are present.
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (T item, size_t count);
                nonvirtual  void    Remove (const Iterator<TallyEntry<T>>& i);

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();
                nonvirtual  void    RemoveAll (T item);

            public:
                // if newCount == 0, equivilent to Remove(i). Require not i.Done () - so it must point to a given item.
                nonvirtual  void    UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount);

            public:
                /*
                 *  TallyOf() returns the number of occurences of 'item' in the tally. The items are compared with operator==.
                 *
                 *  If there are no copies of item in the Tally, 0 is returned.
                 */
                nonvirtual  size_t  TallyOf (T item) const;

            public:
                nonvirtual  size_t  TotalTally () const;

            public:
                nonvirtual  Tally<T, TRAITS>&   operator+= (T item);
                nonvirtual  Tally<T, TRAITS>&   operator+= (const Tally<T, TRAITS>& t);

            public:
                /**
                 * NYI - but this can be remove duplicates. So you can say
                 *      for (auto ti : tally) {} OR
                 *      for (auto i : tally.Elements ()) {}
                 *
                 *  Maybe use that to replace MakeBagIterator (and bagbegin etc)...
                 */
                nonvirtual  Iterable<T>   Elements () const;

            public:
                /**
                 * Return an iterator over individual items in the tally - as if the Tally was a Bag,
                 * not a Tally.
                 */
                nonvirtual  Iterator<T> MakeBagIterator () const;
                nonvirtual  Iterator<T> bagbegin () const;
                nonvirtual  Iterator<T> bagend () const;

            public:
                /*
                 *  Two Tally are considered equal if they contain the same elements (by comparing them with operator==) with the same count.
                 *  In short, they are equal if TallyOf() each item in the LHS equals the TallyOf() the same item in the RHS.
                 *
                 *  Equals is commutative().
                 *
                 *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
                 */
                nonvirtual  bool    Equals (const Tally<T, TRAITS>& rhs) const;

            public:
                nonvirtual  bool    operator== (const Tally<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator!= (const Tally<T, TRAITS>& rhs) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Bag<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                typedef TRAITS  TraitsType;

            public:
                /**
                 *  Just a short-hand for the EqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                typedef typename TraitsType::EqualsCompareFunctionType  EqualsCompareFunctionType;
            };


            /**
             */
            template    <typename T, typename TRAITS>
            class   Tally<T, TRAITS>::_IRep : public Iterable<TallyEntry<T>>::_IRep {
            private:
                typedef typename Iterable<TallyEntry<T>>::_IRep  inherited;

            protected:
                _IRep ();

            public:
                virtual bool        Equals (const _IRep& rhs) const                                 =   0;
                virtual bool        Contains (T item) const                                         =   0;
                virtual size_t      GetLength () const                                              =   0;
                virtual void        RemoveAll ()                                                    =   0;
                virtual void        Add (T item, size_t count)                                      =   0;
                virtual void        Remove (T item, size_t count)                                   =   0;
                virtual void        Remove (const Iterator<TallyEntry<T>>& i)                       =   0;
                virtual void        UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount) =   0;
                virtual size_t      TallyOf (T item) const                                          =   0;
                virtual Iterator<T> MakeBagIterator () const                                        =   0;

                /*
                 *  Reference Implementations (often not used except for ensure's, but can be used for
                 *  quickie backends).
                 *
                 *  Importantly, these are all non-virtual so not actually pulled in or even compiled unless
                 *  the sucblass refers to the method in a subclass virtual override.
                 */
            protected:
                nonvirtual bool    _Equals_Reference_Implementation (const _IRep& rhs) const;

            protected:
                class   _TallyEntryToItemIteratorHelperRep;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Tally.inl"

#endif  /*_Stroika_Foundation_Containers_Tally_h_ */
