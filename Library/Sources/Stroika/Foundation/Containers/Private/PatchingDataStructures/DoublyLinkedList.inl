/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_inl_  1


#include    "../../../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Private {
                namespace   PatchingDataStructures {


                    /*
                    ********************************************************************************
                    ************************* DoublyLinkedList_Patch<T> ****************************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    inline  DoublyLinkedList_Patch<T>::DoublyLinkedList_Patch ()
                        : inherited ()
                        , fIterators (nullptr)
                    {
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  DoublyLinkedList_Patch<T>::DoublyLinkedList_Patch (const DoublyLinkedList_Patch<T>& from)
                        : inherited (from)
                        , fIterators (nullptr)    // Don't copy the list of iterators - would be trouble with backpointers!
                        // Could clone but that would do no good, since nobody else would have pointers to them
                    {
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  DoublyLinkedList_Patch<T>::~DoublyLinkedList_Patch ()
                    {
                        Require (fIterators == nullptr);
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList_Patch<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
                        InvariantOnIterators_ ();
#endif
                    }
                    template    <typename   T>
                    inline  DoublyLinkedList_Patch<T>& DoublyLinkedList_Patch<T>::operator= (const DoublyLinkedList_Patch<T>& rhs)
                    {
                        /*
                         * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
                         * If this is to be supported at some future date, well need to work on our patching.
                         */
                        Assert (not (HasActiveIterators ()));   // cuz copy of DoublyLinkedList does not copy iterators...
                        inherited::operator= (rhs);
                        return *this;
                    }
                    template    <typename   T>
                    inline  bool    DoublyLinkedList_Patch<T>::HasActiveIterators () const
                    {
                        return fIterators != nullptr;
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList_Patch<T>::PatchViewsAdd (const Link* link) const
                    {
                        RequireNotNull (link);
                        for (DoublyLinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            v->PatchAdd (link);
                        }
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList_Patch<T>::PatchViewsRemove (const Link* link) const
                    {
                        RequireNotNull (link);
                        for (DoublyLinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            v->PatchRemove (link);
                        }
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList_Patch<T>::PatchViewsRemoveAll () const
                    {
                        for (DoublyLinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            v->PatchRemoveAll ();
                        }
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList_Patch<T>::Prepend (T item)
                    {
                        Invariant ();
                        inherited::Prepend (item);
                        PatchViewsAdd (this->_fFirst);
                        Invariant ();
                    }
                    //tmphack - must fix for oduble linked list
                    template    <typename   T>
                    inline  void    DoublyLinkedList_Patch<T>::Append (T item)
                    {
                        if (this->IsEmpty ()) {
                            Prepend (item);
                        }
                        else {
                            Link* last = this->_fFirst;
                            for (; last->fNext != nullptr; last = last->fNext)
                                ;
                            Assert (last != nullptr);
                            Assert (last->fNext == nullptr);
                            last->fNext = new Link (item, nullptr);
                            PatchViewsAdd (last->fNext);
                        }
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList_Patch<T>::RemoveFirst ()
                    {
                        Invariant ();
                        PatchViewsRemove (this->_fFirst);
                        inherited::RemoveFirst ();
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList_Patch<T>::RemoveAll ()
                    {
                        Invariant ();
                        inherited::RemoveAll ();
                        PatchViewsRemoveAll ();
                        Invariant ();
                    }
                    template    <typename   T>
                    void    DoublyLinkedList_Patch<T>::Remove (T item)
                    {
                        Invariant ();
                        T current;
                        for (DoublyLinkedListMutator_Patch<T> it (*this); it.More (&current, true); ) {
                            if (current == item) {
                                it.RemoveCurrent ();
                                break;
                            }
                        }
                        Invariant ();
                    }
                    template    <typename   T>
                    void    DoublyLinkedList_Patch<T>::RemoveAt (const ForwardIterator& i)
                    {
                        Invariant ();
                        this->PatchViewsRemove (i._fCurrent);
                        inherited::RemoveAt (i);
                        Invariant ();
                    }
                    template    <typename   T>
                    void    DoublyLinkedList_Patch<T>::AddBefore (const ForwardIterator& i, T newValue)
                    {
                        Invariant ();
                        bool    isPrevNull = i.fPrev == nullptr;
                        inherited::AddBefore (i, newValue);
                        /// WAG - VERY LIKELY WRONG BELOIW - MUST CLENAUP - LGP -2013-06-17
                        if (isPrevNull) {
                            this->fData->PatchViewsAdd (this->_fFirst);       // Will adjust fPrev
                        }
                        else {
                            this->fData->PatchViewsAdd (i._fCurrent->fPrev->fNext);       // Will adjust fPrev
                        }
                        Invariant ();
                    }
                    template    <typename   T>
                    void    DoublyLinkedList_Patch<T>::AddAfter (const ForwardIterator& i, T newValue)
                    {
                        Invariant ();
                        inherited::AddAfter (i, newValue);
                        this->PatchViewsAdd (i_fCurrent->fNext);
                        Invariant ();
                    }
#if     qDebug
                    template    <typename   T>
                    void    DoublyLinkedList_Patch<T>::Invariant_ () const
                    {
                        inherited::Invariant_ ();
                        /*
                         *      Be sure each iterator points back to us. Thats about all we can test from
                         *  here since we cannot call each iterators Invariant(). That would be
                         *  nice, but sadly when this Invariant_ () is called from DoublyLinkedList<T> the
                         *  iterators themselves may not have been patched, so they'll be out of
                         *  date. Instead, so that in local shadow of Invariant() done in DoublyLinkedList_Patch<T>
                         *  so only called when WE call Invariant().
                         */
                        for (DoublyLinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            Assert (v->fData == this);
                        }
                    }
                    template    <typename   T>
                    void    DoublyLinkedList_Patch<T>::InvariantOnIterators_ () const
                    {
                        /*
                         *      Only here can we iterate over each iterator and calls its Invariant()
                         *  since now we've completed any needed patching.
                         */
                        for (DoublyLinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            Assert (v->fData == this);
                            v->Invariant ();
                        }
                    }
#endif


                    /*
                    ********************************************************************************
                    *********************** DoublyLinkedListIterator_Patch<T> **********************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    inline  DoublyLinkedListIterator_Patch<T>::DoublyLinkedListIterator_Patch (const DoublyLinkedList_Patch<T>& data)
                        : inherited (data)
                        , fData (&data)
                        , fNext (data.fIterators)
                        , fPrev (nullptr)         // means invalid or fData->fFirst == _fCurrent ...
                    {
                        const_cast<DoublyLinkedList_Patch<T>*> (&data)->fIterators = this;
                        this->Invariant ();
                    }
                    template    <typename   T>
                    inline  DoublyLinkedListIterator_Patch<T>::DoublyLinkedListIterator_Patch (const DoublyLinkedListIterator_Patch<T>& from)
                        : inherited (from)
                        , fData (from.fData)
                        , fNext (from.fData->fIterators)
                        , fPrev (from.fPrev)
                    {
                        from.Invariant ();
                        const_cast<DoublyLinkedList_Patch<T>*> (fData)->fIterators = this;
                        this->Invariant ();
                    }
                    template    <typename   T>
                    inline  DoublyLinkedListIterator_Patch<T>::~DoublyLinkedListIterator_Patch ()
                    {
                        this->Invariant ();
                        AssertNotNull (fData);
                        if (fData->fIterators == this) {
                            const_cast<DoublyLinkedList_Patch<T>*> (fData)->fIterators = fNext;
                        }
                        else {
                            DoublyLinkedListIterator_Patch<T>* v = fData->fIterators;
                            for (; v->fNext != this; v = v->fNext) {
                                AssertNotNull (v);
                                AssertNotNull (v->fNext);
                            }
                            AssertNotNull (v);
                            Assert (v->fNext == this);
                            v->fNext = fNext;
                        }
                    }
                    template    <typename   T>
                    inline  DoublyLinkedListIterator_Patch<T>&    DoublyLinkedListIterator_Patch<T>::operator= (const DoublyLinkedListIterator_Patch<T>& rhs)
                    {
                        this->Invariant ();

                        /*
                         *      If the fData field has not changed, then we can leave alone our iterator linkage.
                         *  Otherwise, we must remove ourselves from the old, and add ourselves to the new.
                         */
                        if (fData != rhs.fData) {
                            AssertNotNull (fData);
                            AssertNotNull (rhs.fData);

                            /*
                             * Remove from old.
                             */
                            if (fData->fIterators == this) {
                                const_cast<DoublyLinkedList_Patch<T>*>(fData)->fIterators = fNext;
                            }
                            else {
                                DoublyLinkedListIterator_Patch<T>* v = fData->fIterators;
                                for (; v->fNext != this; v = v->fNext) {
                                    AssertNotNull (v);
                                    AssertNotNull (v->fNext);
                                }
                                AssertNotNull (v);
                                Assert (v->fNext == this);
                                v->fNext = fNext;
                            }

                            /*
                             * Add to new.
                             */
                            fData = rhs.fData;
                            fNext = rhs.fData->fIterators;
                            const_cast<DoublyLinkedList_Patch<T>*> (fData)->fIterators = this;
                        }

                        fData = rhs.fData;
                        fPrev = rhs.fPrev;

                        inherited::operator= (rhs);

                        this->Invariant ();
                        return *this;
                    }
                    template    <typename   T>
                    inline  bool    DoublyLinkedListIterator_Patch<T>::More (T* current, bool advance)
                    {
                        this->Invariant ();

                        if (advance) {
                            /*
                             * We could already be done since after the last Done() call, we could
                             * have done a removeall.
                             */
                            if (not this->_fSuppressMore and this->_fCurrent != nullptr) {
                                fPrev = this->_fCurrent;
                                this->_fCurrent = this->_fCurrent->fNext;

                            }
                            this->_fSuppressMore = false;
                        }
                        this->Invariant ();
                        if ((current != nullptr) and (not this->Done ())) {
                            *current = this->_fCurrent->fItem;
                        }
                        return (not this->Done ());
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedListIterator_Patch<T>::PatchAdd (const Link* link)
                    {
                        /*
                         *      link is the new link just added. If it was just after current, then
                         *  there is no problem - we will soon hit it. If it was well before current
                         *  (ie before prev) then there is still no problem. If it is the new
                         *  previous, we just adjust our previous.
                         */
                        RequireNotNull (link);
                        if (link->fNext == this->_fCurrent) {
                            fPrev = link;
                        }
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedListIterator_Patch<T>::PatchRemove (const Link* link)
                    {
                        RequireNotNull (link);

                        /*
                         *  There are basicly three cases:
                         *
                         *  (1)     We remove the current. In this case, we just advance current to the next
                         *          item (prev is already all set), and set _fSuppressMore since we are advanced
                         *          to the next item.
                         *  (2)     We remove our previous. Technically this poses no problems, except then
                         *          our previos pointer is invalid. We could recompute it, but that would
                         *          involve rescanning the list from the beginning - slow. And we probably
                         *          will never need the next pointer (unless we get a remove current call).
                         *          So just set it to nullptr, which conventionally means no valid value.
                         *          It will be recomputed if needed.
                         *  (3)     We are deleting some other value. No probs.
                         */
                        if (this->_fCurrent == link) {
                            this->_fCurrent = this->_fCurrent->fNext;
                            // fPrev remains the same - right now it points to a bad item, since
                            // PatchRemove() called before the actual removal, but right afterwards
                            // it will point to our new _fCurrent.
                            this->_fSuppressMore = true;         // Since we advanced cursor...
                        }
                        else if (fPrev == link) {
                            fPrev = nullptr;                    // real value recomputed later, if needed
                        }
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedListIterator_Patch<T>::PatchRemoveAll ()
                    {
                        this->_fCurrent = nullptr;
                        fPrev = nullptr;
                        Ensure (this->Done ());
                    }
#if     qDebug
                    template    <typename   T>
                    void    DoublyLinkedListIterator_Patch<T>::Invariant_ () const
                    {
                        inherited::Invariant_ ();

                        /*
                         *  fPrev could be nullptr, but if it isn't then its next must be _fCurrent.
                         */
                        Assert ((fPrev == nullptr) or (fPrev->fNext == this->_fCurrent));
                    }
#endif


                    /*
                    ********************************************************************************
                    ************************ DoublyLinkedListMutator_Patch<T> **********************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    inline  DoublyLinkedListMutator_Patch<T>::DoublyLinkedListMutator_Patch (DoublyLinkedList_Patch<T>& data) :
                        inherited ((const DoublyLinkedList_Patch<T>&)data)
                    {
                    }
                    template    <typename   T>
                    inline  DoublyLinkedListMutator_Patch<T>::DoublyLinkedListMutator_Patch (const DoublyLinkedListMutator_Patch<T>& from) :
                        inherited ((const DoublyLinkedListIterator_Patch<T>&)from)
                    {
                    }
                    template    <typename   T>
                    inline  DoublyLinkedListMutator_Patch<T>& DoublyLinkedListMutator_Patch<T>::operator= (DoublyLinkedListMutator_Patch<T>& rhs)
                    {
                        inherited::operator= ((const DoublyLinkedListIterator_Patch<T>&)rhs);
                        return *this;
                    }
#if 0
                    template    <typename   T>
                    inline  void    DoublyLinkedListMutator_Patch<T>::RemoveCurrent ()
                    {
                        Require (not this->Done ());
                        this->Invariant ();
                        Link*    victim  = const_cast<Link*> (this->_fCurrent);
                        AssertNotNull (this->fData);
                        this->fData->PatchViewsRemove (victim);
                        Assert (this->_fCurrent != victim);              // patching should  have guaranteed this
                        /*
                         *      At this point we need the fPrev pointer. But it may have been lost
                         *  in a patch. If it was, its value will be nullptr (NB: nullptr could also mean
                         *  _fCurrent == fData->fFirst). If it is nullptr, recompute. Be careful if it
                         *  is still nullptr, that means update fFirst.
                         */

                        const Link*     firstDataLink = this->_GetFirstDataLink  (this->fData);
                        if ((this->fPrev == nullptr) and (firstDataLink != victim)) {
                            AssertNotNull (firstDataLink);    // cuz there must be something to remove current
                            for (this->fPrev = firstDataLink; this->fPrev->fNext != victim; this->fPrev = this->fPrev->fNext) {
                                AssertNotNull (this->fPrev);    // cuz that would mean victim not in DoublyLinkedList!!!
                            }
                        }
                        if (this->fPrev == nullptr) {
                            this->_SetFirstDataLink (const_cast<DoublyLinkedList_Patch<T>*> (this->fData), victim->fNext);
                        }
                        else {
                            Assert (this->fPrev->fNext == victim);
                            const_cast<Link*> (this->fPrev)->fNext = victim->fNext;
                        }
                        delete (victim);
                        this->Invariant ();
                        this->fData->Invariant ();  // calls by invariant
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedListMutator_Patch<T>::UpdateCurrent (T newValue)
                    {
                        RequireNotNull (this->_fCurrent);
                        const_cast<Link*> (this->_fCurrent)->fItem = newValue;
                    }
#endif
                    template    <typename   T>
                    inline  void    DoublyLinkedListMutator_Patch<T>::AddBefore (T newValue)
                    {
                        /*
                         * NB: This code works fine, even if we are done!!!
                         */

                        /*
                         *      At this point we need the fPrev pointer. But it may have been lost
                         *  in a patch. If it was, its value will be nullptr (NB: nullptr could also mean
                         *  _fCurrent == fData->fFirst). If it is nullptr, recompute. Be careful if it
                         *  is still nullptr, that means update fFirst.
                         */
                        AssertNotNull (this->fData);
                        const Link*     firstDataLink = this->_GetFirstDataLink  (this->fData);
                        if ((this->fPrev == nullptr) and (firstDataLink != nullptr) and (firstDataLink != this->_fCurrent)) {
                            for (this->fPrev = firstDataLink; this->fPrev->fNext != this->_fCurrent; this->fPrev = this->fPrev->fNext) {
                                AssertNotNull (this->fPrev);    // cuz that would mean _fCurrent not in DoublyLinkedList!!!
                            }
                        }
                        if (this->fPrev == nullptr) {
                            Assert (firstDataLink == this->_fCurrent);     // could be nullptr, or not...
                            this->_SetFirstDataLink (const_cast<DoublyLinkedList_Patch<T>*> (this->fData), new Link (newValue, const_cast<Link*> (firstDataLink)));
                            firstDataLink = this->_GetFirstDataLink  (this->fData);
                            this->fData->PatchViewsAdd (firstDataLink);       // Will adjust fPrev
                        }
                        else {
                            Assert (this->fPrev->fNext == this->_fCurrent);
                            const_cast<Link*>(this->fPrev)->fNext = new Link (newValue, this->fPrev->fNext);
                            this->fData->PatchViewsAdd (this->fPrev->fNext);        // Will adjust fPrev
                        }
                        this->fData->Invariant ();  // will call this's Invariant()
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedListMutator_Patch<T>::AddAfter (T newValue)
                    {
                        Require (not this->Done ());
                        AssertNotNull (this->_fCurrent); // since not done...
                        const_cast<Link*>(this->_fCurrent)->fNext = new Link (newValue, this->_fCurrent->fNext);
                        this->fData->PatchViewsAdd (this->_fCurrent->fNext);
                    }


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_inl_ */
