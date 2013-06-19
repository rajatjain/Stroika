/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMapping_h_
#define _Stroika_Foundation_Containers_SortedMapping_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Concepts.h"

#include    "Mapping.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha-Beta">Alpha-Beta</a>
 *
 *
 *  TODO:
 *      @todo   Use TRAITS mechanism - like with Bag<>
 *
 *      @todo   Fix constructors to work better/more flexibly like other containers (copy from iterator etc)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      A SortedMapping is a Mapping<Key,T> which remains sorted (iterator) by the Key.
             *
             *  Note - this class might have been called "Dictionary".
             *
             *  @see    Mapping<Key,T>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename Key, typename T>
            class   SortedMapping : public Mapping<Key, T> {
            public:
                RequireElementTraitsInClass(RequireOperatorLess, Key);

            private:
                typedef     Mapping<Key, T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                SortedMapping ();

            public:
                nonvirtual  SortedMapping<Key, T>&  operator= (const SortedMapping<Key, T>& src);

            protected:
                explicit SortedMapping (const _SharedPtrIRep& rep);
            };


            /**
             *  \brief  Implementation detail for SortedMapping<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the SortedMapping<T> container API.
             *
             *  Note that this doesn't add any methods, but still serves the purpose of allowing
             *  testing/validation that the subtype information is correct (it is sorted).
             */
            template    <typename Key, typename T>
            class   SortedMapping<Key, T>::_IRep : public Mapping<Key, T>::_IRep {
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "SortedMapping.inl"

#endif  /*_Stroika_Foundation_Containers_SortedMapping_h_ */
