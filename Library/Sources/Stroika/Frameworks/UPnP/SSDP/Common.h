/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Common_h_
#define _Stroika_Frameworks_UPnP_SSDP_Common_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String.h"
#include    "../../../Foundation/Configuration/Common.h"
#include    "../../../Foundation/IO/Network/SocketAddress.h"


/**
 *  \file
 *
 *  See http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
 *  for details on the SSDP specification.
 *
 *  And http://www.upnp-hacks.org/upnp.html for more hints.
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace   SSDP {


                namespace V4 {
                    extern  const       Foundation::IO::Network::SocketAddress  kSocketAddress;
                }
                namespace V6 {
                    // NYI
                }


            }
        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Common.inl"

#endif  /*_Stroika_Frameworks_UPnP_SSDP_Common_h_*/
