/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
//  TEST    Foundation::PERFORMANCE
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <array>
#include    <iostream>
#include    <fstream>
#include    <mutex>
#include    <sstream>

#include    "Stroika/Foundation/Configuration/StroikaVersion.h"

#include    "Stroika/Foundation/Characters/Format.h"
#include    "Stroika/Foundation/Characters/Float2String.h"
#include    "Stroika/Foundation/Characters/String.h"
#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
#include    "Stroika/Foundation/Characters/StringBuilder.h"
#endif
#include    "Stroika/Foundation/Characters/String2Float.h"
#include    "Stroika/Foundation/Containers/Collection.h"
#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Set.h"
#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Configuration/Enumeration.h"
#include    "Stroika/Foundation/Configuration/StroikaVersion.h"
#include    "Stroika/Foundation/DataExchange/BadFormatException.h"
#include    "Stroika/Foundation/DataExchange/INI/Reader.h"
#include    "Stroika/Foundation/DataExchange/INI/Writer.h"
#include    "Stroika/Foundation/DataExchange/JSON/Reader.h"
#include    "Stroika/Foundation/DataExchange/JSON/Writer.h"
#include    "Stroika/Foundation/DataExchange/XML/Reader.h"
#include    "Stroika/Foundation/DataExchange/XML/Writer.h"
#include    "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/StringException.h"
#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Memory/BLOB.h"
#include    "Stroika/Foundation/Time/DateTime.h"
#include    "Stroika/Foundation/Time/Duration.h"
#include    "Stroika/Foundation/Time/Realtime.h"
#include    "Stroika/Foundation/Traversal/DiscreteRange.h"
#include    "Stroika/Foundation/Traversal/FunctionalApplication.h"
#include    "Stroika/Foundation/Traversal/Generator.h"
#include    "Stroika/Foundation/Traversal/Range.h"
#include    "Stroika/Foundation/Streams/ExternallyOwnedMemoryBinaryInputStream.h"

#include    "../TestHarness/TestHarness.h"

#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
#include    "Stroika/Foundation/Execution/SpinLock.h"
#include    "Stroika/Foundation/Memory/SharedPtr.h"
#include    "Stroika/Foundation/Streams/BasicTextOutputStream.h"
#endif


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Math;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Time;



/*
 *  TODO:
 *
 *      @todo   PERFORAMNCE SCORES/PERFORAMNCE REGTEST
 *              (add todo - could use multiple samples and take median)
 *              (once we have new median code) *
 *              (create todo)
 *
 */


// Turn this on rarely to calibrate so # runs a good test
//#define   qPrintOutIfBaselineOffFromOneSecond (!qDebug && defined (_MSC_VER) && defined (WIN32) && !defined (_WIN64))


// My performance expectation numbers are calibrated for MSVC (2k13.net)
// Dont print when they differ on other platforms.
// This is only intended to alert me when something changes GROSSLY.
#define   qPrintOutIfFailsToMeetPerformanceExpectations (!qDebug && defined (_MSC_VER) && defined (WIN32) && !defined (_WIN64))


// Use this so when running #if qDebug case - we dont waste a ton of time with this test
#define   qDebugCaseRuncountRatio (.01)




namespace {
    string  pctFaster2String_ (double pct)
    {
        if (pct < 0) {
            return Format (L"%.2f%% slower", -pct).AsNarrowSDKString ();
        }
        else {
            return Format (L"%.2f%% faster", pct).AsNarrowSDKString ();
        }
    }
}


namespace {
    const char kDefaultPerfOutFile_[]   =   "PerformanceDump.txt";
    bool    sShowOutput_    =   false;
}


namespace {
#if     !qDebug && defined (_MSC_VER) && defined (WIN32) && !defined (_WIN64)
    double  sTimeMultiplier_    =   5.0;    // default larger so on reg-tests we get more consistent percentages
#else
    double  sTimeMultiplier_    =   1.0;
#endif
}


namespace {
    ostream&    GetOutStream_ ()
    {
        static  shared_ptr<ostream> out2File;
        if (not sShowOutput_ and out2File.get () == nullptr) {
            out2File.reset (new ofstream (kDefaultPerfOutFile_));
        }
        ostream&    outTo = (sShowOutput_ ? cout : *out2File);
        return outTo;
    }
}


namespace {

    void    DEFAULT_TEST_PRINTER (String testName, String baselineTName, String compareWithTName, double warnIfPerformanceScoreHigherThan, DurationSecondsType baselineTime, DurationSecondsType compareWithTime)
    {
        ostream&    outTo = GetOutStream_ ();
        outTo << "Test " << testName.AsNarrowSDKString () << " (" << baselineTName.AsNarrowSDKString () << " vs " << compareWithTName.AsNarrowSDKString ()  << ")" << endl;
        DurationSecondsType totalTime = baselineTime + compareWithTime;
        double ratio = compareWithTime / baselineTime;
        double performanceScore = ratio;
        //const char    kOneTab_[]  =   "      ";
        const char  kOneTab_[]  =   "\t";
        {
            Float2StringOptions fo  = Float2StringOptions::Precision (2);
            fo.fTrimTrailingZeros = false;
            outTo << kOneTab_ << "PERFORMANCE_SCORE" << kOneTab_ << Float2String (performanceScore, fo).AsNarrowSDKString () << endl;
        }
        outTo << kOneTab_ << "DETAILS:         " << kOneTab_;
        outTo << "[baseline test " << baselineTime << " secs, and comparison " << compareWithTime << " sec, and warnIfPerfScore > " << warnIfPerformanceScoreHigherThan << ", and perfScore=" << performanceScore << "]" << endl;
        outTo << kOneTab_ << "                 " << kOneTab_;
        if (performanceScore < 1) {
            outTo << compareWithTName.AsNarrowSDKString () << " is FASTER" << endl;
        }
        else if (performanceScore > 1) {
            outTo << compareWithTName.AsNarrowSDKString () << " is ***SLOWER***" << endl;
        }
#if     qPrintOutIfFailsToMeetPerformanceExpectations
        if (performanceScore > warnIfPerformanceScoreHigherThan) {
            outTo << kOneTab_ << "                 " << kOneTab_;
            outTo << "{{{WARNING - expected performance score less than " << warnIfPerformanceScoreHigherThan << " and got " << performanceScore << "}}}" << endl;
        }
#endif
        outTo << endl;
    }


    DurationSecondsType RunTest_(function<void()> t, unsigned int runCount)
    {
        DurationSecondsType start = Time::GetTickCount ();
        // volatile attempt to avoid this being optimized away on gcc --LGP 2014-02-17
        for (volatile unsigned int i = 0; i < runCount; ++i) {
            t();
        }
        return Time::GetTickCount () - start;
    }

