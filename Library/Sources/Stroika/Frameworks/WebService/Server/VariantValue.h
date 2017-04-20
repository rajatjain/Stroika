/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_VariantValue_h_
#define _Stroika_Framework_WebService_Server_VariantValue_h_ 1

#include "../../StroikaPreComp.h"

#include "../../../Foundation/Containers/Sequence.h"
#include "../../../Foundation/Containers/Set.h"
#include "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include "../../../Foundation/DataExchange/VariantValue.h"

#include "../../WebServer/Request.h"
#include "../../WebServer/RequestHandler.h"
#include "../../WebServer/Response.h"

#include "../Basic.h"

/*
 */

/*
 * TODO:
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 */

namespace Stroika {
    namespace Frameworks {
        namespace WebService {
            namespace Server {
                namespace VariantValue {

                    using namespace Stroika::Foundation;

                    using DataExchange::VariantValue;
                    using Memory::BLOB;
                    using Memory::Optional;

                    using WebServer::Request;
                    using WebServer::Response;

                    ////// SUPER DUPER ROUGH DRAFT

                    VariantValue GetWebServiceArgsAsVariantValue (Request* request, const Optional<String>& fromInMessage);

                    void WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const VariantValue& responseValue);

                    // @todo eventually find a way to make this owrk with JSON or XML in/ out and in can be GET query args (depending on WebServiceMethodDescription properties)
                    template <typename OUT_ARGS, typename IN_ARGS>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARGS (IN_ARGS)>& f);
                    template <typename OUT_ARGS>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARGS (void)>& f);

                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<BLOB (WebServer::Message* m)>& f);
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
#include "VariantValue.inl"

#endif /*_Stroika_Framework_WebService_Server_VariantValue_h_*/