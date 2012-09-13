/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_Config_h_
#define _Stroika_Framework_Led_Config_h_ 1

#include    "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    LedConfig
@DESCRIPTION:
        <p>Support basic configuration defines for the operating system, and compiler, and some basic features
    (like character set).
    </p>
 */


#if     qMacOS
#if     defined (__cplusplus) && !defined (__SOMIDL__)
#include    <ConditionalMacros.h>
#endif
#endif


#if     defined (__GNUC__)
#include    <features.h>
#endif



/*
 *  This Numeric Led version is intended to allow you to conditionally compile code
 *  based on different versions of Led. The Led::kVersion string is a symbolic representation
 *  of this version.
 *
 *  Don't count on the particular hard-wirded version number. Instead - compare as follows:
 *      #if     qLed_FullVersion > MAKE_LED_FULL_VERSION (1, 0, qLed_Version_Stage_Beta, 3, 1)
 *          DOTHIS ();
 *      #else
 *          DOTHAT ();
 *      #endif
 */

// Support declarations for version#
#define qLed_Version_Stage_Dev              0x1
#define qLed_Version_Stage_Alpha            0x2
#define qLed_Version_Stage_Beta             0x3
#define qLed_Version_Stage_ReleaseCandidate 0x4
#define qLed_Version_Stage_Release          0x5

#define MAKE_LED_FULL_VERSION(_Major_,_Minor_,_Stage_,_SubStage_,_FinalBuild_) \
    ( (_Major_ << 25) | \
      (_Minor_ << 17) | \
      (_Stage_ << 9) | \
      (_SubStage_ << 1) | \
      (_FinalBuild_) \
    )

#define qLed_FullVersion        MAKE_LED_FULL_VERSION (qLed_Version_Major, qLed_Version_Minor, qLed_Version_Stage, qLed_Version_SubStage, qLed_Version_FinalBuild)




// This part is where we actually update the version#
#define qLed_Version_Major                  3
#define qLed_Version_Minor                  1
#define qLed_Version_MajorMinor             0x31
#define qLed_Version_Stage                  qLed_Version_Stage_Beta
#define qLed_Version_SubStage               2
#define qLed_Version_FinalBuild             0

#define qLed_MajorMinorVersionString        "3.1"
#define qLed_ShortVersionString             "3.1b2x"






/*
 * OS Defines.
 *
 *      Be sure OS defines are defined to ZERO if not defined. This is so we can write code
 *  like if (qMacOS) as well as code like #if qMacOS
 */
#ifndef qMacOS
#define qMacOS      defined (macintosh)
#endif
#ifndef qWindows
#define qWindows    (defined (_WIN32) || defined (WIN32))
#endif
#ifndef qXWindows
#define qXWindows   defined (__GNUC__) && !qMacOS
#endif

#if     qMacOS + qWindows + qXWindows > 1
#error  "Only one of these should be defined"
#endif
#if     !qMacOS && !qWindows && !qXWindows
#error  "One of these should be defined - right now thats all we support"
#endif














/// MAY THIS A COMMENTED DEFINE - AND FIXUP THE ABOVE __GNUC__ STUFF LIKE OTHER COMPILER SETTINGS - LGP 991214
#ifndef qBitSetTemplateAvailable
#define qBitSetTemplateAvailable    1
#endif















/*
 *  Do things like memcpy () get inlined by the compiler, or should we do so ourselves
 *  manually for cases that matter.
 */
#ifndef qComplilerDoesNotInlineIntrinsics
#define qComplilerDoesNotInlineIntrinsics   0
#endif







/*
 *  Not a bug to have this - I think - but doesn't appear part of any ansi spec???
 */
#ifndef qHasIsAscii
#define qHasIsAscii                         0
#endif









/*
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 */