    // return true if test failed (slower than expected)
    bool    Tester (String testName,
                    function<void()> baselineT, String baselineTName,
                    function<void()> compareWithT, String compareWithTName,
                    unsigned int runCount,
                    double warnIfPerformanceScoreHigherThan,
                    function<void(String testName, String baselineTName, String compareWithTName, double warnIfPerformanceScoreHigherThan, DurationSecondsType baselineTime, DurationSecondsType compareWithTime)> printResults = DEFAULT_TEST_PRINTER
                   )
    {
#if     qDebug
        runCount = static_cast<unsigned int> (runCount * qDebugCaseRuncountRatio);
#endif
        DurationSecondsType baselineTime = RunTest_ (baselineT, runCount);
        DurationSecondsType compareWithTime = RunTest_ (compareWithT, runCount);
#if     qPrintOutIfBaselineOffFromOneSecond
        if (not NearlyEquals<DurationSecondsType> (baselineTime, 1, .15)) {
            cerr << "SUGGESTION: Baseline Time: " << baselineTime << " and runCount = " << runCount << " so try using runCount = " << int (runCount / baselineTime) << endl;
        }
#endif
        printResults (testName, baselineTName, compareWithTName, warnIfPerformanceScoreHigherThan, baselineTime, compareWithTime);
#if     qPrintOutIfFailsToMeetPerformanceExpectations
        double ratio = compareWithTime / baselineTime;
        return ratio > warnIfPerformanceScoreHigherThan;
#else
        return false;
#endif
    }
    bool    Tester (String testName,
                    DurationSecondsType baselineTime,
                    function<void()> compareWithT, String compareWithTName,
                    unsigned int runCount,
                    double warnIfPerformanceScoreHigherThan,
                    function<void(String testName, String baselineTName, String compareWithTName, double warnIfPerformanceScoreHigherThan, DurationSecondsType baselineTime, DurationSecondsType compareWithTime)> printResults = DEFAULT_TEST_PRINTER
                   )
    {
#if     qDebug
        runCount = static_cast<unsigned int> (runCount * qDebugCaseRuncountRatio);
#endif
        baselineTime *= runCount;
        DurationSecondsType compareWithTime = RunTest_ (compareWithT, runCount);
#if     qPrintOutIfBaselineOffFromOneSecond
        if (not NearlyEquals<DurationSecondsType> (baselineTime, 1, .15)) {
            cerr << "SUGGESTION: Baseline Time: " << baselineTime << " and runCount = " << runCount << " so try using runCount = " << int (runCount / baselineTime) << endl;
        }
#endif
        printResults (testName, Characters::Format (L"%f seconds", baselineTime), compareWithTName, warnIfPerformanceScoreHigherThan, baselineTime, compareWithTime);
#if     qPrintOutIfFailsToMeetPerformanceExpectations
        double ratio = compareWithTime / baselineTime;
        return ratio > warnIfPerformanceScoreHigherThan;
#else
        return false;
#endif
    }

    void    Tester (String testName,
                    DurationSecondsType baselineTime,
                    function<void()> compareWithT, String compareWithTName,
                    unsigned int runCount,
                    double warnIfPerformanceScoreHigherThan,
                    Set<String>* failedTestAccumulator,
                    function<void(String testName, String baselineTName, String compareWithTName, double warnIfPerformanceScoreHigherThan, DurationSecondsType baselineTime, DurationSecondsType compareWithTime)> printResults = DEFAULT_TEST_PRINTER
                   )
    {
        if (Tester (testName, baselineTime, compareWithT, compareWithTName, static_cast<unsigned int> (sTimeMultiplier_ * runCount), warnIfPerformanceScoreHigherThan, printResults)) {
            failedTestAccumulator->Add (testName);
        }
    }
    void    Tester (String testName,
                    function<void()> baselineT, String baselineTName,
                    function<void()> compareWithT, String compareWithTName,
                    unsigned int runCount,
                    double warnIfPerformanceScoreHigherThan,
                    Set<String>* failedTestAccumulator,
                    function<void(String testName, String baselineTName, String compareWithTName, double warnIfPerformanceScoreHigherThan, DurationSecondsType baselineTime, DurationSecondsType compareWithTime)> printResults = DEFAULT_TEST_PRINTER
                   )
    {
        if (Tester (testName, baselineT, baselineTName, compareWithT, compareWithTName, static_cast<unsigned int> (sTimeMultiplier_ * runCount), warnIfPerformanceScoreHigherThan, printResults)) {
            failedTestAccumulator->Add (testName);
        }
    }
}










namespace {

    template <typename WIDESTRING_IMPL>
    void    Test_StructWithStringsFillingAndCopying()
    {
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wreorder\"");  // clang appears confused
        struct  S {
            WIDESTRING_IMPL fS1;
            WIDESTRING_IMPL fS2;
            WIDESTRING_IMPL fS3;
            WIDESTRING_IMPL fS4;
            S() {}
            S (const WIDESTRING_IMPL& w1, const WIDESTRING_IMPL& w2, const WIDESTRING_IMPL& w3, const WIDESTRING_IMPL& w4)
                : fS1(w1)
                , fS2(w2)
                , fS3(w3)
                , fS4(w4)
            {
            }
        };
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wreorder\"");  // clang appears confused
        S   s1;
        S   s2 (L"hi mom", L"124 south vanbergan highway", L"Los Angeles 201243", L"834-313-2144");
        s1 = s2;
        vector<S>   v;
        for (size_t i = 1; i < 10; ++i) {
            v.push_back (s2);
        }
        sort (v.begin (), v.end (), [](S a, S b) {
            return b.fS1 < a.fS1;
        });
        VerifyTestResult (v[0].fS1 == v[1].fS1);
    }

}




namespace {

    template <typename WIDESTRING_IMPL>
    void    Test_StructWithStringsFillingAndCopying2()
    {
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wreorder\"");  // clang appears confused
        struct  S {
            WIDESTRING_IMPL fS1;
            WIDESTRING_IMPL fS2;
            WIDESTRING_IMPL fS3;
            WIDESTRING_IMPL fS4;
            WIDESTRING_IMPL fS5;
            WIDESTRING_IMPL fS6;
            S() {}
            S (const WIDESTRING_IMPL& w1, const WIDESTRING_IMPL& w2, const WIDESTRING_IMPL& w3, const WIDESTRING_IMPL& w4)
                : fS1(w1)
                , fS2(w2)
                , fS3(w3)
                , fS4(w4)
                , fS5()
                , fS6()
            {
            }
        };
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wreorder\"");  // clang appears confused
        S   s1;
        S   s2 { L"hi mom", L"124 south vanbergan highway", L"Los Angeles 201243", L"834-313-2144"};
        s1 = s2;
        vector<S>   v;
        v.reserve (10);
        for (size_t i = 1; i < 10; ++i) {
            v.push_back (s2);
        }
        sort (v.begin (), v.end (), [](S a, S b) {
            return b.fS1 < a.fS1;
        });
        VerifyTestResult (v[0].fS1 == v[1].fS1);
    }

}












