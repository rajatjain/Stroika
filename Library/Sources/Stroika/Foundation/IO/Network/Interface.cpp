/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <cstdio>

#if     qPlatform_POSIX
#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/socket.h>
#include    <sys/ioctl.h>
#include    <net/if.h>
#include    <netinet/in.h>
#include    <netdb.h>
#include    <arpa/inet.h>
#include    <netinet/in.h>
#include    <linux/netlink.h>
#include    <linux/rtnetlink.h>
#include    <linux/ethtool.h>
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <WS2tcpip.h>
#include    <Iphlpapi.h>
#include    <netioapi.h>
#endif

#include    "../../Characters/CString/Utilities.h"
#include    "../../Containers/Collection.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Finally.h"
#if     qPlatform_Windows
#include    "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include    "../../Memory/SmallStackBuffer.h"

#include    "Socket.h"

#include    "Interface.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;


#if     qPlatform_Windows
#pragma comment(lib, "iphlpapi.lib")
#endif





/*
 ********************************************************************************
 **************************** Network::Interface ********************************
 ********************************************************************************
 */
const Configuration::EnumNames<Interface::Status>   Interface::Stroika_Enum_Names(Status) = {
    { Interface::Status::eConnected, L"Connected" },
    { Interface::Status::eRunning, L"Running" },
};

const Configuration::EnumNames<Interface::Type>   Interface::Stroika_Enum_Names(Type) = {
    { Interface::Type::eLoopback, L"Loopback" },
    { Interface::Type::eWiredEthernet, L"WiredEthernet" },
    { Interface::Type::eWIFI, L"WIFI" },
    { Interface::Type::eOther, L"Other" },
};





/*
 ********************************************************************************
 ************************** Network::GetInterfaces ******************************
 ********************************************************************************
 */
