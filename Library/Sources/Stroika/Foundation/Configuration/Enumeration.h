/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Enumeration_h_
#define _Stroika_Foundation_Configuration_Enumeration_h_  1

#include    "../StroikaPreComp.h"

#include    "Common.h"


/**
 *  \file
 *
 * TODO:
 *      @todo - maybe stuff like Add(ENUM, ENUM), and DIFF (ENum,ENUM) to workouarnd
 *              issues with too-strong typing with enum class?? (avoid so many casts)
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /**
             */
            template    <typename   ENUM>
            ENUM    Inc (ENUM e);


            /**
             *  \def Define_Start_End_Count
             *
             *      Define meta information on enums using standardized names, so you can generically
             *      write things like Can do for (i = eSTART; i != eEND; i = Inc (i));
             */
#define Define_Start_End_Count(FIRST_ITEM,LAST_ITEM)\
    eSTART      =   FIRST_ITEM,\
                    eEND        =   LAST_ITEM + 1,\
                                    eCOUNT      =   eEND - eSTART,


        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_Enumeration_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Enumeration.inl"
