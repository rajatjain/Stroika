/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"
#include    "../Containers/Mapping.h"
#include    "../Debug/Trace.h"

#include    "Signals.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;

using   Containers::Mapping;
using   Containers::Set;


// maybe useful while debugging signal code, but VERY unsafe
// and could easily be the source of bugs/deadlocks!
#ifndef qDoDbgTraceOnSignalHandlers_
#define qDoDbgTraceOnSignalHandlers_    0
#endif


namespace   {
    mutex sCritSection_;

    Mapping<SignalIDType, Set<SignalHandlerType>>    sHandlers_;

    bool    IsSigIgnore_ (const Set<SignalHandlerType>& sigSet)
    {
        return sigSet.size () == 1 and sigSet.Contains (SignalHandlerRegistry::kIGNORED);
    }

    void    MyDIRECTSignalHandler_ (int signal)
    {
#if  qDoDbgTraceOnSignalHandlers_
        Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::{}::MyDIRECTSignalHandler_"));
        DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
#endif
        Set<SignalHandlerType>  handlers;
        {
            lock_guard<mutex> critSec (sCritSection_);
            handlers = *sHandlers_.Lookup (signal);
        }
        for (auto i : handlers) {
            if (i != SignalHandlerRegistry::kIGNORED and i != nullptr) {
                i (signal);
            }
        }
    }
}





/*
 ********************************************************************************
 ******************** Execution::SignalHandlerRegistry **************************
 ********************************************************************************
 */

#if     qPlatform_POSIX
// Important to use direct signal handler because we send the signal to a specific thread, and must set a thread local
// variable
SignalHandlerType   SignalHandlerRegistry::kCallInRepThreadAbortProcSignalHandler_ =   SignalHandlerType (Rep_::CalledInRepThreadAbortProc_, SignalHandlerType::Type::eDirect);
#endif

const   SignalHandlerType   SignalHandlerRegistry::kIGNORED =   SIG_IGN;

SignalHandlerRegistry&  SignalHandlerRegistry::Get ()
{
    static  SignalHandlerRegistry   sThe_;
    return sThe_;
}

SignalHandlerRegistry::SignalHandlerRegistry ()
{
}

Set<SignalIDType>   SignalHandlerRegistry::GetHandledSignals () const
{
    Set<SignalIDType>   result;
    {
        lock_guard<mutex> critSec (sCritSection_);
        for (auto i : sHandlers_) {
            result.Add (i.fKey);
        }
    }
    return result;
}

Set<SignalHandlerType>  SignalHandlerRegistry::GetSignalHandlers (SignalIDType signal) const
{
    lock_guard<mutex> critSec (sCritSection_);
    Optional<Set<SignalHandlerType>>    i   =   sHandlers_.Lookup (signal);
    return i.IsMissing () ? set<SignalHandlerType> () : *i;
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal)
{
    SetSignalHandlers (signal, Set<SignalHandlerType> ());
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal, SignalHandlerType handler)
{
    SetSignalHandlers (signal, Set<SignalHandlerType> (&handler, &handler + 1));
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal, const Set<SignalHandlerType>& handlers)
{
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::{}::SetSignalHandlers"));
    DbgTrace (L"(signal = %s, handlers.size () = %d, ....)", SignalToName (signal).c_str (), handlers.size ());
    lock_guard<mutex> critSec (sCritSection_);
    if (handlers.empty ()) {
        // save memory and remove empty items from list
        sHandlers_.Remove (signal);
    }
    else {
        sHandlers_.Add (signal, handlers);
    }
    if (handlers.empty ()) {
        // nothing todo - empty list treated as not in sHandlers_ list
        (void)::signal (signal, SIG_DFL);
    }
    else if (IsSigIgnore_ (handlers)) {
        (void)::signal (signal, SIG_IGN);
    }
    else {
        (void)::signal (signal, MyDIRECTSignalHandler_);
    }
}