Traversal::Iterable<Interface>  Network::GetInterfaces ()
{
    Collection<Interface>   result;
#if     qPlatform_POSIX
    auto getFlags = [] (int sd, const char* name) {
        char buf[1024];

        struct ifreq ifreq;
        memset(&ifreq, 0, sizeof (ifreq));
        strcpy (ifreq.ifr_name, name);

        int r = ioctl (sd, SIOCGIFFLAGS, (char*)&ifreq);
        Assert (r == 0);
        return ifreq.ifr_flags;
    };

    struct ifreq ifreqs[64];
    struct ifconf ifconf;
    memset (&ifconf, 0, sizeof(ifconf));
    ifconf.ifc_req = ifreqs;
    ifconf.ifc_len = sizeof(ifreqs);

    int sd = socket (PF_INET, SOCK_STREAM, 0);
    Assert (sd >= 0);
    Execution::Finally cleanup ([sd] () {
        close (sd);
    });

    int r = ioctl (sd, SIOCGIFCONF, (char*)&ifconf);
    Assert (r == 0);

    for (int i = 0; i < ifconf.ifc_len / sizeof(struct ifreq); ++i) {
        Interface   newInterface;
        newInterface.fInterfaceName = String::FromSDKString (ifreqs[i].ifr_name);
        int flags = getFlags (sd, ifreqs[i].ifr_name);

        if (flags & IFF_LOOPBACK) {
            newInterface.fType = Interface::Type::eLoopback;
        }
        else {
            // NYI
            newInterface.fType = Interface::Type::eWiredEthernet;    // WAY - not the right way to tell!
        }

        {
            auto getSpeed = [] (int sd, const char* name) -> Optional<double> {
                char buf[1024];

                struct ifreq ifreq;
                memset(&ifreq, 0, sizeof (ifreq));
                strcpy (ifreq.ifr_name, name);
                ifreq.ifr_data = &edata;

                edata.cmd = ETHTOOL_GSET;
                int r = ioctl(sock, SIOCETHTOOL, &ifreq);
                Assert (r == 0);
                constexpr double kMegabit_ = 1000 * 1000;
                switch (ethtool_cmd_speed(&edata))
                {
                    case SPEED_10:
                        return 10 * kMegabit_;
                    case SPEED_100:
                        preturn 100 * kMegabit_;
                    case SPEED_1000:
                        return 1000 * kMegabit_;
                    case SPEED_2500:
                        return 2500 * kMegabit_;
                    case SPEED_10000:
                        return 10000 * kMegabit_;
                    default:
                        return Optional<double> ();
                }
            };
            struct ethtool_cmd edata;
            ifr.ifr_data = &edata;
#if 0
            struct ifreq ifr;
            int rc;
            sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
            if (sock < 0) {
                perror("socket");
                exit(1);
            }
            strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name));
            ifr.ifr_data = &edata;

            edata.cmd = ETHTOOL_GSET;
            rc = ioctl(sock, SIOCETHTOOL, &ifr);
            if (rc < 0) {
                perror("ioctl");
                exit(1);
            }
            switch (ethtool_cmd_speed(&edata)) {
                case SPEED_10:
                    printf("10Mbps\n");
                    break;
                case SPEED_100:
                    printf("100Mbps\n");
                    break;
                case SPEED_1000:
                    printf("1Gbps\n");
                    break;
                case SPEED_2500:
                    printf("2.5Gbps\n");
                    break;
                case SPEED_10000:
                    printf("10Gbps\n");
                    break;
                default:
                    printf("Speed returned is %d\n", edata.speed);
            }
#endif
            fTransmitSpeedBaud = getSpeed (sd, ifreqs[i].ifr_name);
            fReceiveLinkSpeedBaud = fTransmitSpeedBaud;
        }

        {
            Containers::Set<Interface::Status>  status;
            if (flags & IFF_RUNNING) {
                // not right!!! But a start...
                status.Add (Interface::Status::eConnected);
                status.Add (Interface::Status::eRunning);
            }
            newInterface.fStatus = status;
        }
        newInterface.fBindings.Add (InternetAddress (((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr));    // @todo fix so works for ipv6 addresses as well!
        result.Add (newInterface);
    }
#elif   qPlatform_Windows
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_UNSPEC;       // Both IPv4 and IPv6 addresses
    Memory::SmallStackBuffer<Byte>  buf(0);
Again:
    ULONG ulOutBufLen = buf.GetSize ();
    PIP_ADAPTER_ADDRESSES   pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES> (buf.begin ());
    DWORD dwRetVal = ::GetAdaptersAddresses (family, flags, nullptr, pAddresses, &ulOutBufLen);
    if (dwRetVal == NO_ERROR) {
        for (PIP_ADAPTER_ADDRESSES currAddresses  = pAddresses; currAddresses != nullptr; currAddresses = currAddresses->Next) {
            Interface   newInterface;
            newInterface.fAdapterName = String::FromNarrowSDKString (currAddresses->AdapterName);
            newInterface.fFriendlyName = currAddresses->FriendlyName;
            newInterface.fDescription = currAddresses->Description;
            switch (currAddresses->IfType) {
                case    IF_TYPE_SOFTWARE_LOOPBACK:
                    newInterface.fType = Interface::Type::eLoopback;
                    break;
                case    IF_TYPE_IEEE80211:
                    newInterface.fType = Interface::Type::eWIFI;
                    break;
                case    IF_TYPE_ETHERNET_CSMACD:
                    newInterface.fType = Interface::Type::eWiredEthernet;
                    break;
                default:
                    newInterface.fType = Interface::Type::eOther;
                    break;
            }
            switch (currAddresses->OperStatus) {
                case    IfOperStatusUp:
                    newInterface.fStatus = Set<Interface::Status> ({Interface::Status::eConnected, Interface::Status::eRunning});
                    break;
                case    IfOperStatusDown:
                    newInterface.fStatus = Set<Interface::Status> ();
                    break;
                default:
                    // Dont know how to interpret the other status states
                    break;
            }
            for (PIP_ADAPTER_UNICAST_ADDRESS pu = currAddresses->FirstUnicastAddress; pu != nullptr; pu = pu->Next) {
                SocketAddress sa { pu->Address };
                if (sa.IsInternetAddress ()) {
                    newInterface.fBindings.Add (sa.GetInternetAddress ());
                }
            }
            for (PIP_ADAPTER_ANYCAST_ADDRESS pa = currAddresses->FirstAnycastAddress; pa != nullptr; pa = pa->Next) {
                SocketAddress sa { pa->Address };
                if (sa.IsInternetAddress ()) {
                    newInterface.fBindings.Add (sa.GetInternetAddress ());
                }
            }
            for (PIP_ADAPTER_MULTICAST_ADDRESS  pm = currAddresses->FirstMulticastAddress; pm != nullptr; pm = pm->Next) {
                SocketAddress sa { pm->Address };
                if (sa.IsInternetAddress ()) {
                    newInterface.fBindings.Add (sa.GetInternetAddress ());
                }
            }
#if     (NTDDI_VERSION >= NTDDI_LONGHORN)
            newInterface.fTransmitSpeedBaud = currAddresses->TransmitLinkSpeed;
            newInterface.fReceiveLinkSpeedBaud = currAddresses->ReceiveLinkSpeed;
#endif
            result.Add (newInterface);
        }
    }
    else if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
        buf.GrowToSize (ulOutBufLen);
        goto Again;
    }
    else if (dwRetVal == ERROR_NO_DATA) {
        DbgTrace ("There are no network adapters with IPv4 enabled on the local system");
    }
    else {
        Execution::Platform::Windows::Exception::DoThrow (dwRetVal);
    }
#else
    AssertNotImplemented ();
#endif
    return result;
}