namespace {

    template <typename WIDESTRING_IMPL>
    void    Test_SimpleStringAppends1_()
    {
        const WIDESTRING_IMPL KBase = L"1234568321";
        WIDESTRING_IMPL w;
        for (int i = 0; i < 10; ++i) {
            w += KBase;
        }
        VerifyTestResult (w.length () == KBase.length () * 10);
    }

}





namespace {

    template <typename WIDESTRING_IMPL>
    void    Test_SimpleStringAppends2_()
    {
        const wchar_t KBase[] = L"1234568321";
        WIDESTRING_IMPL w;
        for (int i = 0; i < 10; ++i) {
            w += KBase;
        }
        VerifyTestResult (w.length () == wcslen(KBase) * 10);
    }

}





namespace {

    template <typename WIDESTRING_IMPL>
    void    Test_SimpleStringAppends3_()
    {
        const wchar_t KBase[] = L"1234568321";
        WIDESTRING_IMPL w;
        for (int i = 0; i < 100; ++i) {
            w += KBase;
        }
        VerifyTestResult (w.length () == wcslen(KBase) * 100);
    }

}






namespace {
    namespace {
        template <typename WIDESTRING_IMPL>
        void    Test_SimpleStringConCat1_T1_(const WIDESTRING_IMPL& src)
        {
            WIDESTRING_IMPL tmp = src + src;
            tmp = tmp + src;
            tmp = src + tmp;
            VerifyTestResult (tmp.length () == src.length () * 4);
        }
    }
    template <typename WIDESTRING_IMPL>
    void    Test_SimpleStringConCat1_()
    {
        const WIDESTRING_IMPL KBase = L"1234568321";
        Test_SimpleStringConCat1_T1_ (KBase);
    }

}










namespace {
    namespace {
        template <typename WIDESTRING_IMPL>
        void    Test_StringSubStr_T1_ (const WIDESTRING_IMPL& src)
        {
            WIDESTRING_IMPL tmp = src.substr (5, 20);
            VerifyTestResult (tmp.length () == 20);
            VerifyTestResult (src.substr (5, 20).length () == 20);
            VerifyTestResult (src.substr (5, 20).substr (3, 3).length () == 3);
        }
    }
    template <typename WIDESTRING_IMPL>
    void    Test_StringSubStr_()
    {
        static  const WIDESTRING_IMPL KBase = L"01234567890123456789012345678901234567890123456789";
        Test_StringSubStr_T1_ (KBase);
    }
}







namespace {

    namespace Test_MutexVersusSharedPtrCopy_MUTEXT_PRIVATE_ {
        mutex   Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_mutex;
        int     Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_int = 1;
        void    Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK(function<void(int*)> doInsideLock)
        {
            // This is to String class locking. We want to know if copying the shared_ptr rep is faster,
            // or just using a mutex
            //
            // I dont care about the (much rarer) write case where we really need to modify
            lock_guard<mutex> critSec (Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_mutex);
            doInsideLock (&Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_int);
        }
        shared_ptr<int> Test_MutexVersusSharedPtrCopy_sharedPtrCase = shared_ptr<int> (new int (1));
        void    Test_MutexVersusSharedPtrCopy_SharedPtrCopy(function<void(int*)> doInsideLock)
        {
            // This is to String class locking. We want to know if copying the shared_ptr rep is faster,
            // or just using a mutex
            //
            // I dont care about the (much rarer) write case where we really need to modify
            shared_ptr<int> tmp = Test_MutexVersusSharedPtrCopy_sharedPtrCase;
            doInsideLock (tmp.get ());
        }

        int s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT;
        void    Test_MutexVersusSharedPtrCopy_COUNTEST (int* i)
        {
            s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT += *i;
        }

    }

    void    Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK()
    {
        using namespace Test_MutexVersusSharedPtrCopy_MUTEXT_PRIVATE_;
        s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK (Test_MutexVersusSharedPtrCopy_COUNTEST);
        }
        VerifyTestResult (s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT == 1000);   // so nothing optimized away
    }
    void    Test_MutexVersusSharedPtrCopy_shared_ptr_copy()
    {
        using namespace Test_MutexVersusSharedPtrCopy_MUTEXT_PRIVATE_;
        s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSharedPtrCopy_SharedPtrCopy (Test_MutexVersusSharedPtrCopy_COUNTEST);
        }
        VerifyTestResult (s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT == 1000);   // so nothing optimized away
    }

}















#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
namespace {

    namespace Test_stdsharedptr_VERSUS_MemorySharedPtr_PRIVATE_ {
        int     COUNTER = 1;
        shared_ptr<int> s_stdSharedPtr2Copy = shared_ptr<int> (new int (1));
        void    Test_stdsharedptr_use_(function<void(int*)> doInsideLock)
        {
            shared_ptr<int> tmp = s_stdSharedPtr2Copy;
            doInsideLock (tmp.get ());
        }
        void    Test_stdsharedptr_alloc_()
        {
            s_stdSharedPtr2Copy = shared_ptr<int> (new int (1));
        }
        SharedPtr<int> s_MemorySharedPtr2Copy = SharedPtr<int> (new int (1));
        void    Test_MemorySharedPtr_use_(function<void(int*)> doInsideLock)
        {
            SharedPtr<int> tmp = s_MemorySharedPtr2Copy;
            doInsideLock (tmp.get ());
        }
        void    Test_MemorySharedPtr_alloc_()
        {
            s_MemorySharedPtr2Copy = SharedPtr<int> (new int (1));
        }
        void    Test_ACCUM (int* i)
        {
            COUNTER += *i;
        }
    }

    void    Test_stdsharedptrBaseline()
    {
        using namespace Test_stdsharedptr_VERSUS_MemorySharedPtr_PRIVATE_;
        COUNTER = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_stdsharedptr_use_ (Test_ACCUM);
        }
        VerifyTestResult (COUNTER == 1000);   // so nothing optimized away
        // less important but still important
        for (int i = 0; i < 100; ++i) {
            Test_stdsharedptr_alloc_ ();
        }
    }
    void    Test_MemorySharedPtr()
    {
        using namespace Test_stdsharedptr_VERSUS_MemorySharedPtr_PRIVATE_;
        COUNTER = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MemorySharedPtr_use_ (Test_ACCUM);
        }
        VerifyTestResult (COUNTER == 1000);   // so nothing optimized away
        // less important but still important
        for (int i = 0; i < 100; ++i) {
            Test_MemorySharedPtr_alloc_ ();
        }
    }

}
#endif












