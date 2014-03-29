/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Execution::ThreadSafetyBuiltinObject
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <mutex>

#include    "Stroika/Foundation/Characters/String.h"
#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Set.h"
#include    "Stroika/Foundation/Execution/ExternallySynchronizedLock.h"
#include    "Stroika/Foundation/Execution/Sleep.h"
#include    "Stroika/Foundation/Execution/SpinLock.h"
#include    "Stroika/Foundation/Execution/Thread.h"
#include    "Stroika/Foundation/Execution/ThreadPool.h"
#include    "Stroika/Foundation/Execution/WaitableEvent.h"
#include    "Stroika/Foundation/Execution/TimeOutException.h"
#include    "Stroika/Foundation/Math/Common.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Characters;
using   namespace   Containers;

using   Execution::ExternallySynchronizedLock;
using   Execution::Thread;
using   Execution::WaitableEvent;






namespace   {
    namespace AssignAndIterateAtSameTimeTest_ {

        template    <typename ITERABLE_TYPE>
        void    DoIt_ (ITERABLE_TYPE elt1, ITERABLE_TYPE elt2, unsigned int repeatCount)
        {
            /*
             *  To REALLY this this code for thread-safety, use ExternallySynchronizedLock, but to verify it works
             *  without worrying about races, just use mutex.
             */
            //ExternallySynchronizedLock lock;
            mutex lock;

            ITERABLE_TYPE   oneToKeepOverwriting = elt1;
            using ElementType   =   typename ITERABLE_TYPE::ElementType;
            Thread  iterateThread = [&oneToKeepOverwriting, repeatCount, &lock] () {
                for (unsigned int i = 0; i < repeatCount; ++i) {
                    lock_guard<decltype(lock)> critSec (lock);
                    for (ElementType e :  oneToKeepOverwriting) {
                        ElementType e2 = e; // do something
                    }
                }
            };
            Thread  overwriteThread = [&oneToKeepOverwriting, elt1, elt2, repeatCount, &lock] () {
                for (unsigned int i = 0; i < repeatCount; ++i) {
                    for (int ii = 0; ii <= 100; ++ii) {
                        if (Math::IsOdd (ii)) {
                            lock_guard<decltype(lock)> critSec (lock);
                            oneToKeepOverwriting = elt1;
                        }
                        else {
                            lock_guard<decltype(lock)> critSec (lock);
                            oneToKeepOverwriting = elt2;
                        }
                    }
                }
            };
            iterateThread.Start ();
            overwriteThread.Start ();
            iterateThread.WaitForDone ();
            overwriteThread.WaitForDone ();
        }
    }


    void    AssignAndIterateAtSameTimeTest_1_ ()
    {
        Debug::TraceContextBumper traceCtx (SDKSTR ("AssignAndIterateAtSameTimeTest_1_"));
        AssignAndIterateAtSameTimeTest_::DoIt_<String> (String (L"123456789"), String (L"abcdedfghijkqlmopqrstuvwxyz"), 500);
        AssignAndIterateAtSameTimeTest_::DoIt_<Sequence<int>> (Sequence<int> ({1, 3, 4, 5, 6, 33, 12, 13}), Sequence<int> ({4, 5, 6, 33, 12, 13, 1, 3, 99, 33, 4, 5}), 500);
        AssignAndIterateAtSameTimeTest_::DoIt_<Set<int>> (Set<int> ({1, 3, 4, 5, 6, 33, 12, 13}), Set<int> ({4, 5, 6, 33, 12, 13, 34, 388, 3, 99, 33, 4, 5}), 500);
    }
}







namespace   {
    void    DoRegressionTests_ ()
    {
        AssignAndIterateAtSameTimeTest_1_ ();
    }
}



int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
