/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Event_inl_
#define _Stroika_Foundation_Execution_Event_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "WaitAbandonedException.h"
#include    "WaitTimedOutException.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            //redeclare to avoid having to include Thread code
            void    CheckForThreadAborting ();


            /*
             ********************************************************************************
             ******************************** Execution::Event ******************************
             ********************************************************************************
             */
            inline  Event::Event ()
                : fMutex_ ()
                , fConditionVariable_ ()
                , fTriggered_ (false)
            {
#if     qTrack_Execution_HandleCounts
                Execution::AtomicIncrement (&sCurAllocatedHandleCount);
#endif
            }
            inline  Event::~Event ()
            {
#if     qTrack_Execution_HandleCounts
                AtomicDecrement (&sCurAllocatedHandleCount);
#endif
            }
            inline  void    Event::Reset ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("Event::Reset"));
                {
                    std::lock_guard<mutex> lockGaurd (fMutex_);
                    fTriggered_ = false;
                }
            }
            inline  void    Event::Set ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("Event::Set"));
                {
                    std::lock_guard<mutex> lockGaurd (fMutex_);
                    fTriggered_ = true;
                    fConditionVariable_.notify_all ();
                }
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Event_inl_*/
