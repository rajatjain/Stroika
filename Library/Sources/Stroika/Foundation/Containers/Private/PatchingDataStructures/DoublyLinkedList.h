/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Memory/SmallStackBuffer.h"
#include    "../../../Traversal/Iterator.h"

#include    "../DataStructures/DoublyLinkedList.h"

#include    "PatchableContainerHelper.h"



/*
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {


                    using   Traversal::IteratorOwnerID;


                    /*
                     *  Patching Support:
                     *
                     *      Here we provide Patching Versions of each iterator, and for convienience
                     *  versions of DoublyLinkedList that maintain a list of all Patching iterators.
                     *
                     *      PatchingDataStructures::DoublyLinkedList<T, TRAITS> is a DoublyLinkedList<T, TRAITS> with the ability to keep track of
                     *  owned patching iterators. These patching iterators will automatically be
                     *  adjusted when the link list is adjusted. This is the class of DoublyLinkedList
                     *  most likely to be used in implementing a concrete container class.
                     */
                    template      <typename  T, typename TRAITS = DataStructures::DoublyLinkedList_DefaultTraits<T>>
                    class   DoublyLinkedList : public PatchableContainerHelper<DataStructures::DoublyLinkedList<T, TRAITS>> {
                    private:
                        using   inherited   =   PatchableContainerHelper<DataStructures::DoublyLinkedList<T, TRAITS>>;

                    public:
                        DoublyLinkedList ();
                        DoublyLinkedList (const DoublyLinkedList<T, TRAITS>& from);

                    public:
                        nonvirtual  DoublyLinkedList<T, TRAITS>& operator= (const DoublyLinkedList<T, TRAITS>& rhs);

                    public:
                        using   Link    =   typename DataStructures::DoublyLinkedList<T, TRAITS>::Link;

                    public:
                        class   ForwardIterator;

                    public:
                        using   UnpatchedForwardIterator = typename inherited::ForwardIterator;

                        /*
                         * Methods we shadow so that patching is done. If you want to circumvent the
                         * patching, thats fine - use scope resolution operator to call this's base
                         * class version.
                         */
                    public:
                        nonvirtual  void    Prepend (T item);
                        nonvirtual  void    Remove (T item);
                        nonvirtual  void    RemoveFirst ();
                        nonvirtual  void    RemoveAll ();

                    public:
                        nonvirtual  void    Append (T item);

                    public:
                        nonvirtual  void    RemoveAt (const ForwardIterator& i);
                        nonvirtual  void    AddBefore (const ForwardIterator& i, T newValue);
                        nonvirtual  void    AddAfter (const ForwardIterator& i, T newValue);

                        /*
                         * Methods to do the patching yourself. Iterate over all the iterators and
                         * perfrom patching.
                         */
                    public:
                        nonvirtual  void    PatchViewsAdd (const Link* link) const;         //  call after add
                        nonvirtual  void    PatchViewsRemove (const Link* link) const;      //  call before remove
                        nonvirtual  void    PatchViewsRemoveAll () const;                   //  call after removeall

                    public:
                        nonvirtual  void    TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch) const;
                        nonvirtual  void    TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, Link* newI) const;

                    public:
                        /*
                         *  Check Invariants for this class, and all the iterators we own.
                         */
                        nonvirtual  void    Invariant () const;

#if     qDebug
                    protected:
                        virtual     void    Invariant_ () const override;
                        nonvirtual  void    InvariantOnIterators_ () const;
#endif

                    protected:
                        friend  class   ForwardIterator;
                    };


                    /*
                     *      ForwardIterator is a .... that allows
                     *  for updates to the DoublyLinkedList<T, TRAITS> to be dealt with properly. It maintains a
                     *  link list of iterators headed by the DoublyLinkedList<T, TRAITS>, and takes care
                     *  of all patching details.
                     */
                    template      <typename  T, typename TRAITS>
                    class   DoublyLinkedList<T, TRAITS>::ForwardIterator : public DataStructures::DoublyLinkedList<T, TRAITS>::ForwardIterator, public PatchableContainerHelper<DataStructures::DoublyLinkedList<T, TRAITS>>::PatchableIteratorMinIn {
                    private:
                        using   inherited_DataStructure =   typename DataStructures::DoublyLinkedList<T, TRAITS>::ForwardIterator;
                        using   inherited_PatchHelper   =   typename PatchableContainerHelper<DataStructures::DoublyLinkedList<T, TRAITS>>::PatchableIteratorMinIn;

                    public:
                        ForwardIterator (IteratorOwnerID ownerID, const DoublyLinkedList<T, TRAITS>* data);
                        ForwardIterator (const ForwardIterator& from);

                    public:
                        ~ForwardIterator ();

                    public:
                        nonvirtual  ForwardIterator&    operator= (const ForwardIterator& rhs);

                    public:
                        using   ContainerType   =   PatchingDataStructures::DoublyLinkedList<T, TRAITS>;
                        using   Link            =   typename ContainerType::Link;

                    public:
                        nonvirtual  void    PatchAdd (const Link* link);        //  call after add
                        nonvirtual  void    PatchRemove (const Link* link);     //  call before remove
                        nonvirtual  void    PatchRemoveAll ();                  //  call after removeall

                        nonvirtual  void    TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch);
                        nonvirtual  void    TwoPhaseIteratorPatcherPass2 (Link* newI);

#if     qDebug
                    protected:
                        virtual void    Invariant_ () const override;
#endif

                    private:
                        friend  class   DoublyLinkedList<T, TRAITS>;
                    };


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DoublyLinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_ */
