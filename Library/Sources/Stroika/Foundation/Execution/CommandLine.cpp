/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/TString.h"

#include    "CommandLine.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;





Sequence<String>  Execution::ParseCommandLine (int argc, const char* argv[])
{
    Require (argc >= 0);
    Sequence<String>  results;
    for (int i = 0; i < argc; ++i) {
        results.push_back (Characters::NarrowSDKStringToWide (argv[i]));
    }

    return results;
}

Sequence<String>  Execution::ParseCommandLine (int argc, const wchar_t* argv[])
{
    Require (argc >= 0);
    Sequence<String>  results;
    for (int i = 0; i < argc; ++i) {
        results.push_back (argv[i]);
    }
    return results;
}




namespace   {
    String  Simplify2Compare_ (const String& actualArg)
    {
#if     qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
        return actualArg.StripAll ([](Characters::Character c) -> bool { return c == '-' or c == '/'; }).ToLowerCase ();
#else
        struct  FOO {
            static  bool    Simplify2Compare_LAMBDA_ (Characters::Character c)      {   return c == '-' or c == '/';    }
        };
        return actualArg.StripAll (FOO::Simplify2Compare_LAMBDA_).ToLowerCase ();
#endif
    }
}

bool    Execution::MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern)
{
    // Command-line arguments must start with - or / (windows only)
    if (actualArg.empty ()) {
        return false;
    }
#if     qPlatform_Windows
    if (actualArg[0] != '-' and actualArg[0] != '/') {
        return false;
    }
#else
    if (actualArg[0] != '-') {
        return false;
    }
#endif
    return Simplify2Compare_ (actualArg) == Simplify2Compare_ (matchesArgPattern);
}

bool    Execution::MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern, String* associatedArgResult)
{
    Require (matchesArgPattern.GetLength () > 0 and matchesArgPattern[matchesArgPattern.GetLength () - 1] == '=');
    AssertNotImplemented ();
    // must first strip everything after the '=' in the actualarg, and then similar to first overload...
    return false;
}

bool    Execution::MatchesCommandLineArgument (const Iterable<String>& argList, const String& matchesArgPattern)
{
#if 1
    return argList.ApplyUntilTrue ([matchesArgPattern] (String i) ->bool { return Execution::MatchesCommandLineArgument (i, matchesArgPattern); });
#else
    for (String i : argList) {
        if (MatchesCommandLineArgument (i, matchesArgPattern)) {
            return true;
        }
    }
    return false;
#endif
}

bool    Execution::MatchesCommandLineArgument (const Iterable<String>& argList, const String& matchesArgPattern, String* associatedArgResult)
{
#if 1
    return argList.ApplyUntilTrue ([matchesArgPattern, associatedArgResult] (String i) -> bool { return Execution::MatchesCommandLineArgument (i, matchesArgPattern); });
#else
    for (String i : argList) {
        if (MatchesCommandLineArgument (i, matchesArgPattern, associatedArgResult)) {
            return true;
        }
    }
    return false;
#endif
}

