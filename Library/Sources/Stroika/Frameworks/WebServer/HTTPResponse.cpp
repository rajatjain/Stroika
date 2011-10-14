/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>

#include	"../../Foundation/Characters/Format.h"
#include	"../../Foundation/Containers/Common.h"
#include	"../../Foundation/DataExchangeFormat/BadFormatException.h"
#include	"../../Foundation/Debug/Assertions.h"
#include	"../../Foundation/Execution/Exceptions.h"
#include	"../../Foundation/Memory/SmallStackBuffer.h"

#include	"HTTPResponse.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Memory;

using	namespace	Stroika::Frameworks;
using	namespace	Stroika::Frameworks::WebServer;







/*
 ********************************************************************************
 ************************ WebServer::HTTPResponse *******************************
 ********************************************************************************
 */
HTTPResponse::HTTPResponse (Streams::BinaryOutputStream& outStream, const InternetMediaType& ct)
	: fOutStream_ (outStream)
	, fStatus (StatusCodes::kOK)
	, fHeaders_ ()
	, fAnyWritesDone_ (false)
	, fContentType_ (ct)
	, fBytes_ ()
{
	AddHeader (L"Server", L"Stroka-Based-Web-Server");
}

void	HTTPResponse::SetContentType (const InternetMediaType& contentType)
{
	fContentType_ = contentType;
}

void	HTTPResponse::AddHeader (String headerName, String value)
{
	Require (not fAnyWritesDone_);
	fHeaders_.insert (map<String,String>::value_type (headerName, value));
}

void	HTTPResponse::ClearHeader ()
{
	fHeaders_.clear ();
}

void	HTTPResponse::ClearHeader (String headerName)
{
	map<String,String>::iterator i = fHeaders_.find (headerName);
	if (i != fHeaders_.end ()) {
		fHeaders_.erase (i);
	}
}

void	HTTPResponse::Flush ()
{
	if (!fAnyWritesDone_) {

		{
			wstring	statusMsg;
			switch (fStatus) {
				case 200: statusMsg = L"OK"; break;
				case 301: statusMsg = L"Moved Permanently"; break;
			}
			wstring	version	=	L"1.1";
			wstring	tmp	=	Characters::Format (L"HTTP/%s %d %s\r\n", version.c_str (), fStatus, statusMsg.c_str ());
		}

		for (map<String,String>::const_iterator i = fHeaders_.begin (); i != fHeaders_.end (); ++i) {
			wstring	tmp	=	Characters::Format (L"%s: %s\r\n", i->first.As<wstring> ().c_str (), i->second.As<wstring> ().c_str ());
			string	utf8	=	String (tmp).AsUTF8 ();
			fOutStream_.Write (reinterpret_cast<const Byte*> (Containers::Start (utf8)), reinterpret_cast<const Byte*> (Containers::End (utf8)));
		}

		const char	kCRLF[]	=	"\r\n";
		fOutStream_.Write (reinterpret_cast<const Byte*> (kCRLF), reinterpret_cast<const Byte*> (kCRLF + 2));
	}
	// write BYTES to fOutStream
	if (not fBytes_.empty ()) {
		fOutStream_.Write (Containers::Start (fBytes_), Containers::End (fBytes_));
		fBytes_.clear ();
	}
	fAnyWritesDone_ = true;
}

void	HTTPResponse::Redirect (const wstring& url)
{
	if (fAnyWritesDone_) {
		Execution::DoThrow (Execution::StringException (L"Redirect after data flushed"));
	}
	fBytes_.clear ();
#if 1
	// PERHAPS should clear some header values???
	AddHeader (L"Connection", L"close");	// needed for redirect
	AddHeader (L"Location", url);	// needed for redirect
	fStatus = 301;
#else
	writeln (L"HTTP/1.1 301 Moved Permanently\r\n");
	writeln (L"Server: SPECIFYSERVERFROMHEADERSREDCORD\r\n");
	writeln (L"Connection: close\r\n");
	printf (L"Location: %s\r\n", url.c_str ());
	writeln (L"\r\n");
#endif
	Flush ();
}
