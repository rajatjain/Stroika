/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
//  TEST    Foundation::Execution::Threads
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <mutex>

#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Execution/BlockingQueue.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/SpinLock.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/ThreadPool.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;

using Characters::String;
using Containers::Sequence;
using Execution::BlockingQueue;
using Execution::Thread;
using Execution::Finally;
using Execution::SpinLock;
using Execution::Synchronized;
using Execution::RWSynchronized;
using Execution::ThreadPool;
using Execution::WaitableEvent;

namespace {
    void RegressionTest1_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest1_");
        struct FRED {
            static void DoIt (void* ignored)
            {
                for (int i = 1; i < 10; i++) {
                    Execution::Sleep (.001);
                }
            }
        };

        Thread::Ptr thread = Thread::New (bind (&FRED::DoIt, const_cast<char*> ("foo")));
        thread.Start ();
        thread.WaitForDone ();
    }
}

namespace {
    recursive_mutex sharedCriticalSection_;
    void            RegressionTest2_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest2_");

        // Make 2 concurrent threads, which share a critical section object to take turns updating a variable
        struct FRED {
            static void DoIt (void* ignored)
            {
                int* argP = reinterpret_cast<int*> (ignored);
                for (int i = 0; i < 10; i++) {
                    lock_guard<recursive_mutex> critSect (sharedCriticalSection_);
                    int                         tmp = *argP;
                    Execution::Sleep (.001);
                    //DbgTrace ("Updating value in thread id %d", ::GetCurrentThreadId  ());
                    *argP = tmp + 1;
                }
            }
        };

        int         updaterValue = 0;
        Thread::Ptr thread1      = Thread::New (bind (&FRED::DoIt, &updaterValue));
        Thread::Ptr thread2      = Thread::New (bind (&FRED::DoIt, &updaterValue));
        thread1.Start ();
        thread2.Start ();
        thread1.WaitForDone ();
        thread2.WaitForDone ();
        VerifyTestResult (updaterValue == 2 * 10);
    }
}

