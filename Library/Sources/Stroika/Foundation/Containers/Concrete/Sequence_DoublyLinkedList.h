/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Sequence.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Sequence_DoublyLinkedList<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <class T>
                class   Sequence_DoublyLinkedList : public Sequence<T> {
                private:
                    typedef     Sequence<T>  inherited;

                public:
                    Sequence_DoublyLinkedList ();
                    Sequence_DoublyLinkedList (const Sequence_DoublyLinkedList<T>& s);
                    template <typename CONTAINER_OF_T>
                    explicit Sequence_DoublyLinkedList (const CONTAINER_OF_T& s);
                    template <typename COPY_FROM_ITERATOR>
                    explicit Sequence_DoublyLinkedList (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

                    nonvirtual  Sequence_DoublyLinkedList<T>& operator= (const Sequence_DoublyLinkedList<T>& s);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
                    class   IteratorRep_;
                };


            }
        }
    }
}




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Sequence_DoublyLinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_ */
