/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Tally_Array_h_   1


#include    "../../StroikaPreComp.h"

#include    "../Tally.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals ()
 *
 *      @todo   Implementation currently stores list of T's rather than a list of
 *              TallyEntry<T>, and as a result computes tally's less efficiently
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                class   Tally_Array : public Tally<T> {
                private:
                    typedef Tally<T>    inherited;

                public:
                    Tally_Array ();
                    Tally_Array (const Tally<T>& src);
                    Tally_Array (const Tally_Array<T>& src);
                    Tally_Array (const T* start, const T* end);
                    Tally_Array (const TallyEntry<T>* start, const TallyEntry<T>* end);

                public:
                    nonvirtual  Tally_Array<T>& operator= (const Tally_Array<T>& src);

                public:
                    nonvirtual  size_t  GetCapacity () const;
                    nonvirtual  void    SetCapacity (size_t slotsAlloced);

                public:
                    nonvirtual  void    Compact ();

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class Rep_;
                    class IteratorRep_;

                private:
                    nonvirtual  const Rep_& GetRep_ () const;
                    nonvirtual  Rep_&       GetRep_ ();
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
#include    "Tally_Array.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Tally_Array_h_*/