/*
@CONFIGVAR:     qDebug
@DESCRIPTION:   <p>Be sure debug #defines setup properly. These are just some sanity checks.
    In Led - we use qDebug. Just make sure other defines are setup consistently
    with that define.</p>
        <p>The default value of the qDebug variable is !defined (NDEBUG)</p>
        <p>But typically, it is set externally via prefix files (Precomp.pch, MSVC settings dialog, or some such).</p>
        <p>See also @'qHeavyDebugging'</p>
 */
#if     defined (__cplusplus)
#ifndef qDebug
// for some reason, directly defining this as qDebug=!defined(NDEBUG) didn't work on msvc
// LGP 070404
#if     defined(NDEBUG)
#define qDebug  0
#else
#define qDebug  1
#endif
#endif
#ifndef qDebug
#error  "This must be defined either 0 or 1, depending on if you are building a debug version or not"
#endif
#if     qDebug && defined (NDEBUG)
#error  "qDebug and NDEBUG must agree"
#endif
#if     !qDebug && !defined (NDEBUG)
#error  "qDebug and NDEBUG must agree"
#endif

#if     defined (_MSC_VER)
#if     qDebug && !defined (_DEBUG)
#error  "qDebug and _DEBUG must agree"
#endif
#if     !qDebug && defined (_DEBUG)
#error  "qDebug and _DEBUG must agree"
#endif
#endif
#endif




/*
@CONFIGVAR:     qHeavyDebugging
@DESCRIPTION:   <p>Led contains lots of internal debugging code which is mainly intended to find bugs
    in Led itself. Its less valuable for finding bugs in your programs' usage of Led. Much of that debugging
    code makes Led quite slow (for larger documents).</p>
        <p>The Led sample applications are all built with qHeavyDebugging when qDebug is on. But Led defaults
    to having this value false, so that your applications won't be needlessly slow.</p>
        <p>If you run into some subtle bug, or if you aren't worried about the speed of Led with large documents
when debugging is ON, then you may want to turn this flag ON.</p>
        <p>See also @'qDebug'</p>
 */
#ifndef qHeavyDebugging
#define qHeavyDebugging     0
#endif








/*
@CONFIGVAR:     qNestedTablesSupported
@DESCRIPTION:   <p>This define controls whether or not the Led WordProcessor/StyledTextIO class support nested tables.
            This will <em>not</em> be supported for Led 3.1, but probably will be in a release thereafter.
            There is SOME code available for both cases, but just a little (as of 2003-04-12).</p>
 */
#ifndef qNestedTablesSupported
#define qNestedTablesSupported  0
#endif





/*
@CONFIGVAR:     qSupportLed30CompatAPI
@DESCRIPTION:   <p>A way to incrementally, semi-backward-compatably implement API changes. Also, this
    serves as documentation.</p>
        <p>Any methods etc marked with @'qSupportLed30CompatAPI' will be supported (assuming this flag
    is set true) for Led 3.1, but probably NOT thereafter. You can get your programs compiling
    with this set true. But you should try getting stuff compiling with this
    set OFF soon thereafter. Using the NEW apis will (hopefully make your code clearer), but at
    any rate, make it more easy to migrate to the NEXT Led release.</p>
        <p>Default is OFF</p>
 */
#ifndef qSupportLed30CompatAPI
// ship with off, but test with it both ways!!!
//#define   qSupportLed30CompatAPI              1
#define qSupportLed30CompatAPI                      0
#endif






/*
 *  -----------------------        Character set Settings/Specifications         ---------------------------
 */



#if     qWindows
/*
 *  Some of Microsoft's headers use _UNICODE and some use UNICODE. If users set one or the other, treat that as having
 *  set both.
 */
#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE         // UNICODE is used by Windows headers
#endif
#endif

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE        // _UNICODE is used by C-runtime/MFC headers
#endif
#endif
#endif