namespace {
    namespace   Test_stdFunction_VERSUS_virtualClassRunnable_PRIVATE_ {
        void    innerLoop_ ()
        {
            for (int i = 1; i < 1000; i++) {
                if (i == 3) {
                    wstring a = L"x";
                }
            }
        }
        struct MyFakeRunnableRep_ {
            virtual void doRun ()  = 0;
        };
    }
    void    Test_stdFunctionBaseline ()
    {
        using namespace Test_stdFunction_VERSUS_virtualClassRunnable_PRIVATE_;
        function<void()> f = innerLoop_;
        for (int i = 0; i < 10; ++i) {
            f ();
        }
    }
    void    Test_VirtualFunctionBasedRunnable ()
    {
        using namespace Test_stdFunction_VERSUS_virtualClassRunnable_PRIVATE_;
        struct x : MyFakeRunnableRep_ {
            virtual void doRun () override { innerLoop_ (); };
        };
        shared_ptr<MyFakeRunnableRep_>  f { new x () };
        for (int i = 0; i < 10; ++i) {
            f->doRun ();
        }
    }
}















#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
namespace {

    namespace Test_MutexVersusSpinLock_MUTEXT_PRIVATE_ {
        mutex   s_Mutex_;
        int     sCnt2Add_ = 1;
        void    Test_MutexVersusSpinLock_MUTEXT_LOCK(function<void(int*)> doInsideLock)
        {
            lock_guard<mutex> critSec (s_Mutex_);
            doInsideLock (&sCnt2Add_);
        }
        SpinLock   s_SpinLock_;
        void    Test_MutexVersusSpinLock_SPINLOCK_LOCK(function<void(int*)> doInsideLock)
        {
            lock_guard<SpinLock> critSec (s_SpinLock_);
            doInsideLock (&sCnt2Add_);
        }
        int sRunningCnt_;
        void    Test_MutexVersusSpinLock_COUNTEST (int* i)
        {
            sRunningCnt_ += *i;
        }
    }



    void    Test_MutexVersusSpinLock_MUTEXT_LOCK()
    {
        using namespace Test_MutexVersusSpinLock_MUTEXT_PRIVATE_;
        sRunningCnt_ = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSpinLock_MUTEXT_LOCK (Test_MutexVersusSpinLock_COUNTEST);
        }
        VerifyTestResult (sRunningCnt_ == 1000);   // so nothing optimized away
    }
    void    Test_MutexVersusSpinLock_SPIN_LOCK()
    {
        using namespace Test_MutexVersusSpinLock_MUTEXT_PRIVATE_;
        sRunningCnt_ = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSpinLock_SPINLOCK_LOCK (Test_MutexVersusSpinLock_COUNTEST);
        }
        VerifyTestResult (sRunningCnt_ == 1000);   // so nothing optimized away
    }
}
#endif















#if 0
namespace {

    namespace Test_shared_ptrVS_atomic_shared_ptr_PRIVATE_ {
        shared_ptr<int> s_SharedPtrCase = shared_ptr<int> (new int (1));
        void    Test_SharedPtrCopy(function<void(int*)> doInsideLock)
        {
            // This is to String class locking. We want to know if copying the shared_ptr rep is faster,
            // or just using a mutex
            //
            // I dont care about the (much rarer) write case where we really need to modify
            shared_ptr<int> tmp = s_SharedPtrCase;
            doInsideLock (tmp.get ());
        }

        atomic<shared_ptr<int>> s_AtomicSharedPtrCase (shared_ptr<int> (new int (1)));
        void    Test_AtomicSharedPtrCopy(function<void(int*)> doInsideLock)
        {
            //Assert (s_AtomicSharedPtrCase.load ().use_count () == 2);
            // This is to String class locking. We want to know if copying the shared_ptr rep is faster,
            // or just using a mutex
            //
            // I dont care about the (much rarer) write case where we really need to modify
            atomic<shared_ptr<int>> tmp = s_AtomicSharedPtrCase.load ();
            doInsideLock (tmp.load ().get ());
        }

        int CNT = 0;
        void    COUNTEST_ (int* i)
        {
            CNT += *i;
        }

    }

    void    Test_shared_ptrVS_atomic_shared_ptr_REGULAR_SHAREDPTR_CASE()
    {
        using namespace Test_shared_ptrVS_atomic_shared_ptr_PRIVATE_;
        CNT = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_SharedPtrCopy (COUNTEST_);
        }
        VerifyTestResult (CNT == 1000);   // so nothing optimized away
    }
    void    Test_shared_ptrVS_atomic_shared_ptr_ATOMIC_SHAREDPTR_CASE()
    {
        using namespace Test_shared_ptrVS_atomic_shared_ptr_PRIVATE_;
        CNT = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_AtomicSharedPtrCopy (COUNTEST_);
        }
        VerifyTestResult (CNT == 1000);   // so nothing optimized away
    }

}
#endif











#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
namespace {
    template <typename WIDESTRING_IMPL>
    void    Test_OperatorINSERT_ostream_ ()
    {
        using namespace std;
        static  WIDESTRING_IMPL kT1 =   L"abc";
        static  WIDESTRING_IMPL kT2 =   L"123";
        static  WIDESTRING_IMPL kT3 =   L"abc123abc123";
        wstringstream   out;
        for (int i = 0; i < 1000; ++i) {
            out << kT1 << kT2 << kT3;
        }
        VerifyTestResult (out.str ().length () == 18 * 1000);
    }
}
#endif








namespace   {

    template <typename STREAMISH_STRINGBUILDERIMPL, typename STRING_EXTRACTOR>
    void    Test_StreamBuilderStringBuildingWithExtract_ (STRING_EXTRACTOR extractor)
    {
        STREAMISH_STRINGBUILDERIMPL    out;
        for (int i = 0; i < 20; ++i) {
            out << L"0123456789";
            out << L" ";
            out << L"01234567890123456789";
        }
        VerifyTestResult (extractor (out).length () == 31 * 20);
    }

}












namespace   {
    template <typename WIDESTRING_IMPL>
    void    Test_String_cstr_call_ ()
    {
        static  WIDESTRING_IMPL s1 = L"abcd 23234j aksdf alksdjf lkasf jklsdf asdf baewr";
        static  WIDESTRING_IMPL s2 = L"o3424";
        static  WIDESTRING_IMPL s3 = L"o3424";
        static  WIDESTRING_IMPL s4 = L"o3424";
        static  WIDESTRING_IMPL s5 = L"abcd 23234j aksdf alksdjf lkasf jklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdf asdf baewr";
        size_t s1len    =   s1.length ();
        size_t s2len    =   s2.length ();
        size_t s3len    =   s3.length ();
        size_t s4len    =   s4.length ();
        size_t s5len    =   s5.length ();
        for (volatile int i = 0; i < 200; ++i) {
            VerifyTestResult (s1len == ::wcslen (s1.c_str ()));
            VerifyTestResult (s2len == ::wcslen (s2.c_str ()));
            VerifyTestResult (s3len == ::wcslen (s3.c_str ()));
            VerifyTestResult (s4len == ::wcslen (s4.c_str ()));
            VerifyTestResult (s5len == ::wcslen (s5.c_str ()));
        }
    }
}











