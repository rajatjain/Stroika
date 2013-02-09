#include    "Stroika/Frameworks/StroikaPreComp.h"

#if     qPlatform_Windows
#include    <winsock2.h>
#endif

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/Event.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Frameworks/UPnP/SSDP/Client/Listener.h"
#include    "Stroika/Frameworks/UPnP/SSDP/Client/Search.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Frameworks::UPnP;
using   namespace Stroika::Frameworks::UPnP::SSDP;

using   Characters::String;
using   Memory::Optional;

using Client::Listener;

namespace {
    void    DoListening_ (Listener* l)
    {
        cout << "Listening..." << endl;
        l->AddOnFoundCallback ([] (const Listener::Result & d) {
            static  mutex       m;
            lock_guard<mutex> critSection (m);
            cout << "\tFound device:" << endl;
            cout << "\t\tUSN:      " << d.fUSN.AsUTF8 () << endl;
            cout << "\t\tLocation: " << d.fLocation.AsUTF8 () << endl;
            cout << "\t\tST:       " << d.fST.AsUTF8 () << endl;
            if (not d.fServer.empty ()) {
                cout << "\t\tServer:   " << d.fServer.AsUTF8 () << endl;
            }
            cout << endl;
        });
        l->Start ();
    }
}

namespace {
    void    DoSearching_ (const String& searchFor)
    {
        cout << "Searching for " << searchFor.AsUTF8 () << "..." << endl;
    }
}


int main(int argc, const char* argv[])
{
    bool    listen  =   false;
    Optional<String>    searchFor;

for (String arg : Execution::ParseCommandLine (argc, argv)) {
        if (Execution::MatchesCommandLineArgument (arg, L"l")) {
            listen = true;
        }
    }

#if qPlatform_Windows
    {
        // Initialize Winsock
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            cerr << "WSAStartup failed " << iResult << endl;
            return 1;
        }
    }
#endif

    // for now - given current API (and not using threads) - do just one or the other
    Listener l;
    if (listen) {
        DoListening_ (&l);
    }
    else if (not searchFor.empty ()) {
        DoSearching_ (*searchFor);
    }

    Execution::Event ().Wait ();    // wait forever - til user hits ctrl-c

    return 0;
}
