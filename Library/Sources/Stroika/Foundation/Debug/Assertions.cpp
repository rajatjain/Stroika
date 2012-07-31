/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cassert>
#include    <cstdlib>

#include    "Debugger.h"
#include    "Trace.h"

#include    "Assertions.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Debug;


#if     qDebug

namespace   {
    void    DefaultAssertionHandler_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName)
    {
        DbgTrace ("%s (%s) failed in '%s'; %s:%d",
                  assertCategory == nullptr ? "Unknown assertion" : assertCategory,
                  assertionText == nullptr ? "" : assertionText,
                  functionName == nullptr ? "" : functionName,
                  fileName == nullptr ? "" : fileName,
                  lineNum
                 );
        DropIntoDebuggerIfPresent ();
        DbgTrace ("ABORTING...");
        abort ();   // if we ever get that far...
    }
}

namespace {
    AssertionHandlerType    sAssertFailureHandler_      =   DefaultAssertionHandler_;
}


AssertionHandlerType    Stroika::Foundation::Debug::GetAssertionHandler ()
{
    return sAssertFailureHandler_;
}

AssertionHandlerType    Stroika::Foundation::Debug::GetDefaultAssertionHandler ()
{
    return DefaultAssertionHandler_;
}

void    Stroika::Foundation::Debug::SetAssertionHandler (AssertionHandlerType assertionHandler)
{
    sAssertFailureHandler_ = (assertionHandler == nullptr) ? DefaultAssertionHandler_ : assertionHandler;
}

void    Stroika::Foundation::Debug::Private::Debug_Trap_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName)
{
	static	bool	s_InTrap	=	false;
	if (s_InTrap) {
		// prevent infinite looping if we get an assertion triggered while processing an assertion.
		// And ignore threading issues, because we are pragmatically aborting at this stage anyhow...
		abort ();
	}
	s_InTrap = true;
	try {
		(sAssertFailureHandler_) (assertCategory, assertionText, fileName, lineNum, functionName);
		s_InTrap = false;	// in case using some sort of assertion handler that allows for continuation
	}
	catch (...) {
		s_InTrap = false;	// in case using some sort of assertion handler that allows for continuation
		throw;
	}
}
#endif