#if     qSupportLed30CompatAPI
/*
@CONFIGVAR:     qLed_CharacterSet
@DESCRIPTION:   <p>OBSOLETE</p>
                <p>As of Led 3.1a4 - you must directly define @'qSingleByteCharacters' or
            @'qMultiByteCharacters' or @'qWideCharacters'.
                </P>
        <p>It was a difficult choice deciding how to deal with character set issues.
    There are four major character sets I was immediately concerned with supporting,
    and, of course, hundreds more that I'd like to be able to support in the future.
    And then there is the question of supporting different character sets at the same
    time.</p>
        <p>Since my immediate contractual obligations only require support for SJIS, and
    possibly ASCII, and or Extended ASCII, and not in any mixed form, I've allowed myself
    the luxury of not worrying beyond that for now. Also - I've taken the approach of
    building the choice of character set in at the compilation level. This is a
    simplification since it allows me to make type choices (as WinNT does for UNICODE)
    depending on character set choices. The difference probably doesn't matter for
    different single byte character sets.</p>
        <p>Anyhow - for the time being, the character set defined at compile time is
    given by qLed_CharacterSet.</p>
        <p><code>qLed_CharacterSet</code> can be any one of the following:
        <ul><code>
            <li>qASCII_CharacterSet</li><br>
            <li>qExtendedASCII_CharacterSet</li><br>
            <li>qSJIS_CharacterSet</li><br>
            <li>qUNICODE_CharacterSet</li><br>
        </ul></code>
        </p>
        <p>Depending on which value is specified, Led will automatically define one of:
        <ul><code>
            <li>qSingleByteCharacters</li><br>
            <li>qMultiByteCharacters</li><br>
            <li>qWideCharacters</li><br>
        </ul></code>
        and will use that value internally.
        </p>
        <p><b>WARNING</b>All this character set support is subject to change, and is being re-thought for future
    Led releases. I probably will go in the direction of just supporting ASCII, or UNICODE. And not all the various
    MBYTE character sets (mainly cuz then no good way to mix charactersets within a document). -- LGP 970722</p>
 */
#define qASCII_CharacterSet         1
#define qExtendedASCII_CharacterSet 2
#define qUNICODE_CharacterSet       3
#define qSJIS_CharacterSet          4

#ifndef qLed_CharacterSet
#ifdef  _UNICODE
#define qLed_CharacterSet       qUNICODE_CharacterSet
#else
#define qLed_CharacterSet       qExtendedASCII_CharacterSet
#endif
#endif


// DON'T Edit THESE directly - change the characterset, and allow
// these to be derivitive...
#if     qLed_CharacterSet == qASCII_CharacterSet
#define qSingleByteCharacters   1
#define qMultiByteCharacters    0
#define qWideCharacters         0
#elif   qLed_CharacterSet == qExtendedASCII_CharacterSet
#define qSingleByteCharacters   1
#define qMultiByteCharacters    0
#define qWideCharacters         0
#elif   qLed_CharacterSet == qUNICODE_CharacterSet
#define qSingleByteCharacters   0
#define qMultiByteCharacters    0
#define qWideCharacters         1
#else
#error  "Unknown character set"
#endif

#else

#if     defined (qLed_CharacterSet)
#error  "This is OBSOLETE - use qSingleByteCharacters/qMultiByteCharacters/qWideCharacters"
#endif

#endif






/*
@CONFIGVAR:     qSingleByteCharacters
@DESCRIPTION:   <p>Define this if you will only be editing 8-bit ascii or extended ASCII characters, and don't
            want to deal with UNICODE.</p>
                <p>See also @'qMultiByteCharacters' and @'qWideCharacters'. Note that
            @'qWideCharacters', @'qMultiByteCharacters', and @'qSingleByteCharacters' are mutually exclusive.</p>
                <p>Defaults OFF</p>
 */
#ifndef qSingleByteCharacters
#define qSingleByteCharacters           0
#endif


