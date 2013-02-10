/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "Common.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP::SSDP;


namespace {
    constexpr   char		SSDP_MULTICAST[]    =      "239.255.255.250";
    constexpr   uint16_t    SSDP_PORT           =       1900;
}


const	SocketAddress   UPnP::SSDP::V4::kSocketAddress =   SocketAddress (InternetAddress (SSDP_MULTICAST, InternetAddress::AddressFamily::V4), SSDP_PORT);

