/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_ObjectVariantMapper_inl_
#define _Stroika_Framework_WebService_Server_ObjectVariantMapper_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Basic.h"
#if 0
namespace std {
    namespace detail {
        template <class F, class Tuple, std::size_t... I>
        constexpr decltype (auto) apply_impl (F&& f, Tuple&& t, std::index_sequence<I...>)
        {
            return std::invoke (std::forward<F> (f), std::get<I> (std::forward<Tuple> (t))...);
        }
    } // namespace detail

    template <class F, class Tuple>
    constexpr decltype (auto) apply (F&& f, Tuple&& t)
    {
        return detail::apply_impl (
            std::forward<F> (f), std::forward<Tuple> (t),
            std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
    }
}
#endif

namespace Stroika {
    namespace Frameworks {
        namespace WebService {
            namespace Server {
                namespace VariantValue {

/*
                     ********************************************************************************
                     **************** WebService::Server::VariantValue::mkRequestHandler ************
                     ********************************************************************************
                     */
#if 0
                    namespace {
                        template <typename T, typename... REST_IN_ARGS>
                        auto ConvertVariantValuesToTypes_ (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, T, REST_IN_ARGS... args)
                        {
                            Require (vvs.size () == (sizeof...(args)) + 1);
                            return tuple_cat (tuple<T>{objVarMapper.ToObject<T> (vvs.Nth (0))}, ConvertVariantValuesToTypes_ (objVarMapper, vvs.Skip (1), std::forward<REST_IN_ARGS> (args)...));
                        }
                        auto ConvertVariantValuesToTypes_ (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs)
                        {
                            Require (vvs.size () == 0);
                            return tuple_cat ();
                        }
                    }
                    namespace {
                        template < typename T, typename... Ts >
                        auto head (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, std::tuple<T, Ts...> t)
                        {
                            return  std::get<0> (t);
                        }
                        template < std::size_t... Ns, typename... Ts >
                        auto tail_impl (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, std::index_sequence<Ns...>, std::tuple<Ts...> t)
                        {
                            return  std::make_tuple (std::get<Ns + 1u> (t)...);
                        }
                        template < typename... Ts >
                        auto tail (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, std::tuple<Ts...> t)
                        {
                            return  tail_impl (std::make_index_sequence<sizeof...(Ts)-1u> (), t);
                        }
                        template <typename T, typename... REST_IN_ARGS>
                        auto ConvertVariantValuesToTypes_x (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, T, std::tuple<T, REST_IN_ARGS...> args)
                        {
                            //WRequire (vvs.size () == (sizeof...(args)) + 1);
                            return tuple_cat (tuple<T>{objVarMapper.ToObject<T> (vvs.Nth (0))}, ConvertVariantValuesToTypes_x (vvs.Skip (1), std::forward<REST_IN_ARGS> (args)...));
                        }
                        auto ConvertVariantValuesToTypes_x (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, std::tuple<> a)
                        {
                            Require (vvs.size () == 0);
                            return tuple_cat ();
                        }
                    }
                    namespace {
                        template <typename OUT_ARG, typename... Args>
                        struct save_it_for_later {

                            DataExchange::ObjectVariantMapper objVarMapper;
                            Sequence<VariantValue>            vvs;

                            const function<OUT_ARG (Args...)>& f;

                            std::tuple<Args...> params;


                            template <std::size_t... I>
                            OUT_ARG call_func (std::index_sequence<I...>)
                            {
                                //return f (ConvertVariantValuesToTypes_ (objVarMapper, vvs, std::get<I> (params)...));
                            //  using T = decltype (std::get<I> (params)...);
                                //using T = tuple_element_t<I, std::tuple<Args...>>;
                                return f (objVarMapper.ToObject<tuple_element_t<I..., decltype(params)>> (vvs.Nth (I))...);
                            }
                            OUT_ARG delayed_dispatch ()
                            {
                                return call_func (std::index_sequence_for<Args...>{});
                            }
                        };
                    }
                    template <typename OUT_ARG, typename... IN_ARGS>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARG (IN_ARGS...)>& f)
                    {
                        return [=](WebServer::Message* m) {
                            using namespace Containers;
                            ExpectedMethod (m->PeekRequest (), webServiceDescription);
                            VariantValue allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
                            Mapping<String, VariantValue> allArgsMap = allArgsAsVariantValue.As<Mapping<String, VariantValue>> ();
                            Sequence<VariantValue> vvs;
                            for (auto i : paramNames) {
                                vvs += allArgsMap.LookupValue (i);
                            }
                            //  tuple<IN_ARGS...> junk;

                            save_it_for_later<OUT_ARG, IN_ARGS...> aaa{objVarMapper, vvs, f};

                            //tuple<IN_ARGS...> args2Forward = ConvertVariantValuesToTypes_ (objVarMapper, vvs, std::forward<IN_ARGS...> (junk)...);
                            //tuple<IN_ARGS...> args2Forward = std::apply (ConvertVariantValuesToTypes_, tuple_cat (objVarMapper, vvs, std::forward<IN_ARGS> (junk)...));
                            //OUT_ARG           response2Send = f (std::forward<IN_ARGS> (args2Forward)...);
                            OUT_ARG response2Send = aaa.delayed_dispatch ();
                            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (response2Send));
                        };
                    }
                    template <typename OUT_ARGS, typename IN_ARGS>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARGS (IN_ARGS)>& f)
                    {
                        return [=](WebServer::Message* m) {
                            ExpectedMethod (m->PeekRequest (), webServiceDescription);
                            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f (objVarMapper.ToObject<IN_ARGS> (GetWebServiceArgsAsVariantValue (m->PeekRequest (), {})))));
                        };
                    }
#endif

