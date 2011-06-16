/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_TString_h_
#define	_Stroika_Foundation_Characters_TString_h_	1


#include	"../StroikaPreComp.h"

#include	<string>

#include	"TChar.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			typedef	std::basic_string<TChar>	TString;

#if 0
//backwards compat name - use "TString" from now on
//typedef	TString	tstring;
#endif


			string	tstring2ANSI (const TString& s);
			wstring	tstring2Wide (const TString& s);

			TString	ANSI2tstring (const string& s);
			TString	Wide2tstring (const wstring& s);

			TString	totstring (const string& s);
			TString	totstring (const wstring& s);

		}
	}
}
#endif	/*_Stroika_Foundation_Characters_TString_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"TString.inl"



