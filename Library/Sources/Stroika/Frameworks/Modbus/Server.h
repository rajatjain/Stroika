/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Modbus_Server_h_
#define _Stroika_Frameworks_Modbus_Server_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Execution/Logger.h"
#include    "../../Foundation/Execution/Thread.h"
#include    "../../Foundation/Execution/ThreadPool.h"
#include    "../../Foundation/IO/Network/Socket.h"
#include    "../../Foundation/Memory/Optional.h"

#include    "IModbusService.h"



/**
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   Modbus {


            using   namespace   Stroika::Foundation;


            /**
             */
            struct  ServerOptions {
                /**
                 *  By spec, defaults to 502
                 */
                Memory::Optional<uint16_t>                          fListenPort;

                /**
                 *  Logger to write interesting messages to.
                 */
                Memory::Optional<Execution::Logger*>                fLogger;

                /**
                 *  Often helpful to specify reUseAddr = true, to avoid trouble restarting service
                 */
                Memory::Optional<IO::Network::Socket::BindFlags>    fBindFlags;

                /**
                 *  To specify size, provide your own threadpool
                 */
                shared_ptr<Execution::ThreadPool>                   fThreadPool;

                /**
                 *  defaults to true iff argument fThreadPool null.
                 *
                 *  \note   Either let this class or caller must shutdown threadpool before exiting app.
                 */
                Memory::Optional<bool>                              fShutdownThreadPool;
            };

            /**
             *  Construct a Modbus TCP Listener which will listen for Modbus connections, run them using
             *  the optionally provided thread pool (and other configuration options) and send actual handler
             *  requests to the argument IModbusService handler.
             *
             *  \req serviceHandler != nullptr
             */
            Execution::Thread   MakeModbusTCPServerThread (const shared_ptr<IModbusService>& serviceHandler, const ServerOptions& options = ServerOptions {});

            namespace {
                template    <typename MODBUS_REGISTER_DESCRIPTOR, typename SRC_TYPE>
                void    SplitSrcAcrossOutputs (const SRC_TYPE& s, typename MODBUS_REGISTER_DESCRIPTOR::NameType baseRegister, Containers::Mapping<typename MODBUS_REGISTER_DESCRIPTOR::NameType, typename MODBUS_REGISTER_DESCRIPTOR::ValueType>* update)
                {
                    using   VT = typename MODBUS_REGISTER_DESCRIPTOR::ValueType;
                    constexpr   size_t  kNElts_  =   sizeof (SRC_TYPE) / sizeof (VT);
                    static_assert (kNElts_ * sizeof (VT) == sizeof (SRC_TYPE), ""); // only works if divides evenly
                    RequireNotNull (update);
                    const VT* arr = reinterpret_cast<const VT*> (&s);
                    for (size_t i = 0; i < kNElts_; ++i) {
                        // high order addressed elements first - big endian
                        update->Add (baseRegister + i - 1, arr[kNElts_ -  i - 1]);
                    }
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
#include    "Server.inl"

#endif  /*_Stroika_Frameworks_Modbus_Server_h_*/
