/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Stack.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   VERY INCOMPLETE/WRONG IMPLEMENTATION - ESPECIALLY COPYING WRONG!!!
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Stack_LinkedList<T> is an LinkedList-based concrete implementation of the Stack<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                class   Stack_LinkedList : public Stack<T> {
                private:
                    using   inherited   =     Stack<T>;

                public:
                    Stack_LinkedList ();
                    Stack_LinkedList (const Stack_LinkedList<T>& m);
                    template    < typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if < Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const Stack_LinkedList<T>*>::value >::type >
                    explicit Stack_LinkedList (const CONTAINER_OF_T& cp);
                    template    <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Stack_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);


                public:
                    nonvirtual  Stack_LinkedList<T>& operator= (const Stack_LinkedList<T>& rhs);

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                private:
                    nonvirtual  void    AssertRepValidType_ () const;
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

#include    "Stack_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_ */