/*
@CONFIGVAR:     qMultiByteCharacters
@DESCRIPTION:   <p>Define this if you will only be editing multibyte text (e.g. SJIS) and don't
            want to deal with UNICODE.</p>
                <p>See also @'qMultiByteCharacters' and @'qSingleByteCharacters'. Note that
            @'qWideCharacters', @'qMultiByteCharacters', and @'qSingleByteCharacters' are mutually exclusive.</p>
                <p>Note that this is really only partially still supported, and may not be supported in the future (as of Led 3.1a4).
            To make this define work - you must implement yourself @'Led_IsLeadByte' and @'Led_IsValidSecondByte' and perhaps
            a few other small matters. The problem with doing this is that its not clear WHICH mutibyte code set you are compiling for.
            Led's multibyte support is designed to only work for a particular multibyte code set at a time (though this could
            be a global variable - perhaps - rather than a compile-time constant). Anyhow - if @'qSingleByteCharacters' isn't good
            enough - we strongly recomend using UNICODE (@'qWideCharacters').</p>
                <p>Defaults OFF</p>
 */
#ifndef qMultiByteCharacters
#define qMultiByteCharacters            0
#endif


/*
@CONFIGVAR:     qWideCharacters
@DESCRIPTION:   <p>Define this to work with the widest variety of characters - UNICODE. This is the preferred way
            to build Led, and as a result - is now the default (as of Led 3.1a4).</p>
                <p>See also @'qMultiByteCharacters' and @'qSingleByteCharacters'. Note that
            @'qWideCharacters', @'qMultiByteCharacters', and @'qSingleByteCharacters' are mutually exclusive.</p>
                <p>Defaults TRUE (if @'qMultiByteCharacters', and @'qSingleByteCharacters' are false).</p>
*/
#ifndef qWideCharacters
#if     qMultiByteCharacters || qSingleByteCharacters
#define qWideCharacters         0
#else
#define qWideCharacters         1
#endif
#endif





// Be sure ONLY ONE of these flags set
#if     qSingleByteCharacters && (qMultiByteCharacters || qWideCharacters)
#error  "Only one can be set at a time"
#endif
#if     qMultiByteCharacters && (qSingleByteCharacters || qWideCharacters)
#error  "Only one can be set at a time"
#endif
#if     qWideCharacters && (qSingleByteCharacters || qSingleByteCharacters)
#error  "Only one can be set at a time"
#endif
#if     !qSingleByteCharacters && !qMultiByteCharacters && !qWideCharacters
#error  "At least one should be set"
#endif







/*
@CONFIGVAR:     qSDK_UNICODE
@DESCRIPTION:   <p>Roughly this corresponds to the MSDEV -DUNICODE -D_UNICODE define. But can
    be used for other things as well on other platforms? Maybe?</p>
 */
#ifndef qSDK_UNICODE
#ifdef  _UNICODE
#define qSDK_UNICODE        1
#else
#define qSDK_UNICODE        0
#endif
#endif





/*
@CONFIGVAR:     qProvideIMESupport
@DESCRIPTION:   <p>Do we need to explictly support the IME?</p>
        <p>NB: qProvideIMESupport used to be called qRequireIMESupport, but now we
    simply provide it, and it essentailly becomes a no-op if you've no IME installed.</p>
        <p>This is ON by default (for windows). It degrades gracefully if no IME found.</p>
 */
#ifndef qProvideIMESupport
#if     qWindows
#define qProvideIMESupport                      1
#else
#define qProvideIMESupport                      0
#endif
#endif





/*
@CONFIGVAR:     qWorkAroundWin95BrokenUNICODESupport
@DESCRIPTION:   <p>Win95 doesn't completely support UNICODE. In particular, the clipboard/Drag&Drop stuff
    does't work with UNICODE. This workaround is on by default, and really only does anything if qWideCharacters
    is set on (depends on character set you've defined). This is here mostly for documentation of why I'm doing
    these workarounds (so they can be lifted in the future), and so folks can conidtionally compile them out
    as Win95 fades into history.</p>
        <p>This is ON by default (for windows, and if _UNICODE not defined - cuz there is no point - _UNICODE doesn't work on Win95)</p>
 */
#ifndef qWorkAroundWin95BrokenUNICODESupport
#define qWorkAroundWin95BrokenUNICODESupport        (qWindows && !defined (_UNICODE))
#endif









