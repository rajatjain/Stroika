/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_

#include    "../../StroikaPreComp.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkMapping_Fasted,
 *              mkMapping_Smallest, mkMappingWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Consider something like RegisterFactory_Mapping below
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename Key, typename T>
            class   Mapping;


            namespace   Concrete {


                /**
                 *  \brief   Create the default backend implementation of a Mapping<> container
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename Key, typename T>
                Mapping<Key, T>  mkMapping_Default ();


                // PROTO-IDEA - NOT IMPLEMENTED
#if 0
                template    <typename Key, typename T>
                void    RegisterFactory_Mapping (Mapping<Key, T> (*factory) () = nullptr);
#endif


            }
        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Mapping_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_ */