namespace   {
    namespace Private_ {
        template <typename CONTAINER>
        void    Test_SequenceVectorAdditionsAndCopies_RecCall_ (CONTAINER c, int recCalls)
        {
            if (recCalls > 0) {
                Test_SequenceVectorAdditionsAndCopies_RecCall_ (c, recCalls - 1);
            }
            VerifyTestResult (c.size () == 500);
        }
    }
    template <typename CONTAINER, typename ELEMENTTYPE = typename CONTAINER::value_type>
    void    Test_SequenceVectorAdditionsAndCopies_ ()
    {
        ELEMENTTYPE addEachTime = ELEMENTTYPE ();
        CONTAINER c;
        for (int i = 0; i < 500; ++i) {
            c.push_back (addEachTime);
        }
        Private_::Test_SequenceVectorAdditionsAndCopies_RecCall_ (c, 20);
    }
}








namespace   {
    namespace Private_ {
        template <typename CONTAINER>
        void    Test_CollectionVectorAdditionsAndCopies_RecCall_ (CONTAINER c, int recCalls)
        {
            if (recCalls > 0) {
                Test_CollectionVectorAdditionsAndCopies_RecCall_ (c, recCalls - 1);
            }
            VerifyTestResult (c.size () == 500);
        }
    }
    template <typename CONTAINER, typename ELEMENTTYPE = typename CONTAINER::value_type>
    void    Test_CollectionVectorAdditionsAndCopies_ (function<void(CONTAINER* c)> f2Add)
    {
        CONTAINER c;
        for (int i = 0; i < 500; ++i) {
            f2Add (&c);
        }
        Private_::Test_CollectionVectorAdditionsAndCopies_RecCall_ (c, 20);
    }
}

















namespace {
    template <typename WIDESTRING_IMPL>
    void    Test_String_Format_ ()
    {
        VerifyTestResult (Format (L"a, %s, %d", L"xxx", 33) == L"a, xxx, 33");
        VerifyTestResult (Format (L"0x%x", 0x20) == L"0x20");
    }
    template <>
    void    Test_String_Format_<wstring> ()
    {
        {
            wchar_t buf[1024];
#if     qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat
            VerifyTestResult (swprintf (buf, NEltsOf (buf), L"a, %s, %d", L"xxx", 33) == 10);
#else
            VerifyTestResult (swprintf (buf, NEltsOf (buf), L"a, %s, %d", "xxx", 33) == 10);     // not with swprintf %s means narrow string unlike Format()
#endif
            VerifyTestResult (wstring (buf) == L"a, xxx, 33");
        }
        {
            wchar_t buf[1024];
            VerifyTestResult (swprintf (buf, NEltsOf (buf), L"0x%x", 0x20) == 4);
            VerifyTestResult (wstring (buf) == L"0x20");
        }
    }
}




















namespace {
    namespace Test_BLOB_Versus_Vector_Byte_DETAILS {
        //static  array<Byte,4*1024>    kArr_4k_ = { 0x1, 0x2, 0x3, };
        static  Byte                kCArr_4k_[4 * 1024] = { 0x1, 0x2, 0x3, };

        template <typename BLOBISH_IMPL>
        size_t    T1_SIZER_ (BLOBISH_IMPL b)
        {
            return b.size ();
        }
        template <typename BLOBISH_IMPL>
        void    T1_ ()
        {
            BLOBISH_IMPL    bn;
            for (int i = 0; i < 100; ++i) {
                //BLOBISH_IMPL  bl = kArr_4k_;
                BLOBISH_IMPL    bl = BLOBISH_IMPL (begin (kCArr_4k_), end (kCArr_4k_));
                BLOBISH_IMPL    b2 = bl;
                BLOBISH_IMPL    b3 = bl;
                BLOBISH_IMPL    b4 = bl;
                bn = b4;
            }
            VerifyTestResult (T1_SIZER_ (bn) == sizeof (kCArr_4k_));
        }

    }
    template <typename BLOBISH_IMPL>
    void    Test_BLOB_Versus_Vector_Byte ()
    {
        Test_BLOB_Versus_Vector_Byte_DETAILS::T1_<BLOBISH_IMPL> ();
    }

}







