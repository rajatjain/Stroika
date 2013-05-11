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
 * TODO:
 *
 *      @todo   Do CTOR () that takes ITERATOR<T> - but not til after next release....
 *
 *      @todo   DOCUMENT - and use some library for OCNPETS (ElementsTraits.h).
 *
 *      @todo   Have Bag_Difference/Union/Interesection??? methods/?? Do research....
 *
 *      @todo   Because of the definition of operator== (Bag,Bag), we have an expensive implemenation.
 *              The underlying IRep should be enhanced to allow if we are talking to two like implementations
 *              we can produce a more efficient comparison.
 *
 *      @todo   Bag<T>::Add (T item) - debug why this->... in assert...
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
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


#if     qCompilerAndStdLib_TemplateFriendFunctionsRequirePredeclaredTemplateFunction
            template    <typename T>
            class   Bag;
            template    <typename T>
            bool    operator== (const Bag<T>& lhs, const Bag<T>& rhs);
#endif


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
                Bag (const Bag<T>& bag);
                explicit Bag (const T* start, const T* end);

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
                 * \brief Produces an empty bag.
                 */
                nonvirtual  void    RemoveAll ();

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();


            public:
                /**
                 * Add the given item(s) to this Bag<T>. Note - if the given items are already present, another
                 * copy will be added.
                 */
                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (const Bag<T>& items);
                nonvirtual  void    Add (const T* begin, const T* end);

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
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Bag<T>& items);

            public:
                /**
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is removed.
                 */
                nonvirtual  void    Remove (const Iterator<T>& i);

            public:
                /**
                 * NYI - but this can be remove duplicates. So you can say
                 *      for (auto i : bag) {} OR
                 *      for (auto i : bag.UniqueElements ()) {}
                 */
                nonvirtual  Iterable<T>   UniqueElements () const;

            public:
                /**
                 *      +=/-= are equivilent Add() and Remove(). They are just syntactic sugar.
                 */
                nonvirtual  Bag<T>& operator+= (T item);
                nonvirtual  Bag<T>& operator+= (const Bag<T>& items);
                nonvirtual  Bag<T>& operator-= (T item);
                nonvirtual  Bag<T>& operator-= (const Bag<T>& items);

            public:
                /**
                 * Since items can appear more than once, this function traverses the bag and returns the
                 * count of times the given item appears.
                 */
                nonvirtual  size_t  TallyOf (T item) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();

            public:
                nonvirtual  bool    operator== (const Bag<T>& rhs) const;
                nonvirtual  bool    operator!= (const Bag<T>& rhs) const;
            };


            template    <typename T>
            Bag<T>  operator+ (const Bag<T>& lhs, const Bag<T>& rhs);
            template    <typename T>
            Bag<T>  operator- (const Bag<T>& lhs, const Bag<T>& rhs);


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
