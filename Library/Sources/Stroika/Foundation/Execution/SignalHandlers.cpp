/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Containers/Mapping.h"
#include    "../Debug/Trace.h"
#include    "../Containers/Concrete/Queue_Array.h"

#include    "BlockingQueue.h"
#include    "Thread.h"

#include    "SignalHandlers.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;

using   Containers::Mapping;
using   Containers::Queue;
using   Containers::Set;



// maybe useful while debugging signal code, but VERY unsafe
// and could easily be the source of bugs/deadlocks!
#ifndef qDoDbgTraceOnSignalHandlers_
#define qDoDbgTraceOnSignalHandlers_    0
#endif








namespace   {
    bool    IsSigIgnore_ (const Set<SignalHandler>& sigSet)
    {
        return sigSet.size () == 1 and sigSet.Contains (SignalHandlerRegistry::kIGNORED);
    }
}






/*
 ********************************************************************************
 *********** Execution::SignalHandlerRegistry::SafeSignalsManager ***************
 ********************************************************************************
 */
namespace {
    Queue<SignalID> mkQ_ ()
    {
        Containers::Concrete::Queue_Array<SignalID> signalQ;
        signalQ.SetCapacity (25);  // quite arbitrary - @todo make configurable somehow...
        return signalQ;
    }
}

struct SignalHandlerRegistry::SafeSignalsManager::Rep_ {
    Rep_ ()
        : fHandlers_ ()
        , fIncomingSafeSignals_ (mkQ_ ())
        , fBlockingQueuePusherThread_ ()
    {
        Thread watcherThread ([this] () {
            // This is a safe context
            Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::{}::fBlockingQueueDelegatorThread_"));
            while (true) {
                Debug::TraceContextBumper trcCtx1 (SDKSTR ("Waiting for next safe signal"));
                SignalID    i   =   fIncomingSafeSignals_.RemoveHead ();
                Debug::TraceContextBumper trcCtx2 (SDKSTR ("Invoking SAFE signal handlers"));
                DbgTrace (L"(signal: %s)", SignalToName (i).c_str ());
                for (SignalHandler sh : fHandlers_.LookupValue (i)) {
                    Assert (sh.GetType () == SignalHandler::Type::eSafe);
                    IgnoreExceptionsExceptThreadAbortForCall (sh (i));
                }
            }
        });
        watcherThread.SetThreadName (L"Signal Handler Safe Execution Thread");
        watcherThread.Start ();
        fBlockingQueuePusherThread_ = std::move (watcherThread);
    }
    ~Rep_ ()
    {
        Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::SignalHandlerRegistry::SafeSignalsManager::Rep_::~Rep_"));
        fBlockingQueuePusherThread_.AbortAndWaitForDone ();
    }
    Containers::Mapping<SignalID, Containers::Set<SignalHandler>>   fHandlers_;
    BlockingQueue<SignalID>                                         fIncomingSafeSignals_;
    Thread                                                          fBlockingQueuePusherThread_;

    void    NotifyOfArrivalOfPossiblySafeSignal (SignalID signal)
    {
        // harmless - but pointless - to add signals that will be ignored
        if (fHandlers_.Lookup (signal).IsPresent ()) {
            fIncomingSafeSignals_.AddTail (signal);
        }
    }
};






/*
 ********************************************************************************
 *********** Execution::SignalHandlerRegistry::SafeSignalsManager ***************
 ********************************************************************************
 */
shared_ptr<SignalHandlerRegistry::SafeSignalsManager::Rep_>  SignalHandlerRegistry::SafeSignalsManager::sTheRep_;

SignalHandlerRegistry::SafeSignalsManager::SafeSignalsManager ()
{
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::SignalHandlerRegistry::CTOR"));
    Assert (sTheRep_ == nullptr);
    sTheRep_ = shared_ptr<SignalHandlerRegistry::SafeSignalsManager::Rep_> (new Rep_ ());
}

