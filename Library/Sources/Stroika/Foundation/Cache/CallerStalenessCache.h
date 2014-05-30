/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_CallerStalenessCache_h_
#define _Stroika_Foundation_Cache_CallerStalenessCache_h_    1

#include    "../StroikaPreComp.h"

#include    "../Containers/Mapping.h"
#include    "../Time/Realtime.h"



/**
 *      \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 *      @todo
 *
 *  Implementation Note:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {


            /**
             */
            struct  CallerStalenessCache_Traits_DEFAULT {
                using  TimeStampType = Time::DurationSecondsType  ; // type must support operator<()
                static  TimeStampType   GetCurrentTimestamp()   { return Time::GetTickCount (); }
            };


            /**
             *
             *  The idea behind this cache to to track when something is added, and that the lookup function can avoid
             *  a costly call to compute soemthing if its been recently enough added.
			 *
			 *	For example, consider a system where memory is stored across a slow bus, and several components need to read data from
			 *	across that bus. But the different components have different tolerance for staleness.
			 *
			 *	This CallerStalenessCache will store when the value is updated, and let the caller either return the
			 *	value from cache, or fetch it and update the cache if needed.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename   KEY, typename VALUE, typename TIME_TRAITS = CallerStalenessCache_Traits_DEFAULT>
            class   CallerStalenessCache {
            public:
                /**
                 */
                using TimeStampType = typename TIME_TRAITS::TimeStampType;

            public:
                /**
                 */
                static  TimeStampType   GetCurrentTimestamp ();

            public:
                /**
                 */
                nonvirtual  void    ClearOlderThan (TimeStampType t);

            public:
                /**
                 */
                nonvirtual  void    Clear ();
                nonvirtual  void    Clear (KEY k);

            public:
                /**
                 */
                nonvirtual  VALUE Lookup (KEY k, TimeStampType staleIfOlderThan, const std::function<VALUE()>& cacheFiller);

            private:
                struct  myVal_ {
                    VALUE   fValue;
                    TimeStampType   fDataCapturedAt;

                    myVal_ (VALUE&& v, TimeStampType t)
                        : fValue (std::move (v))
                        , fDataCapturedAt (t)
                    {
                    }

					// @todo
                    //tmphack cuz Stroika Mapping requires operator== (to be fixed soon)
                    bool operator==(const myVal_& rhs) const
                    {
                        return fValue == rhs.fValue;
                    }
                };

            private:
                Mapping<KEY, myVal_>  fMap_;
            };


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CallerStalenessCache.inl"

#endif  /*_Stroika_Foundation_Cache_CallerStalenessCache_h_*/
