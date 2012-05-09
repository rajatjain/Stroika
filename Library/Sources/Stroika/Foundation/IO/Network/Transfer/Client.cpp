/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../../StroikaPreComp.h"

#if	qPlatform_Windows
	#include	<atlbase.h>

	#include	<Windows.h>
	#include	<URLMon.h>
#endif

#include	"../../../Characters/Format.h"
#include	"../../../Execution/Exceptions.h"

#include	"Client.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::Network;
using	namespace	Stroika::Foundation::IO::Network::Transfer;








/*
 ********************************************************************************
 ******************************* Transfer::Request *****************************
 ********************************************************************************
 */
Request::Request ()
	: fMethod ()
	, fOverrideHeaders ()
	, fData ()
	, fContentType ()
{
}





/*
 ********************************************************************************
 ******************************* Transfer::Response *****************************
 ********************************************************************************
 */
Response::Response ()
	: fData ()
	, fHeaders ()
	, fStatus ()
{
}






/*
 ********************************************************************************
 **************************** Transfer::Connection ******************************
 ********************************************************************************
 */
Response	Connection::Get (const map<String,String>& extraHeaders)
{
	///// TODO:  ADD HTTP::METHODS module (like with Headers) to enumerate common/basic METHODS GET/POST/ETC...
	Request	r;
	r.fMethod = L"GET";
	r.fOverrideHeaders = extraHeaders;
	return SendAndRequest (r);
}

Response	Connection::Post (const vector<Byte>& data, const InternetMediaType& contentType, const map<String,String>& extraHeaders)
{
	///// TODO:  ADD HTTP::METHODS module (like with Headers) to enumerate common/basic METHODS GET/POST/ETC...
	Request	r;
	r.fMethod = L"POST";
	r.fOverrideHeaders = extraHeaders;
	r.fData = data;
	r.fContentType = contentType;
	return SendAndRequest (r);
}

Response	Connection::Delete (const map<String,String>& extraHeaders)
{
	///// TODO:  ADD HTTP::METHODS module (like with Headers) to enumerate common/basic METHODS GET/POST/ETC...
	Request	r;
	r.fMethod = L"DELETE";
	r.fOverrideHeaders = extraHeaders;
	return SendAndRequest (r);
}

Response	Connection::Put (const vector<Byte>& data, const InternetMediaType& contentType, const map<String,String>& extraHeaders)
{
	///// TODO:  ADD HTTP::METHODS module (like with Headers) to enumerate common/basic METHODS GET/POST/ETC...
	Request	r;
	r.fMethod = L"POST";
	r.fOverrideHeaders = extraHeaders;
	r.fData = data;
	r.fContentType = contentType;
	return SendAndRequest (r);
}