/*
@CONFIGVAR:     qPeekForMoreCharsOnUserTyping
@DESCRIPTION:   <p>A trick to get better (worse?) interactivity in typing.</p>
        <p>Defaults to true.</p>
 */
#ifndef qPeekForMoreCharsOnUserTyping
#define qPeekForMoreCharsOnUserTyping               1
#endif





/*
@CONFIGVAR:     qAllowRowsThatAreLongerThan255
@DESCRIPTION:   <p>This define allows control over whether we perform a packing optimization to save memory.
    If we do (qAllowRowsThatAreLongerThan255 FALSE) then we asssert were are never
    given a row > 255 pixels. Similarly, we disalow rows of more than 255 characters.
    If qAllowRowsThatAreLongerThan255 - fine - we just don't optimize packing.</p>
        <p>For now, not fully implemented. Only for RowHeights, not for RowStarts.</p>
        <p>See SPR#0257</p>
 */
#ifndef qAllowRowsThatAreLongerThan255
#define qAllowRowsThatAreLongerThan255              1
#endif





/*
@CONFIGVAR:     qDoubleClickSelectsSpaceAfterWord
@DESCRIPTION:   <p>qDoubleClickSelectsSpaceAfterWord is a very silly idea, imho, but it is a standard UI feature
    on MS Windows editors, and not uncommon on the Mac. As far as I can tell, the idea
    is just to select a word, plus all trailing whitespace on double clicks. The idea is
    that this somehow makes cut/paste work more easily. So it is sometimes refered
    to as smart cut and paste or intelligent cut and paste.</p>
        <p>By default I USED TO enable this only for windows.
    Now it is always OFF by default, since I think its too stupid, and annoying.
    Someday soon I'll implement better smart-cut/paste like Style(on Mac) does.</p>
 */
#ifndef qDoubleClickSelectsSpaceAfterWord
#if     qWindows
#define qDoubleClickSelectsSpaceAfterWord       0
#else
#define qDoubleClickSelectsSpaceAfterWord       0
#endif
#endif





/*
@CONFIGVAR:     qDynamiclyChooseAutoScrollIncrement
@DESCRIPTION:   <p>On slower computers, this can make scrolling appear a bit faster. At some risk to
    predictablity...</p>
 */
#ifndef qDynamiclyChooseAutoScrollIncrement
#define qDynamiclyChooseAutoScrollIncrement         1
#endif





/*
@CONFIGVAR:     qUseOffscreenBitmapsToReduceFlicker
@DESCRIPTION:   <p>Led already has very little flicker. This is because we are very careful to
    draw as little as possible, and to draw quickly. But some cases still exist.
    Like large pictures being drawn are flicker, cuz we must erase the bounds and then
    draw the picture.</p>
        <p>Using this gets rid of these few cases of flicker, but at a small performance cost
    in overall draw speed.</p>
        <P> THIS FLAG IS NOW OBSOLETE (as of Led 3.0b6). Its still supported for backwards compatability.
    But now all it does is set the default VALUE for the ImageUsingOffscreenBitmaps property.</p>
        <p>Instead of using this, use @'TextImager::SetImageUsingOffscreenBitmaps' ().</p>
 */
#ifndef qUseOffscreenBitmapsToReduceFlicker
#define qUseOffscreenBitmapsToReduceFlicker         1
#endif





/*
@CONFIGVAR:     qSilenceAnnoyingCompilerWarnings
@DESCRIPTION:   <p>Replaces 'qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings', 'qQuiteAnnoyingDominanceWarnings',
    'qQuiteAnnoyingDebugSymbolTruncationWarnings'</p>
 */
#ifndef qSilenceAnnoyingCompilerWarnings
#define qSilenceAnnoyingCompilerWarnings            1
#endif





namespace   Stroika {
    namespace   Frameworks {
        namespace   Led {
        }
    }
}
// Be sure namespace is defined for later use.