void    SignalHandlerRegistry::AddSignalHandler (SignalIDType signal, SignalHandlerType handler)
{
    Set<SignalHandlerType>  s   =   GetSignalHandlers (signal);
    s.insert (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::RemoveSignalHandler (SignalIDType signal, SignalHandlerType handler)
{
    Set<SignalHandlerType>  s   =   GetSignalHandlers (signal);
    Require (s.Contains (handler));
    s.erase (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::DefaultCrashSignalHandler (SignalIDType signal)
{
    DbgTrace (L"Serious Signal Error trapped: %s ... Aborting", SignalToName (signal).c_str ());
    abort ();
}

void    SignalHandlerRegistry::SetStandardCrashHandlerSignals (SignalHandlerType handler, const Set<SignalIDType>& excludedSignals)
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







/*
 ********************************************************************************
 ************************** Execution::SignalToName *****************************
 ********************************************************************************
 */
wstring Execution::SignalToName (SignalIDType signal)
{
    switch (signal) {
        case    SIGINT:
            return L"SIGINT";
        case    SIGILL:
            return L"SIGILL";
        case    SIGSEGV:
            return L"SIGSEGV";
        case    SIGABRT:
            return L"SIGABRT";
#if     defined (SIGSYS)
        case    SIGSYS:
            return L"SIGSYS";
#endif
#if     defined (SIGBUS)
        case    SIGBUS:
            return L"SIGBUS";
#endif
#if     defined (SIGUSR1)
        case    SIGUSR1:
            return L"SIGUSR1";
#endif
#if     defined (SIGUSR2)
        case    SIGUSR2:
            return L"SIGUSR2";
#endif
#if     defined (SIGHUP)
        case    SIGHUP:
            return L"SIGHUP";
#endif
        case    SIGTERM:
            return L"SIGTERM";
#if     defined (SIGPIPE)
        case    SIGPIPE:
            return L"SIGPIPE";
#endif
#if     defined (SIGQUIT)
        case    SIGQUIT:
            return L"SIGQUIT";
#endif
#if     defined (SIGXCPU)
        case    SIGXCPU:
            return L"SIGXCPU";
#endif
#if     defined (SIGXFSZ)
        case    SIGXFSZ:
            return L"SIGXFSZ";
#endif

        default:
            return Characters::CString::Format (L"Signal# %d", signal);
    }
}






/*
 ********************************************************************************
 **************************** Execution::SendSignal *****************************
 ********************************************************************************
 */
void    Execution::SendSignal (Thread::NativeHandleType h, SignalIDType signal)
{
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::Execution::SendSignal"));
#if     qPlatform_POSIX
    DbgTrace (L"(signal = %s, 0x%lx)", SignalToName (signal).c_str (), static_cast<unsigned long> (h));
#else
    DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
#endif

#if     qPlatform_POSIX
    Verify (pthread_kill (h, signal) == 0);
#else
    AssertNotImplemented ();
#endif
}





#if     qPlatform_POSIX
/*
 ********************************************************************************
 ************* Execution::ScopedBlockCurrentThreadSignal ************************
 ********************************************************************************
 */
ScopedBlockCurrentThreadSignal::ScopedBlockCurrentThreadSignal (SignalIDType signal)
    : fSignal_ (signal)
    , fRestoreMask_ ()
{
    //DbgTrace (L"ScopedBlockCurrentThreadSignal blocking signals for %s", SignalToName (signal).c_str ());
    sigset_t    mySet;
    sigemptyset (&mySet);
    (void)::sigaddset (&mySet, signal);
    sigemptyset (&fRestoreMask_);           // Unclear if this emptyset call is needed?
    Verify (pthread_sigmask (SIG_BLOCK,  &mySet, &fRestoreMask_) == 0);
}

ScopedBlockCurrentThreadSignal::~ScopedBlockCurrentThreadSignal ()
{
    //DbgTrace (L"ScopedBlockCurrentThreadSignal restoriing signals for %s", SignalToName (fSignal_).c_str ());
    Verify (pthread_sigmask (SIG_SETMASK,  &fRestoreMask_, nullptr) == 0);
}
#endif
