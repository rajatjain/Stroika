/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_ConnectionManager_h_
#define _Stroika_Framework_WebServer_ConnectionManager_h_   1

#include    "../StroikaPreComp.h"

#include    <list>
#include    <memory>

#include    "../../Foundation/Execution/Synchronized.h"
#include    "../../Foundation/Execution/ThreadPool.h"
#include    "../../Foundation/IO/Network/SocketAddress.h"
#include    "../../Foundation/IO/Network/Listener.h"

#include    "Request.h"
#include    "Response.h"
#include    "Router.h"
#include    "Connection.h"



/*
 * TODO:
 *
 *      o   First get basics working
 *
 *      o   Handle thread safety (very tricky - watch adding connection handlers!)
 *
 *      o   Do a single background thread first to run handlers.
 *
 *      o   THen do threadpooling.
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   IO::Network::SocketAddress;
            using   std::shared_ptr;


            /**
             *  This class is a useful helper for managing a set of connections. You can start it and stop it
             *  (it maintains internal threads). And you can hand it Connections, along with a set of handlers,
             *  and it will monitor the connections, and when any is ready with more input, it will assign the
             *  appropriate handler to handle the request, and produce the response.
             *
             *  This doesn't CURRENTLY support keepalives.
             *  This doesn't CURRENTLY (really) support a threadpool (it has one but just puts one thread inside).
             */
            class   ConnectionManager {
            public:
                ConnectionManager (const SocketAddress& bindAddress, const Router& router);
                ConnectionManager (const ConnectionManager&) = delete;
                ~ConnectionManager () = default;


            private:
                Optional<String>    fServerHeader_;
            public:
                Optional<String> GetServerHeader () const { return fServerHeader_; }
                void SetServerHeader (Optional<String> server) {  fServerHeader_ = server; }


            private:
                bool    fIgnoreSillyCORS_;
            public:
                bool GetIgnoreCORS () const { return fIgnoreSillyCORS_; }
                void SetIgnoreCORS (bool ignoreCORS)  {  fIgnoreSillyCORS_ = ignoreCORS; }



            private:
                void onConnect_ (Socket s);
                Router  fRouter_;
                IO::Network::Listener fListener_;

            public:
                nonvirtual  const ConnectionManager& operator= (const ConnectionManager&) = delete;

#if 0
            public:
                nonvirtual  void    Start ();
                nonvirtual  void    Abort ();
                nonvirtual  void    WaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);
                nonvirtual  void    AbortAndWaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);
#endif
#if 0
            public:
                nonvirtual  void    AddHandler (const shared_ptr<RequestHandler>& h);
                nonvirtual  void    RemoveHandler (const shared_ptr<RequestHandler>& h);
#endif

            public:
                nonvirtual  void    AddConnection (const shared_ptr<Connection>& conn);
                nonvirtual  void    AbortConnection (const shared_ptr<Connection>& conn);

            public:
                /**
                 *  We need some sort of status flag on connections - saying of they are OPEN or not - or done.
                 *  But this will return just those which are not 'done'. Of course - due to asyncrhony,
                 *  by the time one looks at the list, some may already be done.
                 */
                nonvirtual  vector<shared_ptr<Connection>> GetConnections () const;

#if 0
            private:
                //VERY VERY SUPER PRIMITIVE FIFRST DRAFT OF CONNECTION HANDLING
                nonvirtual  void    DoMainConnectionLoop_ ();
                nonvirtual  void    DoOneConnection_ (shared_ptr<Connection> c);
#endif

            private:
                // REALLY could use Stroika threadsafe lists here!!! - so could just iterate and forget!
//                Execution::Synchronized<list<shared_ptr<RequestHandler>>>   fHandlers_;
                Execution::Synchronized<list<shared_ptr<Connection>>>       fActiveConnections_;

                // we may eventually want two thread pools - one for managing bookkeeping/monitoring harvests, and one for actually handling
                // connections. Or maybe a single thread for the bookkeeping, and the pool for handling ongoing connections?
                //
                // But for now - KISS
                //
                // Note - for now - we dont even handle servicing connections in the threadpool!!! - just one thread
                Execution::ThreadPool                                       fThreads_;
            };

        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ConnectionManager.inl"

#endif  /*_Stroika_Framework_WebServer_ConnectionManager_h_*/
