/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_BinaryOutputStreamFromOStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_BinaryOutputStreamFromOStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    "OutputStreamAdapter.h"


#pragma message ("Warning: BinaryOutputStreamFromOStreamAdapter FILE DEPRECATED - use OutputStreamAdapter")


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                using BinaryOutputStreamFromOStreamAdapter = OutputStreamAdapter<Memory::Byte>;


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Streams_iostream_BinaryOutputStreamFromOStreamAdapter_h_*/