namespace {
    WaitableEvent sRegTest3Event_T1_ (WaitableEvent::eAutoReset);
    WaitableEvent sRegTest3Event_T2_ (WaitableEvent::eAutoReset);
    namespace WAITABLE_EVENTS_ {
        void NOTIMEOUTS_ ()
        {
            Debug::TraceContextBumper traceCtx ("pingpong threads with event.wait(NOTIMEOUTS)");
            // Make 2 concurrent threads, which share 2 events to synchonize taking turns updating a variable
            struct FRED1 {
                static void DoIt (void* ignored)
                {
                    int* argP = reinterpret_cast<int*> (ignored);
                    for (int i = 0; i < 10; i++) {
                        sRegTest3Event_T1_.Wait ();
                        int tmp = *argP;
                        Execution::Sleep (.001);
                        // Since fred1/fred2 always take turns, and Fred1 always goes first...
                        VerifyTestResult (tmp % 2 == 0);
                        //DbgTrace ("FRED1: Updating value in of %d", tmp);
                        *argP = tmp + 1;
                        sRegTest3Event_T2_.Set ();
                    }
                }
            };
            struct FRED2 {
                static void DoIt (void* ignored)
                {
                    int* argP = reinterpret_cast<int*> (ignored);
                    for (int i = 0; i < 10; i++) {
                        sRegTest3Event_T2_.Wait ();
                        int tmp = *argP;
                        Execution::Sleep (.001);
                        //DbgTrace ("FRED2: Updating value in of %d", tmp);
                        *argP = tmp + 1;
                        sRegTest3Event_T1_.Set ();
                    }
                }
            };

            sRegTest3Event_T1_.Reset ();
            sRegTest3Event_T2_.Reset ();
            int         updaterValue = 0;
            Thread::Ptr thread1      = Thread::New (bind (&FRED1::DoIt, &updaterValue));
            Thread::Ptr thread2      = Thread::New (bind (&FRED2::DoIt, &updaterValue));
            thread1.Start ();
            thread2.Start ();
            // Both threads start out waiting - until we get things rolling telling one to start.
            // Then they pingpong back and forther
            sRegTest3Event_T1_.Set ();
            thread1.WaitForDone ();
            thread2.WaitForDone ();
            //DbgTrace ("Test3 - updaterValue = %d", updaterValue);
            // If there was a race - its unlikely you'd end up with exact 20 as your result
            VerifyTestResult (updaterValue == 2 * 10);
        }
        void PingBackAndForthWithSimpleTimeouts_ ()
        {
            Debug::TraceContextBumper traceCtx ("pingpong threads with event.wait(WITHTIMEOUT)");
            // Make 2 concurrent threads, which share 2 events to synchonize taking turns updating a variable
            struct FRED1 {
                static void DoIt (void* ignored)
                {
                    int* argP = reinterpret_cast<int*> (ignored);
                    for (int i = 0; i < 10; i++) {
                        sRegTest3Event_T1_.Wait (5.0);
                        int tmp = *argP;
                        Execution::Sleep (.001);
                        // Since fred1/fred2 always take turns, and Fred1 always goes first...
                        VerifyTestResult (tmp % 2 == 0);
                        //DbgTrace ("FRED1: Updating value in of %d", tmp);
                        *argP = tmp + 1;
                        sRegTest3Event_T2_.Set ();
                    }
                }
            };
            struct FRED2 {
                static void DoIt (void* ignored)
                {
                    int* argP = reinterpret_cast<int*> (ignored);
                    for (int i = 0; i < 10; i++) {
                        sRegTest3Event_T2_.Wait (5.0);
                        int tmp = *argP;
                        Execution::Sleep (.001);
                        //DbgTrace ("FRED2: Updating value in of %d", tmp);
                        *argP = tmp + 1;
                        sRegTest3Event_T1_.Set ();
                    }
                }
            };

            sRegTest3Event_T1_.Reset ();
            sRegTest3Event_T2_.Reset ();
            int         updaterValue = 0;
            Thread::Ptr thread1      = Thread::New (bind (&FRED1::DoIt, &updaterValue));
            Thread::Ptr thread2      = Thread::New (bind (&FRED2::DoIt, &updaterValue));
            thread1.Start ();
            thread2.Start ();
            // Both threads start out waiting - until we get things rolling telling one to start.
            // Then they pingpong back and forther
            sRegTest3Event_T1_.Set ();
            thread1.WaitForDone ();
            thread2.WaitForDone ();
            //DbgTrace ("Test3 - updaterValue = %d", updaterValue);
            // If there was a race - its unlikely you'd end up with exact 20 as your result
            VerifyTestResult (updaterValue == 2 * 10);
        }
        void TEST_TIMEOUT_EXECPETIONS_ ()
        {
            Debug::TraceContextBumper traceCtx ("Event wait timeouts");
            bool                      passed = false;
            sRegTest3Event_T1_.Reset ();
            try {
                sRegTest3Event_T1_.Wait (0.5); // should timeout
            }
            catch (const Execution::TimeOutException&) {
                passed = true;
            }
            catch (...) {
            }
            VerifyTestResult (passed);
        }
        void TEST_DEADLOCK_BLOCK_WAIT_AND_ABORT_THREAD_WAITING ()
        {
            Debug::TraceContextBumper traceCtx ("Deadlock block on waitable event and abort thread (thread cancelation)");
            // Make 2 concurrent threads, which share 2 events to synchonize taking turns updating a variable
            struct FRED1 {
                static void DoIt (void* ignored)
                {
                    sRegTest3Event_T1_.Wait (60.0); // just has to be much more than the waits below
                }
            };

            sRegTest3Event_T1_.Reset ();
            int         updaterValue = 0;
            Thread::Ptr thread1      = Thread::New (bind (&FRED1::DoIt, &updaterValue));
            thread1.Start ();

            // At this point the thread SHOULD block and wait 60.0 seconds
            {
                const Time::DurationSecondsType kMargingOfErrorLo_ = .5;
                const Time::DurationSecondsType kMargingOfErrorHi_ = 5.0; // if sys busy, thread could be put to sleep almost any amount of time
                const Time::DurationSecondsType kWaitOnAbortFor    = 1.0;
                Time::DurationSecondsType       startTestAt        = Time::GetTickCount ();
                Time::DurationSecondsType       caughtExceptAt     = 0;

                try {
                    thread1.WaitForDone (kWaitOnAbortFor);
                }
                catch (const Execution::TimeOutException&) {
                    caughtExceptAt = Time::GetTickCount ();
                }
                Time::DurationSecondsType expectedEndAt = startTestAt + kWaitOnAbortFor;
                if (not(expectedEndAt - kMargingOfErrorLo_ <= caughtExceptAt and caughtExceptAt <= expectedEndAt + kMargingOfErrorHi_)) {
                    DbgTrace (L"expectedEndAt=%f, caughtExceptAt=%f", double(expectedEndAt), double(caughtExceptAt));
                }
                VerifyTestResult (expectedEndAt - kMargingOfErrorLo_ <= caughtExceptAt);
                // FAILURE:
                //      2.0a208x release - in regtests on raspberrypi-gcc-5, regtests  - (caughtExceptAt - expectedEndAt) was 4.1,
                //      so may need to be much larger occasionally (on slow raspberry pi) - but rarely fails.
                //      But failed with kMargingOfErrorHi_=2.0, so from 2.5 5.0 for future releases
                VerifyTestResult (caughtExceptAt <= expectedEndAt + kMargingOfErrorHi_);
            }

            // Now ABORT and WAITFORDONE - that should kill it nearly immediately
            {
                constexpr Time::DurationSecondsType kMargingOfError = 3.5; // larger margin of error cuz sometimes fails on raspberrypi
                constexpr Time::DurationSecondsType kWaitOnAbortFor = 2.5; // use such a long timeout cuz we run this on 'debug' builds,
                                                                           // with asan, valgrind, and on small arm devices. Upped from 2.0 to 2.5 seconds
                                                                           // due to timeout on raspberrypi (rare even there)
                Time::DurationSecondsType startTestAt = Time::GetTickCount ();
                try {
                    thread1.AbortAndWaitForDone (kWaitOnAbortFor);
                }
                catch (const Execution::TimeOutException&) {
                    VerifyTestResult (false); // shouldn't fail to wait cuz we did abort
                    // Note - saw this fail once on raspberry pi but appears the machine was just being slow - nothing looked other than slow - wrong in
                    // the tracelog - so don't worry unless we see again. That machine can be quite slow
                    //  -- LGP 2017-07-05
                }
                Time::DurationSecondsType doneAt        = Time::GetTickCount ();
                Time::DurationSecondsType expectedEndAt = startTestAt + kWaitOnAbortFor;
                if (not(startTestAt <= doneAt and doneAt <= expectedEndAt + kMargingOfError)) {
                    DbgTrace (L"startTestAt=%f, doneAt=%f, expectedEndAt=%f", double(startTestAt), double(doneAt), double(expectedEndAt));
                }
                VerifyTestResult (startTestAt <= doneAt and doneAt <= expectedEndAt + kMargingOfError);
            }

            // Thread MUST be done/terminated by this point
            VerifyTestResult (thread1.GetStatus () == Thread::Status::eCompleted);
        }
    }
    void RegressionTest3_WaitableEvents_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest3_WaitableEvents_");
        WAITABLE_EVENTS_::NOTIMEOUTS_ ();
        WAITABLE_EVENTS_::PingBackAndForthWithSimpleTimeouts_ ();
        WAITABLE_EVENTS_::TEST_TIMEOUT_EXECPETIONS_ ();
        WAITABLE_EVENTS_::TEST_DEADLOCK_BLOCK_WAIT_AND_ABORT_THREAD_WAITING ();
    }
}

