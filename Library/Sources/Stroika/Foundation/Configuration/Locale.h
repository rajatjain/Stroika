/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Locale_h_
#define _Stroika_Foundation_Configuration_Locale_h_  1

#include    "../StroikaPreComp.h"

#include    <locale>
#include    <vector>

#include    "../Characters/String.h"
#include    "Common.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   verify and document if for windows this is LOCALE_USER_DEFAULT or LOCALE_SYSTEM_DEFAULT.
 *              SB USER!
 *
 *      @todo   reconsider  if GetPlatformDefaultLocale SB inlined, since calls inline funciton that seems relatively
 *              big and I think maybe OK to cache value (static).
 *
 *      @todo   Enhance FindLocaleName() to make second param - optional (territory)
 *
 *      @todo   Consider the idea of a thread-local current locale (locale()). This would be handy
 *              for server applications like HealthFrameWorks Server which might get a locale
 *              from the client connection, for use in generating reports etc.
 *
 *      @todo   rewrite GetAvailableLocales and FindLocaleName() to look at source code for
 *              locale -a, and see what it does on posix systems, and to use EnumSystemLocales()
 *              and mapping to locale names, on windows platform (or better strategy if I can find it).
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /**
             *  In C++, the default locale is "C", not the one inherited from the OS.
             *  Its not hard to get/set the one from the OS, but I've found it not well documented,
             *  so this is intended to make it a little easier/more readable.
             */
            std::locale GetPlatformDefaultLocale ();


            /**
             *  \brief  Set the operating system locale into the current C++ locale used by locale
             *          functions (and most locale-dependent stroika funcitons).
             *
             *  In C++, the default locale is "C", not the one inherited from the OS.
             *  Its not hard to get/set the one from the OS, but I've found it not well documented,
             *  so this is intended to make it a little easier/more readable.
             *
             */
            void    UsePlatformDefaultLocaleAsDefaultLocale ();


            /**
             *  \brief  List all installed locale names (names which can be passed to std::locale::CTOR)
             *
             *  I'm quite surprised this appears so hard to to in stdC++. I must be missing something...
             */
            vector<Characters::String>    GetAvailableLocales ();


            /**
             *  \brief  Not all systems appear to follow the same naming conventions for locales, so help lookup
             *
             *  Not all systems appear to follow the same naming conventions for locales, so provide a handy
             *  lookup function.
             *
             *  This will throw an exception if no matching locale is fine
             */
            Characters::String    FindLocaleName (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode);


            /**
             *  \brief  Find the locale matching these properties (for exception trying)
             *
             *  This will return a valid locale object with the prescribed properties, or it will raise
             *  an exception.
             */
            locale    FindNamedLocale (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode);


        }
    }
}






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Locale.inl"

#endif  /*_Stroika_Foundation_Configuration_Locale_h_*/
