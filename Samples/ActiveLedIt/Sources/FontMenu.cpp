/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "Stroika/Foundation/StroikaPreComp.h"

#include	<afxwin.h>

#include	"Stroika/Frameworks/Led/GDI.h"

#include	"Resource.h"

#include	"FontMenu.h"





namespace	{
	
	Led_InstalledFonts	sInstalledFonts;	// Keep a static copy for speed, and so font#s are static throughout the life of the applet

}







/*
 ********************************************************************************
 *********************************** GetUsableFontNames *************************
 ********************************************************************************
 */
const vector<Led_SDK_String>&	GetUsableFontNames ()
{
	return sInstalledFonts.GetUsableFontNames ();
}






/*
 ********************************************************************************
 *********************************** CmdNumToFontName ***************************
 ********************************************************************************
 */
Led_SDK_String	CmdNumToFontName (UINT cmdNum)
{
	Led_Require (cmdNum >= kBaseFontNameCmd);
	Led_Require (cmdNum <= kLastFontNameCmd);
	const vector<Led_SDK_String>&	fontNames	=	GetUsableFontNames ();
	return (fontNames[cmdNum-kBaseFontNameCmd]);
}



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