namespace {
    namespace RegressionTest4_Synchronized_ {
        namespace Private_ {
            struct data_ {
            };
            void Test1_ ()
            {
                using namespace Execution;
                using syncofdata = Synchronized<data_, Synchronized_Traits<recursive_mutex>>;
                using syncofint  = Synchronized<int, Synchronized_Traits<recursive_mutex>>;

                Debug::TraceContextBumper traceCtx ("RegressionTest4_Synchronized_");
                {
                    syncofdata x;
                    syncofdata y = data_ ();
                    x            = data_ ();
                }
                {
                    syncofint x;
                    syncofint y = 3;
                    x           = 4;
                }
                {
                    // Make 2 concurrent threads, which update a lynchronized variable
                    struct FRED {
                        static void DoIt (void* ignored)
                        {
                            syncofint* argP = reinterpret_cast<syncofint*> (ignored);
                            for (int i = 0; i < 10; i++) {
                                syncofint::WritableReference r   = argP->rwget ();
                                int                          tmp = r;
                                Execution::Sleep (.01);
                                //DbgTrace ("Updating value in thread id %d", ::GetCurrentThreadId  ());
                                r = tmp + 1;
#if 0
                                lock_guard<recursive_mutex> critSect (*argP);
                                int tmp = *argP;
                                Execution::Sleep (.01);
                                //DbgTrace ("Updating value in thread id %d", ::GetCurrentThreadId  ());
                                *argP = tmp + 1;
#endif
                            }
                        }
                    };
                    syncofint   updaterValue = 0;
                    Thread::Ptr thread1      = Thread::New (bind (&FRED::DoIt, &updaterValue));
                    Thread::Ptr thread2      = Thread::New (bind (&FRED::DoIt, &updaterValue));
                    thread1.Start ();
                    thread2.Start ();
                    thread1.WaitForDone ();
                    thread2.WaitForDone ();
                    VerifyTestResult (updaterValue == 2 * 10);
                }
            }
            void Test2_LongWritesBlock_ ()
            {
                Debug::TraceContextBumper                  ctx{"Test2_LongWritesBlock_"};
                static constexpr int                       kBaseRepititionCount_ = 500;
                static constexpr Time::DurationSecondsType kBaseSleepTime_       = 0.02;
                Synchronized<int>                          syncData{0};
                Thread::Ptr                                readerThread = Thread::New ([&]() {
                    Debug::TraceContextBumper ctx{"readerThread"};
                    // Do 10x more reads than writer loop, but sleep 1/10th as long
                    for (int i = 0; i < kBaseRepititionCount_ * 10; ++i) {
                        VerifyTestResult (syncData.load () % 2 == 0);
                        Execution::Sleep (kBaseSleepTime_ / 10.0); // hold the lock kBaseSleepTime_ / 10.0
                    }
                });
                Thread::Ptr writerThread = Thread::New ([&]() {
                    Debug::TraceContextBumper ctx{"writerThread"};
                    for (int i = 0; i < kBaseRepititionCount_; ++i) {
                        auto rwLock = syncData.rwget ();
                        rwLock.store (rwLock.load () + 1);  // set to a value that will cause reader thread to fail
                        Execution::Sleep (kBaseSleepTime_); // hold the lock kBaseSleepTime_
                        VerifyTestResult (syncData.load () % 2 == 1);
                        rwLock.store (rwLock.load () + 1); // set to a safe value
                    }
                    VerifyTestResult (syncData.load () == kBaseRepititionCount_ * 2);
                });
                Thread::Start ({readerThread, writerThread});
                Thread::WaitForDone ({readerThread, writerThread});
            }
        }
        void DoIt ()
        {
            Debug::TraceContextBumper ctx{"RegressionTest4_Synchronized_"};
            Private_::Test1_ ();
            Private_::Test2_LongWritesBlock_ ();
        }
    }
}

