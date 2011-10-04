/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>

#include	"../../Foundation/Containers/Common.h"
#include	"../../Foundation/DataExchangeFormat/BadFormatException.h"
#include	"../../Foundation/Debug/Assertions.h"
#include	"../../Foundation/Execution/Exceptions.h"
#include	"../../Foundation/Memory/SmallStackBuffer.h"

#include	"HTTPConnection.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Memory;

using	namespace	Stroika::Framework;
using	namespace	Stroika::Framework::Service;





HTTPConnection::HTTPConnection (Socket s)
	: fSocketStream_ (s)
{
}


HTTPConnection::HTTPConnection (SocketStream s)
	: fSocketStream_ (s)
{
}





/*
 ********************************************************************************
 ************************************ Service::Main *****************************
 ********************************************************************************
 */
