/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_UserCanceledException_h_
#define _Stroia_Foundation_Execution_UserCanceledException_h_   1

#include    "../StroikaPreComp.h"

#include    "SilentException.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  mostly treat the same as SilentException
             */
            class   UserCanceledException : public SilentException {
            public:
                static  const   UserCanceledException   kThe;
            };


            template    <>
            [[noreturn]]    void    Throw (const UserCanceledException& e2Throw);


        }
    }
}



#endif  /*_Stroia_Foundation_Execution_UserCanceledException_h_*/