namespace {
    void RegressionTest5_Aborting_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest5_Aborting_");
        {
            struct FRED {
                static void DoIt ()
                {
                    while (true) {
                        Execution::CheckForThreadInterruption ();
                    }
                }
            };
            Thread::Ptr thread = Thread::New (&FRED::DoIt);
            thread.Start ();
            try {
                thread.WaitForDone (0.3); // should timeout
                VerifyTestResult (false);
            }
            catch (const Execution::TimeOutException&) {
                // GOOD
            }
            catch (...) {
                VerifyTestResult (false);
            }
            // Now - abort it, and wait
            thread.AbortAndWaitForDone ();
        }
#if qPlatform_Windows
        {
            Thread::Ptr thread = Thread::New ([]() {
                while (true) {
                    // test alertable 'throw'
                    ::SleepEx (0, true);
                }
            });
            thread.Start ();
            try {
                thread.WaitForDone (0.3); // should timeout
                VerifyTestResult (false);
            }
            catch (const Execution::TimeOutException&) {
                // GOOD
            }
            catch (...) {
                VerifyTestResult (false);
            }
            // Now - abort it, and wait
            thread.AbortAndWaitForDone ();
        }
#endif
    }
}

namespace {
    void RegressionTest6_ThreadWaiting_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest6_ThreadWaiting_");
#if qStroika_Foundation_Exection_Thread_SupportThreadStatistics
        // if this triggers - add waits to end of procedure - so we assure no 'side effects' moving on to next test...
        auto&& cleanupReport = Finally (
            []() {
                again:
                    auto runningThreads = Execution::Thread::GetStatistics ().fRunningThreads;
                    DbgTrace (L"Total Running threads at end: %d", runningThreads.size ());
                    for (Execution::Thread::IDType threadID : runningThreads) {
                        DbgTrace (L"Exiting main with thread %s running", Characters::ToString (threadID).c_str ());
                    }
                    if (not runningThreads.empty ()) {
                        Execution::Sleep (1.0);
                        DbgTrace ("trying again...");
                        goto again;
                    }
                    VerifyTestResult (runningThreads.size () == 0);
            });
#endif
        struct FRED {
            static void DoIt ()
            {
                Execution::Sleep (0.01);
            }
        };

        // Normal usage
        {
            Thread::Ptr thread = Thread::New (&FRED::DoIt);
            thread.Start ();
            thread.WaitForDone ();
        }

        // OK to never wait
        for (int i = 0; i < 100; ++i) {
            Thread::Ptr thread = Thread::New (&FRED::DoIt);
            thread.Start ();
        }

