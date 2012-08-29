/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Bag_Array_inl_

#include    "../../Memory/BlockAllocated.h"

#include    "Private/Array.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Bag_Array<T>::Rep_ : public Bag<T>::_IRep {
                public:
                    Rep_ ();
                    ~Rep_ ();

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual typename Iterable<T>::_IRep*    Clone () const override;
                    virtual Iterator<T>                     MakeIterator () const override;
                    virtual size_t                          GetLength () const override;
                    virtual bool                            IsEmpty () const override;
                    virtual void                            Apply (void (*doToElement) (const T& item)) const override;
                    virtual Iterator<T>                     ApplyUntilTrue (bool (*doToElement) (const T& item)) const override;

                    // Bag<T>::_IRep overrides
                public:
                    virtual void                        Compact () override;
                    virtual bool                        Contains (T item) const override;
                    virtual void                        Add (T item) override;
                    virtual void                        Update (const Iterator<T>& i, T newValue) override;
                    virtual void                        Remove (T item) override;
                    virtual void                        Remove (const Iterator<T>& i) override;
                    virtual void                        RemoveAll () override;

                private:
                    Array_Patch<T>  fData_;
                    friend  class Bag_Array<T>::IteratorRep_;
                };


                template    <typename T>
                class  Bag_Array<T>::IteratorRep_ : public Iterator<T>::IRep {
                public:
                    IteratorRep_ (typename Bag_Array<T>::Rep_& owner);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<T>::IRep*     Clone () const override;
                    virtual bool                            More (T* current, bool advance) override;
                    virtual bool                            StrongEquals (typename Iterator<T>::IRep* rhs) override;

                private:
                    mutable ForwardArrayMutator_Patch<T>    fIterator_;

                private:
                    friend  class   Rep_;
                };





                //  class   Bag_Array<T>::IteratorRep_<T>
                template    <typename T>
                Bag_Array<T>::IteratorRep_::IteratorRep_ (typename Bag_Array<T>::Rep_& owner)
                    : Iterator<T>::IRep ()
                    , fIterator_ (owner.fData_)
                {
                }
                template    <typename T>
                bool    Bag_Array<T>::IteratorRep_::More (T* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename T>
                bool    Bag_Array<T>::IteratorRep_::StrongEquals (typename Iterator<T>::IRep* rhs) override
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename T>
                typename Iterator<T>::IRep*  Bag_Array<T>::IteratorRep_::Clone () const
                {
                    return (new IteratorRep_ (*this));
                }






                //  class   Bag_Array<T>::Rep_
                template    <typename T>
                inline  Bag_Array<T>::Rep_::Rep_ ()
                    : fData_ ()
                {
                }
                template    <typename T>
                Bag_Array<T>::Rep_::~Rep_ ()
                {
                }
                template    <typename T>
                typename Iterable<T>::_IRep*  Bag_Array<T>::Rep_::Clone () const
                {
                    return (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Bag_Array<T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    return Iterator<T> (new IteratorRep_ (*NON_CONST_THIS));
                }
                template    <typename T>
                size_t  Bag_Array<T>::Rep_::GetLength () const
                {
                    return (fData_.GetLength ());
                }
                template    <typename T>
                bool  Bag_Array<T>::Rep_::IsEmpty () const
                {
                    return (fData_.GetLength () == 0);
                }
                template    <typename T>
                void      Bag_Array<T>::Rep_::Apply (void (*doToElement) (const T& item)) const override
                {
                    return _Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Bag_Array<T>::Rep_::ApplyUntilTrue (bool (*doToElement) (const T& item)) const override
                {
                    return _ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                void    Bag_Array<T>::Rep_::Compact ()
                {
                    fData_.Compact ();
                }
                template    <typename T>
                bool    Bag_Array<T>::Rep_::Contains (T item) const
                {
                    return (fData_.Contains (item));
                }
                template    <typename T>
                void    Bag_Array<T>::Rep_::Add (T item)
                {
                    // Appending is fastest
                    fData_.InsertAt (item, fData_.GetLength ());
                }
                template    <typename T>
                void    Bag_Array<T>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Bag_Array<T>::IteratorRep_&       mir =   dynamic_cast<const typename Bag_Array<T>::IteratorRep_&> (ir);
                    mir.fIterator_.UpdateCurrent (newValue);
                }
                template    <typename T>
                void    Bag_Array<T>::Rep_::Remove (T item)
                {
                    /*
                     *  Iterate backwards since removing from the end of an array will be faster.
                     */
                    for (BackwardArrayIterator<T> it (fData_); it.More (nullptr, true);) {
                        if (it.Current () == item) {
                            fData_.RemoveAt (it.CurrentIndex ());
                            return;
                        }
                    }
                }
                template    <typename T>
                void    Bag_Array<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Bag_Array<T>::IteratorRep_&       mir =   dynamic_cast<const typename Bag_Array<T>::IteratorRep_&> (ir);
                    mir.fIterator_.RemoveCurrent ();
                }
                template    <typename T>
                void    Bag_Array<T>::Rep_::RemoveAll ()
                {
                    fData_.RemoveAll ();
                }




                //  class   Bag_Array<T>
                template    <typename T>
                Bag_Array<T>::Bag_Array ()
                    : Bag<T> (new Rep_ ())
                {
                }
                template    <typename T>
                Bag_Array<T>::Bag_Array (const Bag<T>& bag)
                    : Bag<T> (new Rep_ ())
                {
                    SetCapacity (bag.GetLength ());
                    operator+= (bag);
                }
                template    <typename T>
                Bag_Array<T>::Bag_Array (const T* start, const T* end)
                    : Bag<T> (new Rep_ ())
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    if (start != end) {
                        SetCapacity (end - start);
                        Add (start, end);
                    }
                }
                template    <typename T>
                inline  Bag_Array<T>::Bag_Array (const Bag_Array<T>& bag)
                    : Bag<T> (bag)
                {
                }
                template    <typename T>
                inline  Bag_Array<T>&   Bag_Array<T>::operator= (const Bag_Array<T>& bag)
                {
                    Bag<T>::operator= (bag);
                    return (*this);
                }
                template    <typename T>
                inline  const typename Bag_Array<T>::Rep_&  Bag_Array<T>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    return (static_cast<const Rep_&> (Bag<T>::_GetRep ()));
                }
                template    <typename T>
                inline  typename Bag_Array<T>::Rep_&    Bag_Array<T>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    return (static_cast<const Rep_&> (Bag<T>::_GetRep ()));
                }
                template    <typename T>
                inline  size_t  Bag_Array<T>::GetCapacity () const
                {
                    return (GetRep_ ().fData_.GetCapacity ());
                }
                template    <typename T>
                inline  void    Bag_Array<T>::SetCapacity (size_t slotsAlloced)
                {
                    GetRep_ ().fData_.SetCapacity (slotsAlloced);
                }

            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Bag_Array_inl_ */
