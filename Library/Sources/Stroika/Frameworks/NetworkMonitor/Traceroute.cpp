/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <random>

#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Collection.h"
#include "../../Foundation/Debug/Trace.h"
#include "../../Foundation/Execution/Sleep.h"
#include "../../Foundation/IO/Network/Listener.h"
#include "../../Foundation/IO/Network/Socket.h"

#include "Traceroute.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::InternetProtocol;
using namespace Stroika::Foundation::IO::Network::InternetProtocol::ICMP;
using namespace Stroika::Foundation::IO::Network::InternetProtocol::IP;
using namespace Stroika::Foundation::Traversal;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::NetworkMontior;
using namespace Stroika::Frameworks::NetworkMontior::Traceroute;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    const Options::SampleInfo kDefaultSampleInfo_{Duration (0), 1};
}

/*
 ********************************************************************************
 ************ NetworkMontior::Traceroute::Options::SampleInfo *******************
 ********************************************************************************
 */
Characters::String Traceroute::Options::SampleInfo::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Interval: " + Characters::ToString (fInterval) + L", ";
    sb += L"Count: " + Characters::Format (L"%d", fSampleCount) + L", ";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ***************** NetworkMontior::Traceroute::Options **************************
 ********************************************************************************
 */
constexpr Traversal::Range<size_t> Traceroute::Options::kAllowedICMPPayloadSizeRange;

const Duration Traceroute::Options::kDefaultTimeout{1.0};

String Traceroute::Options::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    if (fMaxHops) {
        sb += L"Max-Hops: " + Characters::Format (L"%d", *fMaxHops) + L", ";
    }
    if (fTimeout) {
        sb += L"Timeout: " + Characters::ToString (*fTimeout) + L", ";
    }
    if (fPacketPayloadSize) {
        sb += L"Packet-Payload-Size: " + Characters::Format (L"%d", *fPacketPayloadSize) + L", ";
    }
    if (fSampleInfo) {
        sb += L"Sample: " + Characters::ToString (*fSampleInfo) + L", ";
    }
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ******************** NetworkMontior::Traceroute::Hop ***************************
 ********************************************************************************
 */
String Hop::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Time: " + Characters::ToString (fTime) + L", ";
    sb += L"Address: " + Characters::ToString (fAddress) + L", ";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 *********************** NetworkMontior::Traceroute *****************************
 ********************************************************************************
 */