        // OK to wait and wait
        {
            Thread::Ptr thread = Thread::New (&FRED::DoIt);
            thread.Start ();
            thread.WaitForDone ();
            thread.WaitForDone (1.0); // doesn't matter how long cuz its already DONE
            thread.WaitForDone ();
            thread.WaitForDone ();
        }
    }
}

namespace {
    void RegressionTest7_SimpleThreadPool_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest7_SimpleThreadPool_");
        {
            ThreadPool p;
            p.SetPoolSize (1);
        }
        {
            ThreadPool p;
            p.SetPoolSize (1);
            int                  intVal = 3;
            ThreadPool::TaskType task{
                [&intVal]() {
                    intVal++;
                }};
            p.AddTask (task);
            p.WaitForTask (task);
            VerifyTestResult (intVal == 4);
        }
    }
}

namespace {
    void RegressionTest8_ThreadPool_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest8_ThreadPool_");
        // Make 2 concurrent tasks, which share a critical section object to take turns updating a variable
        auto doIt = [](int* argP) {
            for (int i = 0; i < 10; i++) {
                lock_guard<recursive_mutex> critSect (sharedCriticalSection_);
                int                         tmp = *argP;
                Execution::Sleep (.01);
                //DbgTrace ("Updating value in thread id %d", ::GetCurrentThreadId  ());
                *argP = tmp + 1;
            }
        };
        for (unsigned int threadPoolSize = 1; threadPoolSize < 10; ++threadPoolSize) {
            ThreadPool p;
            p.SetPoolSize (threadPoolSize);
            int                  updaterValue = 0;
            ThreadPool::TaskType task1{[&updaterValue, &doIt]() { doIt (&updaterValue); }};
            ThreadPool::TaskType task2{[&updaterValue, &doIt]() { doIt (&updaterValue); }};
            p.AddTask (task1);
            p.AddTask (task2);
            p.WaitForTask (task1);
            p.WaitForTask (task2);
            VerifyTestResult (updaterValue == 2 * 10);
        }
    }
}

namespace {
    void RegressionTest9_ThreadsAbortingEarly_ ()
    {
        Debug::TraceContextBumper traceCtx ("RegressionTest9_ThreadsAbortingEarly_");
        // I was seeing SOME rare thread bug - trying to abort a thread which was itself trying to create a new thread - and was
        // between the create of thread and Abort
        Containers::Collection<Thread::Ptr> innerThreads;
        auto                                DoItInnerThread = []() {
            Execution::Sleep (.01);
        };
        auto DoOuterThread = [DoItInnerThread, &innerThreads]() {
            while (true) {
                Thread::Ptr t = Thread::New (DoItInnerThread);
                innerThreads.Add (t);
                Execution::Sleep (.02);
                t.Start ();
            }
        };
        Thread::Ptr thread = Thread::New (DoOuterThread);
        thread.Start ();
        Execution::Sleep (.5);
        thread.AbortAndWaitForDone ();
        // NB: we must call AbortAndWaitForDone on innerThreads because we could have created the thread but not started it, so
        // wait for done will never terminate
        innerThreads.Apply ([](Thread::Ptr t) { t.AbortAndWaitForDone (); }); // assure subthreads  complete before the text exits (else valgrind may report leak)
    }
}

namespace {
    void RegressionTest10_BlockingQueue_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest10_BlockingQueue_"};
        enum { START = 0,
               END   = 100 };
        int                             expectedValue = (START + END) * (END - START + 1) / 2;
        int                             counter       = 0;
        BlockingQueue<function<void()>> q;

        Verify (q.GetLength () == 0);

        Thread::Ptr producerThread = Thread::New (
            [&q, &counter]() {
                for (int incBy = START; incBy <= END; ++incBy) {
                    q.AddTail ([&counter, incBy]() { counter += incBy; });
                }
                q.EndOfInput ();
            },
            Thread::eAutoStart,
            String{L"Producer"});
        Thread::Ptr consumerThread = Thread::New (
            [&q]() {
                while (true) {
                    function<void()> f = q.RemoveHead ();
                    f ();
                }
            },
            Thread::eAutoStart,
            String{L"Consumer"});
        Time::DurationSecondsType killAt = 10.0 + Time::GetTickCount ();
        Stroika_Foundation_Debug_ValgrindDisableHelgrind (counter);
        while (counter != expectedValue and Time::GetTickCount () < killAt) {
            Execution::Sleep (.5);
        }
        Verify (counter == expectedValue);
        producerThread.WaitForDone (); // producer already set to run off the end...
        consumerThread.WaitForDone (); // consumer will end due to exception reading from end
    }
}