SignalHandlerRegistry::SafeSignalsManager::~SafeSignalsManager ()
{
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::SignalHandlerRegistry::DTOR"));
    shared_ptr<SignalHandlerRegistry::SafeSignalsManager::Rep_> tmp = SignalHandlerRegistry::SafeSignalsManager::sTheRep_;
    sTheRep_.reset ();
    // avoid slight race - after reset above - could still be processing a signal (holding refcount above zero).
    // this check and abort/wait guaratess at least no more thread running (though other stuff - like signal handling - could
    // still be running)
    if (tmp != nullptr) {
        tmp->fBlockingQueuePusherThread_.AbortAndWaitForDone ();
    }
}






/*
 ********************************************************************************
 ******************** Execution::SignalHandlerRegistry **************************
 ********************************************************************************
 */
const   SignalHandler   SignalHandlerRegistry::kIGNORED =   SignalHandler (SIG_IGN, SignalHandler::Type::eDirect);

SignalHandlerRegistry&  SignalHandlerRegistry::Get ()
{
    static  SignalHandlerRegistry   sThe_;
    return sThe_;
}

SignalHandlerRegistry::SignalHandlerRegistry ()
    : fDirectHandlers_ ()
{
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::SignalHandlerRegistry::CTOR"));
}

SignalHandlerRegistry::~SignalHandlerRegistry ()
{
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::SignalHandlerRegistry::DTOR"));
    Assert (SafeSignalsManager::sTheRep_ == nullptr);  // must be cleared first
}

Set<SignalID>   SignalHandlerRegistry::GetHandledSignals () const
{
    // @todo redo using Mapping<>::Keys () when implemented...
    Set<SignalID>   result;
    for (auto i : fDirectHandlers_) {
        result.Add (i.fKey);
    }
    shared_ptr<SafeSignalsManager::Rep_> tmp = SafeSignalsManager::sTheRep_;
    if (tmp != nullptr) {
        for (auto i : tmp->fHandlers_) {
            result.Add (i.fKey);
        }
    }
    return result;
}

Set<SignalHandler>  SignalHandlerRegistry::GetSignalHandlers (SignalID signal) const
{
    Set<SignalHandler>  result  =   fDirectHandlers_.LookupValue (signal);
    shared_ptr<SafeSignalsManager::Rep_> tmp = SafeSignalsManager::sTheRep_;
    if (tmp != nullptr) {
        result += tmp->fHandlers_.LookupValue (signal);
    }
    return result;
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalID signal)
{
    SetSignalHandlers (signal, Set<SignalHandler> ());
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalID signal, SignalHandler handler)
{
    SetSignalHandlers (signal, Set<SignalHandler> ({handler}));
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalID signal, const Set<SignalHandler>& handlers)
{
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::SignalHandlerRegistry::{}::SetSignalHandlers"));
    DbgTrace (L"(signal = %s, handlers.size () = %d, ....)", SignalToName (signal).c_str (), handlers.size ());

    shared_ptr<SignalHandlerRegistry::SafeSignalsManager::Rep_> tmp = SignalHandlerRegistry::SafeSignalsManager::sTheRep_;
    if (handlers.empty ()) {
        /*
         *  No handlers means use default.
         */
        fDirectHandlers_.Remove (signal);
        if (tmp != nullptr) {
            tmp->fHandlers_.Remove (signal);
        }
        (void)::signal (signal, SIG_DFL);
    }
    else if (IsSigIgnore_ (handlers)) {
        Assert (handlers.size () == 1);
        fDirectHandlers_.Add (signal, handlers);
        if (tmp != nullptr) {
            tmp->fHandlers_.Remove (signal);
        }
        (void)::signal (signal, SIG_IGN);
    }
    else {
        Set<SignalHandler>  directHandlers;
        Set<SignalHandler>  safeHandlers;
        handlers.Apply ([&directHandlers, &safeHandlers] (SignalHandler si) {
            switch (si.GetType ()) {
                case SignalHandler::Type::eDirect: {
                        directHandlers.Add (si);
                    }
                    break;
                case SignalHandler::Type::eSafe: {
                        safeHandlers.Add (si);
                    }
                    break;
            }
        });
        Assert (directHandlers.size () + safeHandlers.size () == handlers.size ());
        fDirectHandlers_.Add (signal, directHandlers);
        if (not safeHandlers.empty ()) {
            // To use safe signal handlers, you must have a SignalHandlerRegistry::SafeSignalsManager
            // defined first. It is recommended that you define an instance of
            // SignalHandlerRegistry::SafeSignalsManager handler; should be defined in main ()
            Require (SafeSignalsManager::sTheRep_ != nullptr);
        }
        if (tmp != nullptr) {
            if (safeHandlers.empty ()) {
                tmp->fHandlers_.Remove (signal);
            }
            else {
                tmp->fHandlers_.Add (signal, safeHandlers);
            }
        }
        (void)::signal (signal, FirstPassSignalHandler_);
    }
}

