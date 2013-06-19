/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Tally_h_
#define _Stroika_Foundation_Containers_Tally_h_ 1


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterator.h"
#include    "Iterable.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Use TRAITS mechanism - like with Bag<>
 *
 *      @todo   Consider if MakeBagIterator/bagbegin/bagend should  be replaced with
 *              As<Bag<T>>(), and then As<Bag<T>>().begin ()? Or some such?
 *
 *      @todo   Maybe get rid of TallyEntry<T> and instead repalce with pair<T,count>. If not
 *              then change Mapping to use a named type like  this...
 *
 *      @todo   Need Tally_Tree<T> implementation - where we use btree to keep tally's sorted,
 *              so faster lookup. PROBLEM with that impl is it requires an ordering on T, which the others dont
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T>
            class   TallyEntry {
            public:
#if qIteratorsRequireNoArgContructorForT
                TallyEntry () {}
#endif
                TallyEntry (T item);
                TallyEntry (T item, size_t count);

                nonvirtual  bool    operator== (const TallyEntry<T>& rhs) const;
                nonvirtual  bool    operator!= (const TallyEntry<T>& rhs) const;

                T       fItem;
                size_t  fCount;
            };


            /**
             *  A Tally<T> a collection of <T> elements, but where each time you add something, the tally
             *  tracks the number of times that thing has been entered. This is not a commonly used class,
             *  but handy when you want to count things.
             *
             *  Tally<T> inherits from Iterable<TallyEntry<T>> instead of Iterable<T> because if you are
             *  using a Tally, you probably want access to the counts as you iterate - not just the
             *  unique elements (though we make it easy to get that iterator too with MakeBagIterator()).
             *
             *  A Tally<T> makes no promises about ordering of elements in iteration.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T>
            class  Tally : public Iterable<TallyEntry<T>> {
            public:
                RequireElementTraitsInClass(RequireOperatorEquals, T);

            private:
                typedef Iterable<TallyEntry<T>> inherited;

            public:
                /**
                 *      \brief  TallyOfElementType is just a handly copy of the *T* template type which this
                 *              Tally<T> parameterizes counting.
                 */
                typedef T   TallyOfElementType;

            protected:
                class _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                Tally ();
                Tally (const Tally<T>& src);
                Tally (const T* start, const T* end);
                Tally (const TallyEntry<T>* start, const TallyEntry<T>* end);

            protected:
                explicit Tally (const _SharedPtrIRep& rep);

            public:
                nonvirtual  bool    Contains (T item) const;

            public:
                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (T item, size_t count);
                nonvirtual  void    Add (const TallyEntry<T>& item);

            public:
                nonvirtual  void    AddAll (const T* begin, const T* end);
                nonvirtual  void    AddAll (const TallyEntry<T>* start, const TallyEntry<T>* end);

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
                nonvirtual  Tally<T>&   operator+= (T item);
                nonvirtual  Tally<T>&   operator+= (const Tally<T>& t);

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
                nonvirtual  bool    Equals (const Tally<T>& rhs) const;

            public:
                nonvirtual  bool    operator== (const Tally<T>& rhs) const;
                nonvirtual  bool    operator!= (const Tally<T>& rhs) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             */
            template    <typename T>
            class   Tally<T>::_IRep : public Iterable<TallyEntry<T>>::_IRep {
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
                class _TallyEntryToItemIteratorHelperRep;
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