namespace {
    namespace Test_JSONReadWriteFile_ {
        constexpr   Byte    kSAMPLE_FILE_[] =
            "{\
    \"Aux-Data\" : {\
        \"C3\" : \"-0\",\
        \"EngineId\" : \"B1E56F82-B217-40D3-A24D-FAC491EDCDE8\",\
        \"Gas-Cell\" : \"Short\",\
        \"Sample-Pressure\" : \"-129.277\",\
        \"Sample-Temperature\" : \"-74.51\",\
        \"iC4\" : \"-0\",\
        \"nC4\" : \"-0\",\
        \"nC5\" : \"-0\"\
    },\
    \"Background-ID\" : 5378,\
    \"Raw-Spectrum\" : {\
        \"1000\" : 102.207499,\
        \"1001\" : 104.437091,\
        \"1002\" : 105.038416,\
        \"1003\" : 106.717942,\
        \"1004\" : 108.897728,\
        \"1005\" : 110.894962,\
        \"1006\" : 109.450638,\
        \"1007\" : 109.475062,\
        \"1008\" : 112.364229,\
        \"1009\" : 114.742569,\
        \"1010\" : 114.876544,\
        \"1011\" : 115.329764,\
        \"1012\" : 114.287301,\
        \"1013\" : 116.257922,\
        \"1014\" : 116.513513,\
        \"1015\" : 118.098858,\
        \"1016\" : 120.959124,\
        \"1017\" : 122.762808,\
        \"1018\" : 120.115923,\
        \"1019\" : 123.527138,\
        \"1020\" : 123.097837,\
        \"1021\" : 123.135073,\
        \"1022\" : 127.237821,\
        \"1023\" : 126.812848,\
        \"1024\" : 129.457969,\
        \"1025\" : 128.345917,\
        \"1026\" : 129.984425,\
        \"1027\" : 131.375393,\
        \"1028\" : 131.884103,\
        \"1029\" : 132.656497,\
        \"1030\" : 134.59663,\
        \"1031\" : 135.856095,\
        \"1032\" : 135.952807,\
        \"1033\" : 137.859343,\
        \"1034\" : 136.869097,\
        \"1035\" : 138.835342,\
        \"1036\" : 140.525628,\
        \"1037\" : 141.462064,\
        \"1038\" : 143.509938,\
        \"1039\" : 143.025406,\
        \"1040\" : 145.696478,\
        \"1041\" : 145.621319,\
        \"1042\" : 144.466126,\
        \"1043\" : 147.638448,\
        \"1044\" : 146.277531,\
        \"1045\" : 147.775448,\
        \"1046\" : 151.020184,\
        \"1047\" : 150.296378,\
        \"1048\" : 150.245013,\
        \"1049\" : 154.725126,\
        \"1050\" : 152.592499,\
        \"1051\" : 154.033013,\
        \"1052\" : 157.257001,\
        \"1053\" : 155.615766,\
        \"1054\" : 157.941679,\
        \"1055\" : 158.318632,\
        \"1056\" : 159.615589,\
        \"1057\" : 160.554203,\
        \"1058\" : 163.494878,\
        \"1059\" : 161.874425,\
        \"1060\" : 164.481251,\
        \"1061\" : 166.688488,\
        \"1062\" : 167.057323,\
        \"1063\" : 168.264615,\
        \"1064\" : 166.01023,\
        \"1065\" : 168.396505,\
        \"1066\" : 168.089616,\
        \"1067\" : 170.023101,\
        \"1068\" : 173.431235,\
        \"1069\" : 173.246185,\
        \"1070\" : 173.722823,\
        \"1071\" : 173.153608,\
        \"1072\" : 176.137788,\
        \"1073\" : 175.371997,\
        \"1074\" : 176.688347,\
        \"1075\" : 180.784501,\
        \"1076\" : 180.070223,\
        \"1077\" : 182.610873,\
        \"1078\" : 181.150272,\
        \"1079\" : 183.36719,\
        \"1080\" : 184.092786,\
        \"1081\" : 186.711791,\
        \"1082\" : 187.572159,\
        \"1083\" : 188.785103,\
        \"1084\" : 189.263375,\
        \"1085\" : 190.892796,\
        \"1086\" : 190.323923,\
        \"1087\" : 189.079699,\
        \"1088\" : 190.530891,\
        \"1089\" : 194.173038,\
        \"1090\" : 193.672212,\
        \"1091\" : 194.384409,\
        \"1092\" : 197.342573,\
        \"1093\" : 197.415818,\
        \"1094\" : 199.179823,\
        \"1095\" : 200.596746,\
        \"1096\" : 201.53603,\
        \"1097\" : 199.419327,\
        \"1098\" : 200.564464,\
        \"1099\" : 201.269345,\
        \"1100\" : 202.64605,\
        \"900\" : 5.276987,\
        \"901\" : 6.840619,\
        \"902\" : 6.500289,\
        \"903\" : 8.03882,\
        \"904\" : 6.711202,\
        \"905\" : 10.280579,\
        \"906\" : 10.475101,\
        \"907\" : 10.606406,\
        \"908\" : 13.54551,\
        \"909\" : 13.841327,\
        \"910\" : 12.198958,\
        \"911\" : 16.643659,\
        \"912\" : 14.578187,\
        \"913\" : 18.954581,\
        \"914\" : 18.36068,\
        \"915\" : 19.383819,\
        \"916\" : 21.717242,\
        \"917\" : 19.646697,\
        \"918\" : 23.250176,\
        \"919\" : 22.704226,\
        \"920\" : 25.335496,\
        \"921\" : 25.824652,\
        \"922\" : 26.055423,\
        \"923\" : 28.658592,\
        \"924\" : 28.517954,\
        \"925\" : 30.196885,\
        \"926\" : 31.030664,\
        \"927\" : 30.098038,\
        \"928\" : 33.244351,\
        \"929\" : 34.620035,\
        \"930\" : 35.68152,\
        \"931\" : 36.90124,\
        \"932\" : 36.283642,\
        \"933\" : 38.395281,\
        \"934\" : 39.033175,\
        \"935\" : 40.367104,\
        \"936\" : 39.395377,\
        \"937\" : 39.044223,\
        \"938\" : 43.853149,\
        \"939\" : 43.431587,\
        \"940\" : 42.951665,\
        \"941\" : 44.675716,\
        \"942\" : 46.983485,\
        \"943\" : 45.722461,\
        \"944\" : 47.532444,\
        \"945\" : 49.952878,\
        \"946\" : 48.673128,\
        \"947\" : 51.676115,\
        \"948\" : 50.569155,\
        \"949\" : 54.984444,\
        \"950\" : 54.494472,\
        \"951\" : 53.869777,\
        \"952\" : 54.25914,\
        \"953\" : 58.971926,\
        \"954\" : 57.559519,\
        \"955\" : 57.468957,\
        \"956\" : 60.300616,\
        \"957\" : 59.822135,\
        \"958\" : 61.488314,\
        \"959\" : 62.261685,\
        \"960\" : 64.511706,\
        \"961\" : 63.959589,\
        \"962\" : 65.517899,\
        \"963\" : 66.882009,\
        \"964\" : 66.425605,\
        \"965\" : 68.261815,\
        \"966\" : 70.418805,\
        \"967\" : 69.337126,\
        \"968\" : 72.908709,\
        \"969\" : 73.304041,\
        \"970\" : 73.119387,\
        \"971\" : 76.79671,\
        \"972\" : 74.605076,\
        \"973\" : 75.799573,\
        \"974\" : 77.236541,\
        \"975\" : 79.427965,\
        \"976\" : 78.456946,\
        \"977\" : 82.153217,\
        \"978\" : 81.065032,\
        \"979\" : 83.16378,\
        \"980\" : 85.913195,\
        \"981\" : 86.969779,\
        \"982\" : 86.159601,\
        \"983\" : 87.806876,\
        \"984\" : 88.487533,\
        \"985\" : 90.95273,\
        \"986\" : 88.678744,\
        \"987\" : 92.527448,\
        \"988\" : 93.661814,\
        \"989\" : 93.114072,\
        \"990\" : 92.373317,\
        \"991\" : 96.029655,\
        \"992\" : 96.849889,\
        \"993\" : 97.373866,\
        \"994\" : 98.85073,\
        \"995\" : 100.856283,\
        \"996\" : 101.807204,\
        \"997\" : 102.519829,\
        \"998\" : 100.426089,\
        \"999\" : 104.86934\
    },\
    \"Reference-ID\" : 5379,\
    \"Scan-End\" : \"2015-01-14T15:31:08Z\",\
    \"Scan-ID\" : 5856,\
    \"Scan-Kind\" : \"Sample\",\
    \"Scan-Label\" : \"\",\
    \"Scan-Start\" : \"2015-01-14T15:31:07Z\"\
}";
        enum class ScanKindType {
            Background,
            Reference,
            Sample,
            Stroika_Define_Enum_Bounds(Background, Sample)
        };
        constexpr   Configuration::EnumNames<ScanKindType>    Stroika_Enum_Names(ScanKindType)
        {
            Configuration::EnumNames<ScanKindType>::BasicArrayInitializer {
                {
                    { ScanKindType::Background, L"Background" },
                    { ScanKindType::Reference, L"Reference" },
                    { ScanKindType::Sample, L"Sample" },
                }
            }
        };
        using ScanIDType = int;
        using SpectrumType = Mapping<double, double>;
        using PersistenceScanAuxDataType = Mapping<String, String>;
        struct  ScanDetails_ {
            ScanIDType                  fScanID {};
            DateTime                    fScanStart;
            DateTime                    fScanEnd;
            ScanKindType                fScanKind {};
            String                      fScanLabel {};
            SpectrumType      fRawSpectrum {};
            PersistenceScanAuxDataType      fAuxData {};
            Optional<ScanIDType>        fUseBackground {};
            Optional<ScanIDType>        fUseReference {};
        };

