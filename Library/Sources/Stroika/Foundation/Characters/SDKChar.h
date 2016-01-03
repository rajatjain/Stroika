/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_SDKChar_h_
#define _Stroika_Foundation_Characters_SDKChar_h_ 1

#include    "../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <tchar.h>
#endif

#include    "../Configuration/Common.h"



/**
 *  Each platform SDK has its own policy for representing characters. Some use narrow characters (char),
 *  and a predefined code page (often configured via locale), and others use wide-characters (wchar_t unicode).
 *
 *  SDKChar is the underlying represenation of the SDK's characters - whether it be narrow or wide.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             *  qTargetPlatformSDKUseswchar_t
             *
             *  Defines if we use wchar_t or char for most platform interfaces (mostly applicable/useful for windows)
             */
#if     !defined (qTargetPlatformSDKUseswchar_t)
#error "qTargetPlatformSDKUseswchar_t should normally be defined indirectly by StroikaConfig.h"
#endif


            /**
             *  SDKChar is the kind of character passed to most/default platform SDK APIs.
             */
#if     qTargetPlatformSDKUseswchar_t
            using       SDKChar     =   wchar_t;
#else
            using       SDKChar     =   char;
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "SDKChar.inl"

#endif  /*_Stroika_Foundation_Characters_SDKChar_h_*/