namespace {
    void RegressionTest11_AbortSubAbort_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest11_AbortSubAbort_"};
        auto                      testFailToProperlyAbort = []() {
            Thread::Ptr innerThread = Thread::New ([]() {
                Execution::Sleep (1000);
            });
            innerThread.SetThreadName (L"innerThread");
            Thread::Ptr testThread = Thread::New ([&innerThread]() {
                innerThread.Start ();
                Execution::Sleep (1000);
                innerThread.AbortAndWaitForDone ();
            });
            testThread.SetThreadName (L"testThread");
            testThread.Start ();
            Execution::Sleep (1); // wait til both threads running and blocked in sleeps
            testThread.AbortAndWaitForDone ();
            // This is the BUG SuppressInterruptionInContext was meant to solve!
            VerifyTestResult (innerThread.GetStatus () == Thread::Status::eRunning);
            innerThread.AbortAndWaitForDone ();
        };
        auto testInnerThreadProperlyShutDownByOuterThread = []() {
            Thread::Ptr innerThread = Thread::New ([]() {
                Execution::Sleep (1000);
            });
            innerThread.SetThreadName (L"innerThread");
            Thread::Ptr testThread = Thread::New ([&innerThread]() {
                innerThread.Start ();
                auto&& cleanup = Finally (
                    [&innerThread]() noexcept {
                        Thread::SuppressInterruptionInContext suppressInterruptions;
                        innerThread.AbortAndWaitForDone ();
                    });
                Execution::Sleep (1000);
            });
            testThread.SetThreadName (L"testThread");
            testThread.Start ();
            Execution::Sleep (1); // wait til both threads running and blocked in sleeps
            // This is the BUG SuppressInterruptionInContext was meant to solve!
            testThread.AbortAndWaitForDone ();
            VerifyTestResult (innerThread.GetStatus () == Thread::Status::eCompleted);
        };
        testFailToProperlyAbort ();
        testInnerThreadProperlyShutDownByOuterThread ();
    }
}

namespace {
    void RegressionTest12_WaitAny_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest12_WaitAny_"};
        // EXPERIMENTAL
        WaitableEvent                              we1 (WaitableEvent::eAutoReset);
        WaitableEvent                              we2 (WaitableEvent::eAutoReset);
        static constexpr Time::DurationSecondsType kMaxWaitTime_{5.0};
        Thread::Ptr                                t1 = Thread::New ([&we1]() {
            Execution::Sleep (kMaxWaitTime_); // wait long enough that we are pretty sure t2 will always trigger before we do
            we1.Set ();
        });
        Thread::Ptr t2 = Thread::New ([&we2]() {
            Execution::Sleep (0.1);
            we2.Set ();
        });
        Time::DurationSecondsType startAt = Time::GetTickCount ();
        t1.Start ();
        t2.Start ();
        VerifyTestResult (WaitableEvent::WaitForAny (Sequence<WaitableEvent*> ({&we1, &we2})) == set<WaitableEvent*> ({&we2})); // may not indicate a real problem if triggered rarely - just threads ran in queer order, but can happen
        Time::DurationSecondsType timeTaken = Time::GetTickCount () - startAt;
        VerifyTestResult (timeTaken <= kMaxWaitTime_); // make sure we didnt wait for the 1.0 second on first thread
        // They capture so must wait for them to complete
        t1.AbortAndWaitForDone ();
        t2.AbortAndWaitForDone ();
    }
}

namespace {
    void RegressionTest13_WaitAll_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest13_WaitAll_"};
        // EXPERIMENTAL
        WaitableEvent we1 (WaitableEvent::eAutoReset);
        WaitableEvent we2 (WaitableEvent::eAutoReset);
        bool          w1Fired = false;
        bool          w2Fired = false;
        Thread::Ptr   t1      = Thread::New ([&we1, &w1Fired]() {
            Execution::Sleep (0.5);
            w1Fired = true;
            we1.Set ();
        });
        Thread::Ptr t2 = Thread::New ([&we2, &w2Fired]() {
            Execution::Sleep (0.1);
            w2Fired = true;
            we2.Set ();
        });
        Time::DurationSecondsType startAt = Time::GetTickCount ();
        t2.Start ();
        t1.Start ();
        WaitableEvent::WaitForAll (Sequence<WaitableEvent*> ({&we1, &we2}));
        Stroika_Foundation_Debug_ValgrindDisableHelgrind (w1Fired); // tecnically a race
        Stroika_Foundation_Debug_ValgrindDisableHelgrind (w2Fired); // tecnically a race
        VerifyTestResult (w1Fired and w2Fired);
        // They capture so must wait for them to complete
        t1.AbortAndWaitForDone ();
        t2.AbortAndWaitForDone ();
    }
}

