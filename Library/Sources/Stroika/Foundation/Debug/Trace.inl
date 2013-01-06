
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_Trace_inl_
#define _Stroika_Foundation_Debug_Trace_inl_    1

namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            //  class   Trace::Emitter
            template    <typename   CHARTYPE>
            inline  void    Emitter::EmitUnadornedText (const CHARTYPE* p)
            {
                DoEmit_ (p);
            }

            //  class   TraceContextBumper
            inline  TraceContextBumper::TraceContextBumper ()
#if     qDefaultTracingOn
                : fDoEndMarker (false)
                //,fSavedContextName_ ()
#endif
            {
#if     qDefaultTracingOn
                IncCount ();
#endif
            }

#if     !qDefaultTracingOn
            inline  TraceContextBumper::TraceContextBumper (const TChar* contextName)
            {
            }
            inline  TraceContextBumper::~TraceContextBumper ()
            {
            }
#endif


            struct  TraceModuleData_ {
                TraceModuleData_ ();
                ~TraceModuleData_ ();

                Emitter                                 fEmitter;

                // Really SB STRING DEPENDENCY!!!
                Execution::DependableModule::Dependency fBlockAllocationDependency;

            };


        }
    }
}

namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Debug::TraceModuleData_> _Stroika_Foundation_Debug_Trace_ModuleData_; // this object constructed for the CTOR/DTOR per-module side-effects
}

inline  Stroika::Foundation::Debug::Emitter&    Stroika::Foundation::Debug::Emitter::Get ()
{
    return Execution::ModuleInitializer<TraceModuleData_>::Actual ().fEmitter;
}
#endif  /*_Stroika_Foundation_Debug_Trace_inl_*/
