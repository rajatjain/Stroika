/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Memory/SmallStackBuffer.h"
#include    "../../../Traversal/Iterator.h"

#include    "../../ExternallySynchronizedDataStructures/DoublyLinkedList.h"

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
                     *      This class wraps a basic container (in this case ExternallySynchronizedDataStructures::DoublyLinkedList)
                     *  and adds in patching support (tracking a list of iterators - and managing thier
                     *  patching when appropriately wrapped changes are made to the data structure container.
                     *
                     *      This code leverages PatchableContainerHelper<> to do alot of the book-keeping.
                     *
                     *      Note: Disallow X(const X&), and operator=() (copy constructor/assignement operator), and
                     *  instead require use of X(X*,IteratorOwnerID) for copying - so we always get both values -
                     *  the source to copy from and the newOwnerID to copy INTO.
                     */
                    template      <typename  T, typename LOCKER, typename TRAITS = ExternallySynchronizedDataStructures::DoublyLinkedList_DefaultTraits<T>>
                    class   DoublyLinkedList : public PatchableContainerHelper<ExternallySynchronizedDataStructures::DoublyLinkedList<T, TRAITS>, LOCKER> {
                    private:
                        using   inherited   =   PatchableContainerHelper<ExternallySynchronizedDataStructures::DoublyLinkedList<T, TRAITS>, LOCKER>;

                    public:
                        DoublyLinkedList ();
                        DoublyLinkedList (DoublyLinkedList<T, LOCKER, TRAITS>* rhs, IteratorOwnerID newOwnerID);
                        DoublyLinkedList (const DoublyLinkedList<T, LOCKER, TRAITS>& from) = delete;

                    public:
                        nonvirtual  DoublyLinkedList<T, LOCKER, TRAITS>& operator= (const DoublyLinkedList<T, LOCKER, TRAITS>& rhs) =   delete;

                    public:
                        using   Link    =   typename ExternallySynchronizedDataStructures::DoublyLinkedList<T, TRAITS>::Link;

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
                    template      <typename  T, typename LOCKER, typename TRAITS>
                    class   DoublyLinkedList<T, LOCKER, TRAITS>::ForwardIterator : public ExternallySynchronizedDataStructures::DoublyLinkedList<T, TRAITS>::ForwardIterator, public PatchableContainerHelper<ExternallySynchronizedDataStructures::DoublyLinkedList<T, TRAITS>, LOCKER>::PatchableIteratorMixIn {
                    private:
                        using   inherited_DataStructure =   typename ExternallySynchronizedDataStructures::DoublyLinkedList<T, TRAITS>::ForwardIterator;
                        using   inherited_PatchHelper   =   typename PatchableContainerHelper<ExternallySynchronizedDataStructures::DoublyLinkedList<T, TRAITS>, LOCKER>::PatchableIteratorMixIn;

                    public:
                        ForwardIterator (IteratorOwnerID ownerID, const DoublyLinkedList<T, LOCKER, TRAITS>* data);
                        ForwardIterator (const ForwardIterator& from);

                    public:
                        ~ForwardIterator ();

                    public:
                        nonvirtual  ForwardIterator&    operator= (const ForwardIterator& rhs);

                    public:
                        using   ContainerType   =   PatchingDataStructures::DoublyLinkedList<T, LOCKER, TRAITS>;
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
                        friend  class   DoublyLinkedList<T, LOCKER, TRAITS>;
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