namespace {
    void RegressionTest14_SpinLock_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest14_SpinLock_"};
        SpinLock                  lock;
        int                       sum = 0;
        Thread::Ptr               t1  = Thread::New ([&lock, &sum]() {
            for (int i = 0; i < 100; ++i) {
                Execution::Sleep (0.001);
                lock_guard<SpinLock> critSec (lock);
                sum += i;
            }
        });
        Thread::Ptr t2 = Thread::New ([&lock, &sum]() {
            for (int i = 0; i < 100; ++i) {
                Execution::Sleep (0.001);
                lock_guard<SpinLock> critSec (lock);
                sum -= i;
            }
        });
        t1.Start ();
        t2.Start ();
        t1.WaitForDone ();
        t2.WaitForDone ();
        Stroika_Foundation_Debug_ValgrindDisableHelgrind_START (sum);
        VerifyTestResult (sum == 0);
        Stroika_Foundation_Debug_ValgrindDisableHelgrind_END (sum);
    }
}

namespace {
    void RegressionTest15_ThreadPoolStarvationBug_ ()
    {
        //?? DO WE NEED TO ADD
        //#if 0
        //      //fTasksAdded_.WaitQuietly (0.1);
        //      fTasksAdded_.Wait ();
        //#endif
        // Maybe no bug??? BUt tried to reproduce what looked like it MIGHT be a bug/issue based on behavior in
        // BLKQCL...--LGP 2015-10-05
        //
        Debug::TraceContextBumper traceCtx ("RegressionTest15_ThreadPoolStarvationBug_");
        {
            Time::DurationSecondsType           testStartedAt       = Time::GetTickCount ();
            constexpr unsigned                  kThreadPoolSize_    = 10;
            constexpr unsigned                  kStepsToGetTrouble_ = 100 * kThreadPoolSize_; // wag - should go through each thread pretty quickly
            constexpr Time::DurationSecondsType kTime2WaitPerTask_{0.01};
            constexpr Time::DurationSecondsType kRoughEstimateOfTime2Run_ = kTime2WaitPerTask_ * kStepsToGetTrouble_ / kThreadPoolSize_;
            ThreadPool                          p;
            p.SetPoolSize (kThreadPoolSize_);
            auto doItHandler = [kTime2WaitPerTask_]() { Execution::Sleep (kTime2WaitPerTask_); }; // sb pretty quick

            for (int i = 0; i < kStepsToGetTrouble_; ++i) {
                p.AddTask (doItHandler);
            }

            const double              kBigSafetyMultiplierIncaseRunningUnderValgrind_{10000}; // valgrind not speedy ;-)
            Time::DurationSecondsType betterFinishBy = Time::GetTickCount () + kBigSafetyMultiplierIncaseRunningUnderValgrind_ * kRoughEstimateOfTime2Run_;
            while (Time::GetTickCount () <= betterFinishBy) {
                if (p.GetTasksCount () == 0) {
                    break;
                }
                Execution::Sleep (.5); // dont spin too aggressively.
            }
            VerifyTestResult (p.GetTasksCount () == 0);
            Time::DurationSecondsType totalTestTime = Time::GetTickCount () - testStartedAt;
            Verify (totalTestTime < kBigSafetyMultiplierIncaseRunningUnderValgrind_ * kRoughEstimateOfTime2Run_);
        }
    }
}

namespace {
    namespace RegressionTest16_SimpleThreadConstructDestructLeak_ {
        void RunTests ()
        {
            Debug::TraceContextBumper ctx{"RegressionTest16_SimpleThreadConstructDestructLeak_::RunTests"};
            // This test doesn't do a lot by itself, but we run this test under valgrind to look for leaks
            {
                Thread::Ptr t;
            }
            {
                Thread::Ptr t = Thread::New ([]() {});
            }
        }
    }
}

