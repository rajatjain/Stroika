/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WhenTimeExceeded_h_
#define _Stroika_Foundation_Execution_WhenTimeExceeded_h_ 1

#include "../StroikaPreComp.h"

#include "../Time/Realtime.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Execution {

    /**
     *  \note callIfTakesLongerThan is compared <=, so you can pass in zero to always trigger.
     *
     *  \par Example Usage
     *      \code
     *      WhenTimeExceeded    whenTimeExceeded (1.0, [] (DurationSecondsType timeTaken) { Logger::Get ().Log (Logger::eWarning, "Took along time  to do 'x'"); });
     *      \endcode
     */
    struct WhenTimeExceeded {
        WhenTimeExceeded (Time::DurationSecondsType callIfTakesLongerThan, const function<void(Time::DurationSecondsType)>& f);
        ~WhenTimeExceeded ();

    private:
        Time::DurationSecondsType                 fStartedAt_;
        Time::DurationSecondsType                 fCallIfTakesLongerThan_;
        function<void(Time::DurationSecondsType)> fRunIfTakesTooLong;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WhenTimeExceeded.inl"

#endif /*_Stroika_Foundation_Execution_WhenTimeExceeded_h_*/
