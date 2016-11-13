/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cassert>
#include    <cstdlib>

#include    "BackTrace.h"
#include    "Debugger.h"
#include    "Trace.h"

#include    "Assertions.h"

#if     qPlatform_POSIX
#include    <cstdio>
#endif


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Debug;




CompileTimeFlagChecker_SOURCE(Stroika::Foundation::Debug, qDebug, qDebug);




#if     qDebug

namespace   {
    void    DefaultAssertionHandler_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept
    {
        try {
            DbgTrace ("%s (%s) failed in '%s'; %s:%d",
                      assertCategory == nullptr ? "Unknown assertion" : assertCategory,
                      assertionText == nullptr ? "" : assertionText,
                      functionName == nullptr ? "" : functionName,
                      fileName == nullptr ? "" : fileName,
                      lineNum
                     );
#if     qPlatform_POSIX
            fprintf (stderr, "%s (%s) failed in '%s'; %s:%d\n",
                     assertCategory == nullptr ? "Unknown assertion" : assertCategory,
                     assertionText == nullptr ? "" : assertionText,
                     functionName == nullptr ? "" : functionName,
                     fileName == nullptr ? "" : fileName,
                     lineNum
                    );
#endif
#if     qDefaultTracingOn
            {
                wstring tmp { Debug::BackTrace () };
                if (not tmp.empty ()) {
                    DbgTrace (L"BackTrace: %s", tmp.c_str ());
                }
            }
#endif
            DropIntoDebuggerIfPresent ();
            DbgTrace ("ABORTING...");
#if     qPlatform_POSIX
            fprintf (stderr, "ABORTING...\n");
#endif
        }
        catch (...) {
        }
        abort ();   // if we ever get that far...
    }
}

namespace {
    AssertionHandlerType    sAssertFailureHandler_      =   DefaultAssertionHandler_;
}




/*
 ********************************************************************************
 **************************** Debug::GetAssertionHandler ************************
 ********************************************************************************
 */
AssertionHandlerType    Stroika::Foundation::Debug::GetAssertionHandler ()
{
    return sAssertFailureHandler_;
}


/*
 ********************************************************************************
 ************************** Debug::GetDefaultAssertionHandler *******************
 ********************************************************************************
 */
AssertionHandlerType    Stroika::Foundation::Debug::GetDefaultAssertionHandler ()
{
    return DefaultAssertionHandler_;
}

/*
 ********************************************************************************
 ********************************* Debug::SetAssertionHandler *******************
 ********************************************************************************
 */
void    Stroika::Foundation::Debug::SetAssertionHandler (AssertionHandlerType assertionHandler)
{
    sAssertFailureHandler_ = (assertionHandler == nullptr) ? DefaultAssertionHandler_ : assertionHandler;
}






DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-noreturn\"");
// Cannot figure out how to disable this warning? -- LGP 2014-01-04
//DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Wenabled-by-default\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
[[noreturn]]    void    Stroika::Foundation::Debug::Private_::Debug_Trap_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept
{
    static  bool    s_InTrap    =   false;
    if (s_InTrap) {
        // prevent infinite looping if we get an assertion triggered while processing an assertion.
        // And ignore threading issues, because we are pragmatically aborting at this stage anyhow...
        abort ();
    }
    s_InTrap = true;
    (sAssertFailureHandler_) (assertCategory, assertionText, fileName, lineNum, functionName);
    s_InTrap = false;   //  in case using some sort of assertion handler that allows for continuation
    //  (like under debugger manipulation of PC to go a little further in the code)
}
//DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Wenabled-by-default\"");
DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-noreturn\"");
#endif