        DataExchange::ObjectVariantMapper GetPersistenceDetailsMapper_ ()
        {
            using   namespace DataExchange;
            ObjectVariantMapper mapper;
            mapper.AddCommonType<ScanIDType> ();
            mapper.AddCommonType<Optional<ScanIDType>> ();
            mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<ScanKindType> (Stroika_Enum_Names(ScanKindType)));
            mapper.Add (ObjectVariantMapper::MakeCommonSerializer_ContainerWithStringishKey<SpectrumType> ());
            mapper.Add (ObjectVariantMapper::MakeCommonSerializer_ContainerWithStringishKey<PersistenceScanAuxDataType> ());
            mapper.AddClass<ScanDetails_> ({
                ObjectVariantMapper_StructureFieldInfo_Construction_Helper (ScanDetails_, fScanID, L"Scan-ID"),
                ObjectVariantMapper_StructureFieldInfo_Construction_Helper (ScanDetails_, fScanStart, L"Scan-Start"),
                ObjectVariantMapper_StructureFieldInfo_Construction_Helper (ScanDetails_, fScanEnd, L"Scan-End"),
                ObjectVariantMapper_StructureFieldInfo_Construction_Helper (ScanDetails_, fScanKind, L"Scan-Kind"),
                ObjectVariantMapper_StructureFieldInfo_Construction_Helper (ScanDetails_, fScanLabel, L"Scan-Label"),
                ObjectVariantMapper_StructureFieldInfo_Construction_Helper (ScanDetails_, fRawSpectrum, L"Raw-Spectrum"),
                ObjectVariantMapper_StructureFieldInfo_Construction_Helper (ScanDetails_, fAuxData, L"Aux-Data"),
                ObjectVariantMapper_StructureFieldInfo_Construction_Helper (ScanDetails_, fUseBackground, L"Background-ID"),
                ObjectVariantMapper_StructureFieldInfo_Construction_Helper (ScanDetails_, fUseReference, L"Reference-ID"),
            });
            return mapper;
        }

        ScanDetails_    doRead_ ()
        {
            using   namespace DataExchange;
            VariantValue o { JSON::Reader ().Read (Streams::ExternallyOwnedMemoryBinaryInputStream (begin (kSAMPLE_FILE_), end (kSAMPLE_FILE_))) };
            static  const   ObjectVariantMapper kMapper_ = GetPersistenceDetailsMapper_ ();
            return kMapper_.ToObject<ScanDetails_> (o);
        }
        void    DoRunPerfTest ()
        {
            ScanDetails_    sd { doRead_ () };
            Assert (sd.fAuxData.ContainsKey (L"Sample-Pressure"));
            Assert (sd.fScanID == 5856);
        }
    }
}



