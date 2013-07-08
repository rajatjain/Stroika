/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Set.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Set_LinkedList<T, TRAITS> is an LinkedList-based concrete implementation of the Set<T, TRAITS> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = Set_DefaultTraits<T>>
                class   Set_LinkedList : public Set<T, TRAITS> {
                private:
                    typedef     Set<T, TRAITS>  inherited;

                public:
                    Set_LinkedList ();
                    Set_LinkedList (const Set_LinkedList<T, TRAITS>& m);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Set_LinkedList (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Set_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);


                public:
                    nonvirtual  Set_LinkedList<T, TRAITS>& operator= (const Set_LinkedList<T, TRAITS>& rhs);


                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
                    class   IteratorRep_;

                private:
                    nonvirtual  const Rep_&  GetRep_ () const;
                    nonvirtual  Rep_&        GetRep_ ();
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

#include    "Set_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_ */
