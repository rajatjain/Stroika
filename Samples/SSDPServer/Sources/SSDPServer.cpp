/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>
#include <mutex>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String_Constant.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/LinkMonitor.h"
#include "Stroika/Foundation/IO/Network/Listener.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Frameworks/UPnP/SSDP/Common.h"
#include "Stroika/Frameworks/UPnP/SSDP/Server/BasicServer.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::WebServer;

using Characters::String;
using Characters::String_Constant;
using Containers::Sequence;
using Memory::Optional;
using Server::BasicServer;

namespace {
    struct WebServerForDeviceDescription_ {

        WebServerForDeviceDescription_ (uint16_t webServerPortNumber, const Device& d, const DeviceDescription& dd)
            : fListener ()
        {
            auto onConnect = [d, dd](Socket s) {
                Execution::Thread runConnectionOnAnotherThread ([s, d, dd]() {
                    // If the URLs are served locally, you may want to update the URL based on
                    // IO::Network::GetPrimaryInternetAddress ()
                    Memory::BLOB deviceDescription = Stroika::Frameworks::UPnP::Serialize (d, dd);
                    // now read
                    Connection conn (s);
                    conn.ReadHeaders (); // bad API. Must rethink...
                    conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kServer, L"stroika-ssdp-server-demo");
                    conn.GetResponse ().write (deviceDescription.begin (), deviceDescription.end ());
                    conn.GetResponse ().SetContentType (DataExchange::PredefinedInternetMediaType::XML_CT ());
                    conn.GetResponse ().End ();
                });
                runConnectionOnAnotherThread.SetThreadName (L"Connection Thread"); // SHOULD use a fancier name (connection#)
                runConnectionOnAnotherThread.Start ();
                //runConnectionOnAnotherThread.WaitForDone ();    // maybe save these in connection mgr so we can force them all shut down...
            };
            fListener = Optional<Listener> (Listener (SocketAddress (Network::V4::kAddrAny, webServerPortNumber), onConnect));
        }

        Optional<Listener> fListener;
    };
}

int main (int argc, const char* argv[])
{
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    try {
        uint16_t portForOurWS = 8080;

        Device d;
        d.fLocation.SetPortNumber (portForOurWS);
        d.fServer   = UPnP::SSDP::MakeServerHeaderValue (String_Constant (L"MyStroikaBasedSampleProduct/1.0"));
        d.fDeviceID = UPnP::MungePrimaryMacAddrIntoBaseDeviceID (String_Constant (L"315CAAE0-1335-57BF-A178-24C9EE756627"));

        DeviceDescription deviceInfo;
        deviceInfo.fPresentationURL  = URL::Parse (String_Constant (L"http://www.sophists.com/"));
        deviceInfo.fDeviceType       = String_Constant (L"urn:sophists.com:device:deviceType:1.0");
        deviceInfo.fManufactureName  = String_Constant (L"Sophist Solutions, Inc.");
        deviceInfo.fFriendlyName     = String_Constant (L"Sophist Solutions fake device");
        deviceInfo.fManufacturingURL = URL::Parse (String_Constant (L"http://www.sophists.com/"));
        deviceInfo.fModelDescription = String_Constant (L"long user-friendly title");
        deviceInfo.fModelName        = String_Constant (L"model name");
        deviceInfo.fModelNumber      = String_Constant (L"model number");
        deviceInfo.fModelURL         = URL::Parse (String_Constant (L"http://www.sophists.com/"));
        deviceInfo.fSerialNumber     = String_Constant (L"manufacturer's serial number");

        WebServerForDeviceDescription_ deviceWS (portForOurWS, d, deviceInfo);
        BasicServer                    b (d, deviceInfo, BasicServer::FrequencyInfo ());
        Execution::WaitableEvent (Execution::WaitableEvent::eAutoReset).Wait (); // wait forever - til user hits ctrl-c
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        cerr << "Exception - " << exceptMsg.AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