void    SignalHandlerRegistry::AddSignalHandler (SignalID signal, SignalHandler handler)
{
    Set<SignalHandler>  s   =   GetSignalHandlers (signal);
    s.Add (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::RemoveSignalHandler (SignalID signal, SignalHandler handler)
{
    Set<SignalHandler>  s   =   GetSignalHandlers (signal);
    Require (s.Contains (handler));
    s.Remove (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::DefaultCrashSignalHandler (SignalID signal)
{
    DbgTrace (L"Serious Signal Error trapped: %s ... Aborting", SignalToName (signal).c_str ());
    abort ();
}

void    SignalHandlerRegistry::SetStandardCrashHandlerSignals (SignalHandler handler, const Set<SignalID>& excludedSignals)
{
    if (not excludedSignals.Contains (SIGINT))      {  SetSignalHandlers (SIGINT, handler);        }
    if (not excludedSignals.Contains (SIGILL))      {  SetSignalHandlers (SIGILL, handler);        }
    if (not excludedSignals.Contains (SIGFPE))      {  SetSignalHandlers (SIGFPE, handler);        }
    if (not excludedSignals.Contains (SIGSEGV))     {  SetSignalHandlers (SIGSEGV, handler);       }
    if (not excludedSignals.Contains (SIGTERM))     {  SetSignalHandlers (SIGTERM, handler);       }
#if     qPlatform_POSIX
    if (not excludedSignals.Contains (SIGSYS))      {  SetSignalHandlers (SIGSYS, handler);        }
    if (not excludedSignals.Contains (SIGBUS))      {  SetSignalHandlers (SIGBUS, handler);        }
    if (not excludedSignals.Contains (SIGQUIT))     {  SetSignalHandlers (SIGQUIT, handler);       }
    if (not excludedSignals.Contains (SIGPIPE))     {  SetSignalHandlers (SIGPIPE, handler);       }
    if (not excludedSignals.Contains (SIGHUP))      {  SetSignalHandlers (SIGHUP, handler);        }
    if (not excludedSignals.Contains (SIGXCPU))     {  SetSignalHandlers (SIGXCPU, handler);       }
    if (not excludedSignals.Contains (SIGXFSZ))     {  SetSignalHandlers (SIGXFSZ, handler);       }
#endif
}

void    SignalHandlerRegistry::FirstPassSignalHandler_ (SignalID signal)
{
#if     qDoDbgTraceOnSignalHandlers_
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::SignalHandlerRegistry::FirstPassSignalHandler_"));
    DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
#endif
    SignalHandlerRegistry&  SHR =   Get ();

    for (SignalHandler sh : SHR.fDirectHandlers_.LookupValue (signal)) {
        sh (signal);
    }
    shared_ptr<SignalHandlerRegistry::SafeSignalsManager::Rep_> tmp = SignalHandlerRegistry::SafeSignalsManager::sTheRep_;
    if (tmp != nullptr) {
        tmp->NotifyOfArrivalOfPossiblySafeSignal (signal);
    }
}
