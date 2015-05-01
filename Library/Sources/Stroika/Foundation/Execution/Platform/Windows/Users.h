/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_Users_h_
#define _Stroika_Foundation_Execution_Platform_Windows_Users_h_   1

#include    "../../../StroikaPreComp.h"

#if     !qPlatform_Windows
#error  "ONLY INCLUDE FOR Windows"
#endif

#include    <Windows.h>

#include    "../../../Configuration/Common.h"
#include    "../../../Characters/String.h"

#include    "../../Exceptions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Platform {
                namespace   Windows {

                    using   Characters::String;

                    String  SID22UserName (PSID sid);


                }
            }
        }
    }
}



#endif  /*_Stroika_Foundation_Execution_Platform_Windows_Users_h_*/
