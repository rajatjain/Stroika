/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_IterableFromIterator_h_
#define _Stroika_Foundation_Traversal_IterableFromIterator_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterator.h"



/**
 *  \file
 *
 *  TODO:
 *  @todo   Traversal::mkIterable ()
 *
 *          Consider having Iterator<T> have begin(), end() methods that do magic so
 *          you can also directly use an intertor in
 *              for (auto i : soemthingThatReturnsIterator()) {
 *              }
 *          I think easy and useful, and biggest concern is the potential for subtle
 *          overload confusion generation.
 *
 *          But PROBABLY NOT!!! - That is what Iterable is for. Instead - just have Traversal::mkIterable(Iterator<T>) - and
 *          then that iterable will be a trivail, short-lived private impl iterable that just indirects
 *          all iteration calls to that iterator!
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *  Helper class to make it a little easier to wrap an Iterable<> around an Iterator class.
             *
             *  EXAMPLE:
             *
             *  INCOMPLETE!!!
             *      USE GENERATOR FOR EXAMPLE - CUZ IT MAKES EASY TODO ITERATOR.
             *
             *      template    <typename T>
             *      class   MyIterableTest : public Iterable<T> {
             *      public:
             *          typedef ACTUAL_ITERATOR_REP   MyIteratorRep_;
             *          struct MyIterableRep_ : IterableFromIterator<T, MyIteratorRep_>::_Rep {
             *              using   inherited = typename IterableFromIterator<T, MyIteratorRep_>::_Rep;
             *              DECLARE_USE_BLOCK_ALLOCATION(MyIterableRep_);
             *              MyIterableRep_ ()
             *                  : inherited ()
             *              {
             *              }
             *              virtual typename Iterable<T>::_SharedPtrIRep Clone () const override
             *              {
             *                  return typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (*this));
             *              }
             *          };
             *      public:
             *          MyIterableTest ()
             *              : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ ()))
             *          {
             *          }
             *      };
             *
             *
             *      Note _Rep is an abstract class, and you MUST provide your own Clone () method, and often will
             *  want to override to provide a more efficeint IsEmpty () and GetLength () implementation.
             *
             */
            template    <typename T, typename NEW_ITERATOR_REP_TYPE = void, typename DATA_BLOB = void>
            class   IterableFromIterator : public Iterable<T> {
            public:
                class   _Rep : public Iterable<T>::_IRep {
                protected:
                    DATA_BLOB   _fDataBlob;
                protected:
                    _Rep (const DATA_BLOB& dataBLOB);
                public:
                    virtual Iterator<T>     MakeIterator () const override;
                    virtual size_t          GetLength () const override;
                    virtual bool            IsEmpty () const override;
                    virtual void            Apply (typename _Rep::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>     ApplyUntilTrue (typename _Rep::_APPLYUNTIL_ARGTYPE doToElement) const override;
                };
            };
            template    <typename T, typename NEW_ITERATOR_REP_TYPE>
            class   IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, void> : public Iterable<T> {
            public:
                class   _Rep : public Iterable<T>::_IRep {
                public:
                    virtual Iterator<T>     MakeIterator () const override;
                    virtual size_t          GetLength () const override;
                    virtual bool            IsEmpty () const override;
                    virtual void            Apply (typename _Rep::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>     ApplyUntilTrue (typename _Rep::_APPLYUNTIL_ARGTYPE doToElement) const override;
                };
            };
            template    <typename T>
            class   IterableFromIterator<T, void, void> : public Iterable<T> {
            public:
                class   _Rep : public Iterable<T>::_IRep {
                public:
                    virtual size_t          GetLength () const override;
                    virtual bool            IsEmpty () const override;
                    virtual void            Apply (typename _Rep::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>     ApplyUntilTrue (typename _Rep::_APPLYUNTIL_ARGTYPE doToElement) const override;
                };
            };


            /**
             *  This makes a copy of the given iterator, and wraps it in an iterable. That iterable then makes
             *  additional copies of that (argument) iterator as needed.
             *
             *  \em Important Note
             *      The original Iterator<T> will have lifetime == the constructed Iterable<> - so be careful - that
             *      the thing the Iterator<T> came from has long enough lifetime!
             */
            template    <typename   T>
            Iterable<T> MakeIterableFromIterator (const Iterator<T>& iterator);


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "IterableFromIterator.inl"

#endif  /*_Stroika_Foundation_Traversal_IterableFromIterator_h_ */
