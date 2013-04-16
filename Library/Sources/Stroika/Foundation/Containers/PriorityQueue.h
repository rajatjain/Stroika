/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_PriorityQueue_h_
#define _Stroika_Foundation_Containers_PriorityQueue_h_  1

/*
 *
 *
 *  STATUS:
 *      CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika
 *
 *
 *
 *  TODO:
 *
 *      (o)         Implement first draft of code based on
 *                  http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/PriorityQueue.hh
 *
 *      (o)         Should inherit from Iterable<T>
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            typedef uint16_t    Priority;
            const   Priority    kMinPriority    =   kMinUInt16;
            const   Priority    kMaxPriority    =   kMaxUInt16;
            // Someday this should be renamed ...
            template    <typename T>
            class   PQEntry {
            public:
                PQEntry (T item, Priority p);

                T           fItem;
                Priority    fPriority;
            };
            template    <class T> Boolean   operator== (const PQEntry<T>& lhs, const PQEntry<T>& rhs);


            /*
             *  PriorityQueues are a like a Queue that allows retrieval based the priority assigned an item.
             *  This priority is given either at the time when the item is Enqueueed to the PriorityQueue, or
             *  by a function. The default function always assigns the lowest possible priority to an item.
             *  Priority start at zero and work upwards, so a zero priority item will be the last item
             *  removed from the PriorityQueue.
             *
             *  PriorityQueues support two kinds of iteration: over type T, or over ProirityQueueEntrys of
             *  type T. A PriorityQueueEntry is a simple structure that couples together the item and its
             *  priority.
             *
             *
             *  PriorityQueues always iterate from highest to lowest priority.
             */
            template    <typename T>
            class   PriorityQueue : public Iterable<pair<T, Priority>> {
            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /*
                 */
                PriorityQueue ();
                PriorityQueue (const Queue<T>& s);

            protected:
                explicit PriorityQueue (const _SharedPtrIRep& rep);

            public:
                nonvirtual  PriorityQueue<T>& operator= (const PriorityQueue<T>& src);

            public:
                nonvirtual  void    RemoveAll ();

            public:
                Iterable<T> Elements () const;

            public:
                nonvirtual  void        Enqueue (T item);
                nonvirtual  void        Enqueue (T item, Priority priority);

            public:
                nonvirtual  T           Dequeue ();

            public:
                nonvirtual  T           Head () const;

            public:
                nonvirtual  Priority    TopPriority () const;

            public:
                nonvirtual  void        RemoveHead ();

            public:
                nonvirtual  PriorityQueue<T>&   operator+= (T item);
                nonvirtual  PriorityQueue<T>&   operator+= (IteratorRep<PQEntry<T> >* it);
                nonvirtual  PriorityQueue<T>&   operator-- ();

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


        }
    }
}

#endif  /*_Stroika_Foundation_Containers_PriorityQueue_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "PriorityQueue.inl"
