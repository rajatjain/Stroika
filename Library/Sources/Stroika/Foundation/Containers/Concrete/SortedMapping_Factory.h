/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_h_

#include    "../../StroikaPreComp.h"



/**
 *  \file
 *
 *  TODO:
 *		@todo	Extend this metaphor to have different kinds of factories, like mkMapping_Fasted, _Smallest, etc...
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

			template    <typename Key, typename T>
			class	Mapping;

            namespace   Concrete {


				/**
                 * \brief   Create the default backend implementaiton of a Mapping<> template
                 */
                template    <typename Key, typename T>
				SortedMapping<Key,T>	mkSortedMapping_Default ();


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_ */


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
//	Any module #including the Mapping_Factory.h must explicit include the SortedMapping_Factory.inl 
//	someplace (often a different place to avoid nested includes)
//#include    "SortedMapping_Factory.inl"