/*
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 */
#if     qMacOS

#ifndef TARGET_CARBON
#define TARGET_CARBON   0
#endif


/*
@CONFIGVAR:     qPeekAtQuickDrawGlobals
@DESCRIPTION:   <p><b>MAC ONLY</b></p>
 */
#if     defined (__cplusplus)
#ifndef qPeekAtQuickDrawGlobals
#if     TARGET_CARBON
#define qPeekAtQuickDrawGlobals 0
#else
#if     GENERATINGCFM
#define qPeekAtQuickDrawGlobals 0
#else
#define qPeekAtQuickDrawGlobals 1
#endif
#endif
#endif
#endif





/*
@CONFIGVAR:     qUseMacTmpMemForAllocs
@DESCRIPTION:   <p><b>MAC ONLY</b></p>
        <p>Use of TmpMemory on the mac allows for the editor to run in a small partition
    but to use all available system resources (memory) when the user requests it
    for a large clipbaord operation, or opening a large file, or whatever.</p>
        <p>I believe it generally makes sense to leave this on.</p>
        <p>LGP - 960314</p>
 */
#if     !defined (qUseMacTmpMemForAllocs)
#define qUseMacTmpMemForAllocs  1
#endif





/*
@CONFIGVAR:     qUseInternetConfig
@DESCRIPTION:   <p><b>MAC ONLY</b></p>
        <p>For URL support, and any other places where it is appropriate, should we try
    to use Internet Config (IC) when available? Mainly used right now by
    URL embeddings. Default to TRUE on mac. Not avialable elsewhere.</p>
 */
#ifndef qUseInternetConfig
#if     TARGET_CARBON
#define qUseInternetConfig      qSDKHasInternetConfig
#else
// disable it by default if using CFM68K - since right now I have no CFM68K lib for it
#if     defined(__CFM68K__)
#define qUseInternetConfig  0
#else
#define qUseInternetConfig  1
#endif
#endif
#endif

#endif  /*qMacOS*/











/*
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 */
#if     qWindows


/*
@CONFIGVAR:     qUseQuicktimeForWindows
@DESCRIPTION:   <p><b>Win32 ONLY</b></p>
        <p>When we attempt to draw Macintosh Picture objects, should we try using QuickTime For Windows?
    If not, the rectangle where the picture should be will be left blank.</p>
 */
#ifndef qUseQuicktimeForWindows
#define qUseQuicktimeForWindows         0
#endif





/*
@CONFIGVAR:     qUseSpyglassDDESDIToOpenURLs
@DESCRIPTION:   <p><b>Win32 ONLY</b></p>
        <p>For supporting openening URLs, use DDE to an application with Spyglass SDI/DDE.
    This takes over DDE processing for your app (if you care, this could probably
    be fixed).</p>
        <p>Detaults OFF now (as of Led 3.0d5, 2000/04/03) - because this may fail when Led embedded in OCX, and
    seems of only very marginal value anyhow. The ActiveX stuff SHOULD work, by now!</p>
 */
#ifndef qUseSpyglassDDESDIToOpenURLs
#define qUseSpyglassDDESDIToOpenURLs    0
#endif





/*
@CONFIGVAR:     qUseActiveXToOpenURLs
@DESCRIPTION:   <p><b>Win32 ONLY</b></p>
        <p>For supporting openening URLs.</p>
 */
#ifndef qUseActiveXToOpenURLs
#define qUseActiveXToOpenURLs           1
#endif

#endif  /*qWindows*/








/*
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 */
#if     qXWindows

/*
@CONFIGVAR:     qUseSystemNetscapeOpenURLs
@DESCRIPTION:   <p><b>X-Windows ONLY</b></p>
        <p>For supporting openening URLs.</p>
 */
#ifndef qUseSystemNetscapeOpenURLs
#define qUseSystemNetscapeOpenURLs          1
#endif

#endif  /*qWindows*/




#endif  /*_Stroika_Framework_Led_Config_h_*/