namespace   {
    void    RunPerformanceTests_ ()
    {
        DateTime    startedAt = DateTime::Now ();
        GetOutStream_ () << "[[[Started testing at: " << startedAt.Format ().AsNarrowSDKString () << "]]]" << endl << endl;
        if (not Math::NearlyEquals (sTimeMultiplier_, 1.0)) {
            GetOutStream_ () << "Using TIME MULTIPLIER: " << sTimeMultiplier_ << endl << endl;
        }

        Set<String> failedTests;
        Tester (
            L"Test of simple locking strategies (mutex v shared_ptr copy)",
            Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK, L"mutex",
            Test_MutexVersusSharedPtrCopy_shared_ptr_copy, L"shared_ptr<> copy",
            14500,
            .5,
            &failedTests
        );
#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
        Tester (
            L"Test of simple locking strategies (mutex v SpinLock)",
            Test_MutexVersusSpinLock_MUTEXT_LOCK, L"mutex",
            Test_MutexVersusSpinLock_SPIN_LOCK, L"SpinLock",
            14600,
            .35,
            &failedTests
        );
#endif
#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
        Tester (
            L"std::shared_ptr versus Memory::SharedPtr",
            Test_stdsharedptrBaseline, L"shared_ptr",
            Test_MemorySharedPtr, L"SharedPtr",
            22950,
            .9,
            &failedTests
        );
#endif
        Tester (
            L"IRunnable versus std::function",
            Test_VirtualFunctionBasedRunnable, L"IRunnable",
            Test_stdFunctionBaseline, L"std::function",
            210000,
            0.5,
            &failedTests
        );
#if 0
        Tester (
            L"atomic sharedptr versus sharedptr",
            Test_shared_ptrVS_atomic_shared_ptr_REGULAR_SHAREDPTR_CASE, L"shared_ptr",
            Test_shared_ptrVS_atomic_shared_ptr_ATOMIC_SHAREDPTR_CASE, L"atomic<shared_ptr<>>",
            15000,
            .8,
            &failedTests
        );
#endif
        Tester (
            L"Simple Struct With Strings Filling And Copying",
            Test_StructWithStringsFillingAndCopying<wstring>, L"wstring",
            Test_StructWithStringsFillingAndCopying<String>, L"Charactes::String",
            38700,
            0.5,
            &failedTests
        );
        Tester (
            L"Simple Struct With Strings Filling And Copying2",
            Test_StructWithStringsFillingAndCopying2<wstring>, L"wstring",
            Test_StructWithStringsFillingAndCopying2<String>, L"Charactes::String",
            37200,
            0.56,
            &failedTests
        );
        Tester (
            L"Simple String append test (+='string object') 10x",
            Test_SimpleStringAppends1_<wstring>, L"wstring",
            Test_SimpleStringAppends1_<String>, L"Charactes::String",
            1210000,
            4.1,
            &failedTests
        );
        Tester (
            L"Simple String append test (+=wchar_t[]) 10x",
            Test_SimpleStringAppends2_<wstring>, L"wstring",
            Test_SimpleStringAppends2_<String>, L"Charactes::String",
            1270000,
            3.7,
            &failedTests
        );
        Tester (
            L"Simple String append test (+=wchar_t[]) 100x",
            Test_SimpleStringAppends3_<wstring>, L"wstring",
            Test_SimpleStringAppends3_<String>, L"Charactes::String",
            267000,
            8.6,
            &failedTests
        );
        Tester (
            L"String a + b",
            Test_SimpleStringConCat1_<wstring>, L"wstring",
            Test_SimpleStringConCat1_<String>, L"String",
            1990000,
            2.3,
            &failedTests
        );
#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
        Tester (
            L"wstringstream << test",
            Test_OperatorINSERT_ostream_<wstring>, L"wstring",
            Test_OperatorINSERT_ostream_<String>, L"Charactes::String",
            5350 ,
            1.6,
            &failedTests
        );
#endif
        Tester (
            L"String::substr()",
            Test_StringSubStr_<wstring>, L"wstring",
            Test_StringSubStr_<String>, L"Charactes::String",
            3060000,
            1.9,
            &failedTests
        );
#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
        Tester (
            L"wstringstream versus BasicTextOutputStream",
        [] () {Test_StreamBuilderStringBuildingWithExtract_<wstringstream> ([](const wstringstream & w) {return w.str ();});} , L"wstringstream",
        [] () {Test_StreamBuilderStringBuildingWithExtract_<BasicTextOutputStream> ([](const BasicTextOutputStream & w) {return w.As<String> ();});}  , L"BasicTextOutputStream",
        184000,
        4.0,
        &failedTests
        );
#endif
#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
        Tester (
            L"wstringstream versus StringBuilder",
        [] () {Test_StreamBuilderStringBuildingWithExtract_<wstringstream> ([](const wstringstream & w) {return w.str ();});} , L"wstringstream",
        [] () {Test_StreamBuilderStringBuildingWithExtract_<StringBuilder> ([](const StringBuilder & w) {return w.As<String> ();});}  , L"StringBuilder",
        181000 ,
        .14,
        &failedTests
        );
#endif
        Tester (
            L"Simple c_str() test",
            Test_String_cstr_call_<wstring>, L"wstring",
            Test_String_cstr_call_<String>, L"Charactes::String",
            39700,
            1.2,
            &failedTests
        );
#if     kStroika_Version_FullVersion  >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
        Tester (
            L"Sequence<int> basics",
            Test_SequenceVectorAdditionsAndCopies_<vector<int>>, L"vector<int>",
            Test_SequenceVectorAdditionsAndCopies_<Sequence<int>>, L"Sequence<int>",
            136000,
            7.2,
            &failedTests
        );
        Tester (
            L"Sequence<string> basics",
            Test_SequenceVectorAdditionsAndCopies_<vector<string>>, L"vector<string>",
            Test_SequenceVectorAdditionsAndCopies_<Sequence<string>>, L"Sequence<string>",
            9660,
            0.9,
            &failedTests
        );
        Tester (
            L"Sequence_DoublyLinkedList<int> basics",
            Test_SequenceVectorAdditionsAndCopies_<vector<int>>, L"vector<int>",
            Test_SequenceVectorAdditionsAndCopies_<Sequence<int>>, L"Sequence_DoublyLinkedList<int>",
            136000,
            7.2,
            &failedTests
        );
        Tester (
            L"Sequence_DoublyLinkedList<string> basics",
            Test_SequenceVectorAdditionsAndCopies_<vector<string>>, L"vector<string>",
            Test_SequenceVectorAdditionsAndCopies_<Sequence<string>>, L"Sequence_DoublyLinkedList<string>",
            9310,
            1.1,
            &failedTests
        );
        Tester (
            L"Collection<int> basics",
        [] () {Test_CollectionVectorAdditionsAndCopies_<vector<int>> ([](vector<int>* c) {c->push_back(2); });} , L"vector<int>",
        [] () {Test_CollectionVectorAdditionsAndCopies_<Collection<int>> ([](Collection<int>* c) {c->Add(2); });}, L"Collection<int>",
        95700,
        7.7,
        &failedTests
        );
        Tester (
            L"Collection<string> basics",
        [] () {Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([](vector<string>* c) {c->push_back(string ()); });} , L"vector<string>",
        [] () {Test_CollectionVectorAdditionsAndCopies_<Collection<string>> ([](Collection<string>* c) {c->Add(string()); });}, L"Collection<string>",
        9100,
        0.9,
        &failedTests
        );
#endif
        Tester (
            L"String Chracters::Format ()",
            Test_String_Format_<wstring>, L"sprintf",
            Test_String_Format_<String>, L"String Characters::Format",
            1390000,
            2.1,
            &failedTests
        );
        Tester (
            L"BLOB versus vector<Byte>",
            Test_BLOB_Versus_Vector_Byte<vector<Byte>>, L"vector<Byte>",
            Test_BLOB_Versus_Vector_Byte<Memory::BLOB>, L"BLOB",
            6440,
            0.26,
            &failedTests
        );
        Tester (
            L"Test_JSONReadWriteFile",
            1 / 600.0,
            Test_JSONReadWriteFile_::DoRunPerfTest , L"Test_JSONReadWriteFile",
            550,
            1.2,
            &failedTests
        );

        GetOutStream_ () << "[[[Tests took: " << (DateTime::Now () - startedAt).PrettyPrint ().AsNarrowSDKString () << "]]]" << endl << endl;

        if (not failedTests.empty ()) {
            String listAsMsg;
            failedTests.Apply ([&listAsMsg] (String i) {if (not listAsMsg.empty ()) {listAsMsg += L", ";} listAsMsg += i; });
            if (sShowOutput_) {
#if     kStroika_Version_FullVersion >= Stroika_Make_FULL_VERSION (2, 0, kStroika_Version_Stage_Alpha, 21, 0)
                Execution::DoThrow (StringException (L"At least one test failed expected time constaint (see above): " + listAsMsg));
#endif
            }
            else {
                Execution::DoThrow (StringException (Format (L"At least one test (%s) failed expected time constraint (see %s)", listAsMsg.c_str (), String::FromAscii (kDefaultPerfOutFile_).c_str ())));
            }
        }
    }
}








int     main (int argc, const char* argv[])
{
    // NOTE: run with --show or look for output in PERF-OUT.txt
    try {
        Sequence<String>  cmdLine   =   ParseCommandLine (argc, argv);
        sShowOutput_ = MatchesCommandLineArgument (cmdLine, L"show");

        {
            Optional<String> arg = MatchesCommandLineArgumentWithValue (cmdLine, L"x");
            if (arg.IsPresent ()) {
                sTimeMultiplier_ = String2Float<double> (*arg);
            }
        }
    }
    catch (const Execution::StringException& e) {
        cerr << "Usage: '" << Characters::WideStringToNarrowSDKString (e.As<wstring> ()) << endl;
        exit (EXIT_FAILURE);
    }
    catch (const std::exception& e) {
        cerr << "Usage: '" << e.what () << endl;
        exit (EXIT_FAILURE);
    }
    catch (...) {
        cerr << "Usage:" << endl;
        exit (EXIT_FAILURE);
    }

    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (RunPerformanceTests_);
    return EXIT_SUCCESS;
}
