/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedTally_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedTally_stdmap_h_   1


#include    "../../StroikaPreComp.h"

#include    "../SortedTally.h"



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
 *      @todo   Finish using CONTAINER_LOCK_HELPER_() - synchronization support
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
                template    <typename T, typename TRAITS = SortedTally_DefaultTraits<T>>
                class   SortedTally_stdmap : public SortedTally<T, TRAITS> {
                private:
                    typedef SortedTally<T, TRAITS>    inherited;

                public:
                    SortedTally_stdmap ();
                    SortedTally_stdmap (const SortedTally_stdmap<T, TRAITS>& src);
                    template <typename CONTAINER_OF_T>
                    explicit SortedTally_stdmap (const CONTAINER_OF_T& src);
                    SortedTally_stdmap (const T* start, const T* end);
                    SortedTally_stdmap (const TallyEntry<T>* start, const TallyEntry<T>* end);

                public:
                    nonvirtual  SortedTally_stdmap<T, TRAITS>& operator= (const SortedTally_stdmap<T, TRAITS>& src);

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
#include    "SortedTally_stdmap.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Tally_Array_h_*/