Sequence<Hop> NetworkMontior::Traceroute::Run (const InternetAddress& addr, const Options& options)
{
    Sequence<Hop> result;

    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Frameworks::NetworkMontior::Traceroute::Run", L"addr=%s, options=%s", Characters::ToString (addr).c_str (), Characters::ToString (options).c_str ())};
    size_t                    icmpPacketSize = Options::kAllowedICMPPayloadSizeRange.Pin (options.fPacketPayloadSize.Value (Options::kDefaultPayloadSize)) + sizeof (ICMP::PacketHeader);
    unsigned int              ttl            = options.fMaxHops.Value (Options::kDefaultMaxHops);

    Time::DurationSecondsType pingTimeout = options.fTimeout.Value (Options::kDefaultTimeout).As<Time::DurationSecondsType> ();

    Options::SampleInfo sampleInfo = options.fSampleInfo.Value (kDefaultSampleInfo_);
    Require (sampleInfo.fSampleCount >= 1);

    static std::mt19937    rng{std::random_device () ()};
    SmallStackBuffer<Byte> sendPacket (icmpPacketSize);
    // use random data as a payload
    static std::uniform_int_distribution<std::mt19937::result_type> distByte (0, numeric_limits<Byte>::max ());
    for (Byte* p = (Byte*)sendPacket.begin () + sizeof (ICMP::PacketHeader); p < sendPacket.end (); ++p) {
        static std::uniform_int_distribution<std::mt19937::result_type> distribution (0, numeric_limits<Byte>::max ());
        *p = distribution (rng);
    }

    /*
    *  General theorey of operation:
    *      >   Use ICMP (low level IP raw packets) to send a packet with a time-to-live (max # of hops) to the target address.
    *      >   That target address - if ICMP enabled properly - will send an ECHO reply back.
    *      >   we can look at how long this took, and report back the difference.
    *
    *  Also, allow for sampling differntly sized packets, etc.
    */

    Socket s{Socket::ProtocolFamily::INET, Socket::SocketKind::RAW, IPPROTO_ICMP};
    s.setsockopt (IPPROTO_IP, IP_TTL, ttl); // max # of hops

    Collection<DurationSecondsType> sampleTimes;
    Collection<unsigned int>        sampleHopCounts;
    unsigned int                    samplesTaken{};
    while (samplesTaken < sampleInfo.fSampleCount) {
        if (sampleInfo.fSampleCount != 0) {
            Execution::Sleep (sampleInfo.fInterval);
        }
        try {
            ICMP::PacketHeader pingRequest = [&]() {
                static std::uniform_int_distribution<std::mt19937::result_type> distribution (0, numeric_limits<uint16_t>::max ());
                static uint16_t                                                 seq_no = distribution (rng);
                ICMP::PacketHeader                                              tmp{};
                tmp.type      = ICMP_ECHO_REQUEST;
                tmp.id        = distribution (rng);
                tmp.seq       = seq_no++;
                tmp.timestamp = static_cast<uint32_t> (Time::GetTickCount () * 1000);
                return tmp;
            }();
            memcpy (sendPacket.begin (), &pingRequest, sizeof (pingRequest));
            reinterpret_cast<ICMP::PacketHeader*> (sendPacket.begin ())->checksum = ip_checksum (sendPacket.begin (), sendPacket.begin () + icmpPacketSize);
            s.SendTo (sendPacket.begin (), sendPacket.end (), SocketAddress{addr, 0});

            // Find first packet responding
            while (true) {
                using IO::Network::InternetProtocol::IP::PacketHeader;
                SocketAddress fromAddress;

                SmallStackBuffer<Byte> recv_buf (icmpPacketSize + sizeof (PacketHeader));
                size_t                 n = s.ReceiveFrom (begin (recv_buf), end (recv_buf), 0, &fromAddress, pingTimeout);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"got back packet from %s", Characters::ToString (fromAddress).c_str ());
#endif
                const PacketHeader* reply = reinterpret_cast<const PacketHeader*> (recv_buf.begin ());

                {
                    // Skip ahead to the ICMP header within the IP packet
                    unsigned short            header_len = reply->ihl * 4;
                    const ICMP::PacketHeader* icmphdr    = (const ICMP::PacketHeader*)((const Byte*)reply + header_len);

                    // Make sure the reply is sane
                    if (n < header_len + ICMP_MIN) {
                        Execution::Throw (Execution::StringException (L"too few bytes from " + Characters::ToString (fromAddress))); // draft @todo fix
                    }
                    else if (icmphdr->id != pingRequest.id) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"icmphdr->id != pingRequest.id so ignoring this reply");
#endif
                        // Must be a reply for another pinger running locally, so just
                        // ignore it.
                        continue;
                    }
                    switch (icmphdr->type) {
                        case ICMP_ECHO_REPLY: {
                            // Different operating systems use different starting values for TTL. TTL here is the original number used,
                            // less the number of hops. So we are left with making an educated guess. Need refrence and would be nice to find better
                            // way, but this seems to work pretty often.
                            unsigned int nHops{};
                            if (reply->ttl > 128) {
                                nHops = 256 - reply->ttl;
                            }
                            else if (reply->ttl > 64) {
                                nHops = 128 - reply->ttl;
                            }
                            else {
                                nHops = 65 - reply->ttl;
                            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                            DbgTrace (L"reply->ttl = %d, nHops = %d", reply->ttl, nHops);
#endif

                            sampleTimes += (Time::GetTickCount () * 1000 - icmphdr->timestamp) / 1000;
                            sampleHopCounts += nHops;
                            samplesTaken++;
                            goto nextSample;
                        }
                        case ICMP_TTL_EXPIRE: {
                            Execution::Throw (ICMP::TTLExpiredException ());
                        }
                        case ICMP_DEST_UNREACH: {
                            Execution::Throw (Network::InternetProtocol::ICMP::DestinationUnreachableException (icmphdr->code));
                        };
                        default: {
                            Execution::Throw (Network::InternetProtocol::ICMP::UnknownICMPPacket (icmphdr->type));
                        }
                    }
                }
            }
        }
        catch (...) {
            bool suppressThrows = sampleInfo.fSampleCount > 1;
            if (suppressThrows) {
                DbgTrace (L"ignore exception %s", Characters::ToString (current_exception ()).c_str ());
            }
            else {
                Execution::ReThrow ();
            }
            samplesTaken++;
        }
    nextSample:;
    }

    Assert (sampleTimes.empty () == sampleHopCounts.empty ());
#if 0
    if (sampleTimes.empty ()) {
        return Results{ {},{}, samplesTaken };
    }
    else {
        return Results{ Duration (*sampleTimes.Median ()), *sampleHopCounts.Median (), static_cast<unsigned int> (samplesTaken - sampleTimes.size ()) };
    }
#endif
    return result;
}
