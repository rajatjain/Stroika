/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BlockAllocator_h_
#define _Stroika_Foundation_Memory_BlockAllocator_h_    1

#include    "../StroikaPreComp.h"

//#include    <algorithm>
#include    <cstddef>

#include    "../Configuration/Common.h"

#include    "../Execution/ModuleInit.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 *
 *  @todo   Consider doing GetMem_Util_ code outside of the context of the lock-gaurd, and if
 *          we get multiple results, just patch them into the linked list. That way in case of
 *          multithreading (when we're paging in freepool) - we'll do less busy waiting.
 *
 *  @todo   BlockAllocator<T> could hugely benefit from some optimistic locking
 *          strategy - like in http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3341.pdf
 *
 *          Make sure when we experiment with that - we include it here as one of our first
 *          optimization points!
 *
 *  @todo   And maybe use TRAITS on BlockAllocator to define some stuff
 *          about strategies (options) - like how to share pools?
 *
 *  @todo   Comments generally need a thorough review. Many VERY VERY old - from Led days.
 *
 *  @todo   Fix all the BlockAllocated<T>::fValue_ code - clean it up, put it in INL file, and
 *          think through more carefully CTORs etc - to make it act more like a "T" type.
 *
 *          Sterl thinks its best to just add a generic solution like:
 *              template <typename T>
 *              struct  AsObject {
 *                  AsObject ()    {    }
 *
 *                  AsObject (T value) :         fValue (value)    {    }
 *
 *                  nonvirtual  operator T () const    {        return fValue;    }
 *                  nonvirtual  operator T ()    {        return fValue;    }
 *
 *                  T fValue;
 *              };
 *
 *          Closely related - document WHY we didn't just have BlockAllocated<T> inherit from T (issue is
 *          when T = int, for example. There maybe some template magic I can use to fix that, and then
 *          thats probably a better solution!
 *
 *  @todo   We should either add a VARIANT or template parameter to BlockAllocated<> saying whether we
 *          should force block allocation, or ONLY block-allocate if size is appropriate to one of our
 *          preformed pools.
 *
 *          The reason for this option is for better use in templates like LinkedList<> - where we might
 *          want to blockallocate for small sizes of T, but not for ALL.
 *
 *  @todo   BlockAllocator<>::Compact ()
 *              o   not sure this is useful, or worth the effort, but since
 *                  Sterl wrote it - leave it in for a while - til I get clearer experience.
 *
 *              o   BlockAllocationPool_<SIZE>::Compact () algorithm uses vector, which could fail if
 *                  we are so fragmented we cannot allocate the large contiguous block needed for the vector<>.
 *                  Catch-22. Maybe find a less costly strategy?
 *
 *              o   I make have broken something in Compact() routine when I transcribed it from Sterl's code.
 *                  never tested (by me).
 */





namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /**
             *  Low-level tool to allocate and free memory from a fixed size/element pool. Very high performance since
             *  no searching or coalescing ever needed, but at the cost of creating some amount of fragmentation.
             *
             *  If qAllowBlockAllocation true (default) - this will use the optimized block allocation store, but if qAllowBlockAllocation is
             *  false (0), this will just default to the global ::new/::delete
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            template    <typename   T>
            class   BlockAllocator  {
            public:
                /**
                 *  \req (n == sizeof (T))
                 */
                static  void*   Allocate (size_t n);

            public:
                /**
                 *  \req (p allocated by BlockAllocator<T>::Allocate ());
                 *  p can be nullptr
                 */
                static  void    Deallocate (void* p);

            public:
                /**
                  * Return to the free store all deallocated blocks whcih can be returned.
                  *
                  * This takes oN, where N is the total amount of operator new for this size.
                  *
                  * It also currently uses a large block of contiguous memory, which could fail.
                  *
                  * But it might sometimes return memory from a particular data structure (fixed element size pool) to
                  * the general free store.
                  */
                static  void    Compact ();
            };


            /**
             *  This can be referenced in your ModuleInit<> to force correct inter-module construction order.
             */
            Execution::ModuleDependency MakeModuleDependency_BlockAllocator ();


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BlockAllocator.inl"

#endif  /*_Stroika_Foundation_Memory_BlockAllocator_h_*/
