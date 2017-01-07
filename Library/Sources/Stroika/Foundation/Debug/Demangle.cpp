/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     defined (__GCC__)
#include    <cxxabi.h>
#elif   qPlatform_Windows
#include    <Windows.h>
#include    <Dbghelp.h>
#endif

#include    "Demangle.h"

using   namespace   Stroika::Foundation;



#if     qPlatform_Windows
// otherwise modules linking with this code will tend to get link errors without explicitly linking
// to this module...
#pragma comment (lib, "Dbghelp.lib")
#endif




/*
 ********************************************************************************
 ************************ Debug::DropIntoDebuggerIfPresent **********************
 ********************************************************************************
 */
Characters::String    Debug::Demangle (const Characters::String& originalName)
{
#if     defined (__GCC__)
    int     status {};
    char*   realname = abi::__cxa_demangle (originalName.AsNarrowSDKString ().c_str (), 0, 0, &status);
    auto&&  cleanup = Execution::Finally ([&realname]() { if (realname != nullptr) { ::free (realname); } });
    if (status == 0) {
        return Characters::String::FromNarrowSDKString (realname);
    }
#elif   qPlatform_Windows
    char    resultBuf[1000];
    if (::UnDecorateSymbolName (originalName.AsNarrowSDKString ().c_str (), resultBuf, sizeof (resultBuf), UNDNAME_COMPLETE) != 0) {
        return Characters::String::FromNarrowSDKString (resultBuf);
    }
#endif
    return originalName;
}
