/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_h_
#define _Stroika_Foundation_Containers_Bag_h_   1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterable.h"
#include    "Iterator.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *
 *      @todo   Do CTOR () that takes ITERATOR<T> - but not til after next release....
 *
 *      @todo   DOCUMENT - and use some library for OCNPETS (ElementsTraits.h).
 *
 *      @todo   Have Bag_Difference/Union/Interesection??? methods/?? Do research....
 *
 *      @todo   Add more efficent-for-tally implementation of bag (like multimap?). Low priority since you can
 *              always use a Tally<T>...
 *
 *      @todo   Consider adding RetainAll (Set<T>) API - like in Collection.h, and Java. Key diff is was force
 *              use of SET as arg - not another Bag? Or maybe overload with different container types as args?
 *              COULD do 2 versions - one with Iterable<T> and one with Set<T>. trick is to get definition
 *              to work without untoward dependencies between set and bag code? I think that means
 *              most of the check impl needs to be in the envelope to avoid always building it through vtables.
 *
 *      @todo   Consider adding smarter implementation with keys - some btree indexing impl - to make stuff that
 *              looks up by value quicker.
 *
 *      @todo   Add Shake() method, which MAY randomize the ordering of items. Note - since ordering is not
 *              defined, this may do nothing, but will often randomize order. Often handy as a testing tool.
 *
 *      @todo   Move Bag<T>::Equals() to REP code so can be done more efficiently. Makes sense since operator==
 *              on T already required! Often can be done quite cheaply
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *  \brief  A Bag<T> is a container to manage an un-ordered collection of items.
             *
             *  A Bag<T> is a container pattern to manage an un-ordered collection of items.
             *  This is both an abstract interface, but the Bag<T> class it actually concrete because
             *  it automatically binds to a default implementation.
             *
             *  A Bag<T> is the simplest kind of collection. It allows addition and
             *  removal of elements, but makes no guarantees about element ordering. Two
             *  bags are considered equal if they contain the same items, even if iteration
             *  order is different.
             *
             *  Bags are typically designed to optimize item addition and iteration.
             *  They are fairly slow at item access (as they have no keys). Removing items
             *  is usually slow, except in the context of an Iterator<T>, where it is usually
             *  very fast. Bag comparison (operator==) is often very slow in the worst
             *  case (n^2) and this worst case is the relatively common case of identical
             *  bags.
             *
             *  Although Bag has an TallyOf () method, it is nonvirtual, and therefore
             *  not optimized for the various backends. There is a separate class, Tally,
             *  for cases where you are primarily interested in keeping an summary count
             *  of the occurences of each item.
             *
             *  Bags allow calls to Remove with an item not contained within the bag.
             *
             *  As syntactic sugar, using either functional (Add, Remove) or
             *  operator (+,-) is allowed.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            template    <typename T>
            class   Bag : public Iterable<T> {
            private:
                typedef Iterable<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                Bag ();
                Bag (const Bag<T>& b);
                template <typename CONTAINER_OF_T>
                explicit Bag (const CONTAINER_OF_T& b);
                template <typename COPY_FROM_ITERATOR>
                explicit Bag (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

            protected:
                explicit Bag (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Bag<T>& operator= (const Bag<T>& src);

            public:
                /**
                 * \brief Compares items with operator==, and returns true if any match.
                 */
                nonvirtual  bool    Contains (T item) const;

            public:
                /**
                 * Add the given item(s) to this Bag<T>. Note - if the given items are already present, another
                 * copy will be added.
                 */
                nonvirtual  void    Add (T item);

            public:
                /**
                 */
                template    <typename COPY_FROM_ITERATOR>
                nonvirtual  void    AddAll (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    AddAll (const CONTAINER_OF_T& s);

            public:
                /**
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is updated - replaced with the value 'newValue'.
                 */
                nonvirtual  void    Update (const Iterator<T>& i, T newValue);

            public:
                /**
                 * It is legal to remove something that is not there. This function removes the first instance of item
                 * (or each item for the 'items' overload), meaning that another instance of item could still be in the
                 * Bag<T> after teh remove. Thus function just reduces the Tally() by one (or zero if item wasn't found).
                 *
                 * SECOND OVERLOAD:
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is removed.
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Iterator<T>& i);

            public:
                /**
                 * It is legal to remove something that is not there. This function removes the first instance of item
                 * (or each item for the 'items' overload), meaning that another instance of item could still be in the
                 * Bag<T> after teh remove. Thus function just reduces the Tally() by one (or zero if item wasn't found).
                 *
                 *  The no-argument verison Produces an empty bag.
                 */
                nonvirtual  void    RemoveAll ();
                template    <typename COPY_FROM_ITERATOR>
                nonvirtual  void    RemoveAll (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    RemoveAll (const CONTAINER_OF_T& c);

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();


            public:
                /**
                 * NYI - but this can be remove duplicates. So you can say
                 *      for (auto i : bag) {} OR
                 *      for (auto i : bag.UniqueElements ()) {}
                 */
                nonvirtual  Iterable<T>   UniqueElements () const;

            public:
                /*
                 *  Two Bags are considered equal if they contain the same elements (by comparing them with operator==) with the same count.
                 *  In short, they are equal if TallyOf() each item in the LHS equals the TallyOf() the same item in the RHS.
                 *
                 *  Equals is commutative().
                 *
                 *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
                 */
                nonvirtual  bool    Equals (const Bag<T>& rhs) const;

            public:
                /**
                 * Since items can appear more than once, this function traverses the bag and returns the
                 * count of times the given item appears.
                 */
                nonvirtual  size_t  TallyOf (T item) const;

            public:
                /**
                 *  operator+ is syntactic sugar on Add() or AddAll() - depending on the overload.
                 *
                 *  *DEVELOPER NOTE*
                 *      Note - we use an overload
                 *      of Bag<T> for the container case instead of a template, because I'm not sure how to use specializations
                 *      to distinguish the two cases. If I can figure that out, this can transparently be
                 *      replaced with operator+= (X), with appropriate specializations.
                 */
                nonvirtual  Bag<T>& operator+= (T item);
                nonvirtual  Bag<T>& operator+= (const Bag<T>& items);

            public:
                /**
                 *  operator- is syntactic sugar on Remove() or RemoveAll() - depending on the overload.
                 *
                 *  *DEVELOPER NOTE*
                 *      Note - we use an overload
                 *      of Bag<T> for the container case instead of a template, because I'm not sure how to use specializations
                 *      to distinguish the two cases. If I can figure that out, this can transparently be
                 *      replaced with operator+= (X), with appropriate specializations.
                 */
                nonvirtual  Bag<T>& operator-= (T item);
                nonvirtual  Bag<T>& operator-= (const Bag<T>& items);

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Bag<T>& rhs) const;

            public:
                /**
                 *      Syntactic sugar on not Equals()
                 */
                nonvirtual  bool    operator!= (const Bag<T>& rhs) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Bag<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Bag<T> container API.
             */
            template    <typename T>
            class   Bag<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual bool    Contains (T item) const                     =   0;
                virtual void    Add (T item)                                =   0;
                virtual void    Update (const Iterator<T>& i, T newValue)   =   0;
                virtual void    Remove (T item)                             =   0;
                virtual void    Remove (const Iterator<T>& i)               =   0;
                virtual void    RemoveAll ()                                =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Bag.inl"

#endif  /*_Stroika_Foundation_Containers_Bag_h_ */