                    // WORKAROUND FACT I CANNOT GET VARIADIC TEMPLATES WORKING...
                    template <typename OUT_ARGS, typename ARG_TYPE_0>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0)>& f)
                    {
                        using namespace Containers;
                        Require (paramNames.size () == 1);
                        return [=](WebServer::Message* m) {
                            ExpectedMethod (m->PeekRequest (), webServiceDescription);
                            VariantValue allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
                            Mapping<String, VariantValue> allArgsMap = allArgsAsVariantValue.As<Mapping<String, VariantValue>> ();
                            Sequence<VariantValue> vvs;
                            for (auto i : paramNames) {
                                vvs += allArgsMap.LookupValue (i);
                            }
                            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f (objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]))));
                        };
                    }
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1)>& f)
                    {
                        using namespace Containers;
                        Require (paramNames.size () == 2);
                        return [=](WebServer::Message* m) {
                            ExpectedMethod (m->PeekRequest (), webServiceDescription);
                            VariantValue allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
                            Mapping<String, VariantValue> allArgsMap = allArgsAsVariantValue.As<Mapping<String, VariantValue>> ();
                            Sequence<VariantValue> vvs;
                            for (auto i : paramNames) {
                                vvs += allArgsMap.LookupValue (i);
                            }
                            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f (objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]), objVarMapper.ToObject<ARG_TYPE_1> (vvs[1]))));
                        };
                    }
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2)>& f)
                    {
                        using namespace Containers;
                        Require (paramNames.size () == 3);
                        return [=](WebServer::Message* m) {
                            ExpectedMethod (m->PeekRequest (), webServiceDescription);
                            VariantValue allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
                            Mapping<String, VariantValue> allArgsMap = allArgsAsVariantValue.As<Mapping<String, VariantValue>> ();
                            Sequence<VariantValue> vvs;
                            for (auto i : paramNames) {
                                vvs += allArgsMap.LookupValue (i);
                            }
                            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f (objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]), objVarMapper.ToObject<ARG_TYPE_1> (vvs[1]), objVarMapper.ToObject<ARG_TYPE_2> (vvs[2]))));
                        };
                    }
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2, typename ARG_TYPE_3>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2, ARG_TYPE_3)>& f)
                    {
                        using namespace Containers;
                        Require (paramNames.size () == 4);
                        return [=](WebServer::Message* m) {
                            ExpectedMethod (m->PeekRequest (), webServiceDescription);
                            VariantValue allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
                            Mapping<String, VariantValue> allArgsMap = allArgsAsVariantValue.As<Mapping<String, VariantValue>> ();
                            Sequence<VariantValue> vvs;
                            for (auto i : paramNames) {
                                vvs += allArgsMap.LookupValue (i);
                            }
                            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f (objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]), objVarMapper.ToObject<ARG_TYPE_1> (vvs[1]), objVarMapper.ToObject<ARG_TYPE_2> (vvs[2]), objVarMapper.ToObject<ARG_TYPE_3> (vvs[3]))));
                        };
                    }

#if 0
                    template <typename OUT_ARGS, typename IN_ARGS>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARGS (IN_ARGS)>& f)
                    {
                        return [=](WebServer::Message* m) {
                            ExpectedMethod (m->PeekRequest (), webServiceDescription);
                            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f (objVarMapper.ToObject<IN_ARGS> (GetWebServiceArgsAsVariantValue (m->PeekRequest (), {})))));
                        };
                    }
#endif
                    template <typename OUT_ARGS>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARGS (void)>& f)
                    {
                        return [=](WebServer::Message* m) {
                            ExpectedMethod (m->PeekRequest (), webServiceDescription);
                            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f ()));
                        };
                    }
                }
            }
        }
    }
}
#endif /*_Stroika_Framework_WebService_Server_ObjectVariantMapper_inl_*/