namespace {
    namespace RegressionTest17_ThreadInterruption_ {
        void RunTests ()
        {
            Debug::TraceContextBumper ctx{"RegressionTest17_ThreadInterruption_::RunTests"};
            atomic<unsigned>          interruptCnt{};
            WaitableEvent             we{WaitableEvent::eManualReset};
            Thread::Ptr               t = Thread::New (
                [&]() {
                    while (true) {
                        try {
                            Execution::Sleep (10);
                        }
                        catch (const Thread::AbortException&) {
                            Execution::ReThrow ();
                        }
                        catch (const Thread::InterruptException&) {
                            interruptCnt++;
                        }
                        we.Set ();
                    }
                },
                Thread::eAutoStart);
            constexpr unsigned int kInteruptCnt_{10};
            for (int i = 0; i < kInteruptCnt_; ++i) {
                Execution::Sleep (0.1); // if we interrupt too fast, the thread may only get one or two
                t.Interrupt ();
            }
            we.Wait ();            // so we get at least one interruption
            Execution::Sleep (.1); // so we get a few - not needed
            VerifyTestResult (interruptCnt >= 1);
            VerifyTestResult (interruptCnt <= kInteruptCnt_);
            t.AbortAndWaitForDone ();
        }
    }
}

namespace {
    namespace RegressionTest18_RWSynchronized_ {
        namespace Private_ {
            void Test1_MultipleConcurrentReaders ()
            {
                /**
                 *  Note - this code can EASILY (but not definitely) deadlock if t1 is BETWEEN its two locks when t2 is BETWEEN its two locks (so more likely with longer sleep)
                 *  if using a regular Syncrhonized() lock. But using RWSyncrhonized allows multiple read locks (which is what we are doing) - so no deadlock.
                 *
                 *  NOTE - had to add VALGRIND SUPRESSION - RegressionTest18_RWSynchronized____Test1_MultipleConcurrentReaders__LockOrderReversedForSpecificTestPurpose_Part1
                 *  and RegressionTest18_RWSynchronized____Test1_MultipleConcurrentReaders__LockOrderReversedForSpecificTestPurpose_Part2 because of the reversed lock order.
                 */
                RWSynchronized<int>    sharedData{0};
                unsigned int           sum1{};
                unsigned int           sum2{};
                constexpr unsigned int kRepeatCount_{10000};
                mutex                  forceDeadlockOccasionallyIfNotUsingMultipleReaderLock;
                Thread::Ptr            t1 = Thread::New ([&]() {for (int i = 1; i < kRepeatCount_; i++) { auto holdRWLock = sharedData.cget (); Execution::Sleep (0.0001); lock_guard<mutex> crit { forceDeadlockOccasionallyIfNotUsingMultipleReaderLock };  sum1 += holdRWLock.load ();  } });
                Thread::Ptr            t2 = Thread::New ([&]() {for (int i = 1; i < kRepeatCount_; i++) { lock_guard<mutex> crit{ forceDeadlockOccasionallyIfNotUsingMultipleReaderLock }; Execution::Sleep (0.0001);  auto holdRWLock = sharedData.cget ();  sum2 += holdRWLock.load ();  } });
                Thread::Start ({t1, t2});
                Thread::WaitForDone ({t1, t2});
            }
        }
        void DoIt ()
        {
            Debug::TraceContextBumper ctx{"RegressionTest18_RWSynchronized_"};
            Private_::Test1_MultipleConcurrentReaders ();
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
#if qStroika_Foundation_Exection_Thread_SupportThreadStatistics
        auto&& cleanupReport = Finally (
            []() {
                auto runningThreads = Execution::Thread::GetStatistics ().fRunningThreads;
                DbgTrace (L"Total Running threads at end: %d", runningThreads.size ());
                for (Execution::Thread::IDType threadID : runningThreads) {
                    DbgTrace (L"Exiting main with thread %s running", Characters::ToString (threadID).c_str ());
                }
                VerifyTestResult (runningThreads.size () == 0);
            });
#endif
        RegressionTest1_ ();
        RegressionTest2_ ();
        RegressionTest3_WaitableEvents_ ();
        RegressionTest4_Synchronized_::DoIt ();
        RegressionTest5_Aborting_ ();
        RegressionTest6_ThreadWaiting_ ();
        RegressionTest7_SimpleThreadPool_ ();
        RegressionTest8_ThreadPool_ ();
        RegressionTest9_ThreadsAbortingEarly_ ();
        RegressionTest10_BlockingQueue_ ();
        RegressionTest11_AbortSubAbort_ ();
        RegressionTest12_WaitAny_ ();
        RegressionTest13_WaitAll_ ();
        RegressionTest14_SpinLock_ ();
        RegressionTest15_ThreadPoolStarvationBug_ ();
        RegressionTest16_SimpleThreadConstructDestructLeak_::RunTests ();
        RegressionTest17_ThreadInterruption_::RunTests ();
        RegressionTest18_RWSynchronized_::DoIt ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
