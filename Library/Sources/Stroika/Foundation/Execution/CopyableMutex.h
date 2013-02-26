/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CopyableMutex_h_
#define _Stroika_Foundation_Execution_CopyableMutex_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>
#include    <mutex>

#include    "../Configuration/Common.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  Normal std::mutex instances are not copyable. But sometimes its handy to be able to expose a
             *  mutex to a number of different peices of code, those other peices of code must be told what
             *  mutex to share.
             *
             *  In essence - CopyableMutex simple creates a pointer to the mutex and copies that pointer, but then
             *  acts like a mutex in providing a lock/unlock API.
             */
            template    <typename   REAL_MUTUX = recursive_mutex>
            class CopyableMutex {
            public:
                CopyableMutex ();

            public:
                nonvirtual  void    lock ();
                nonvirtual  void    unlock ();

            private:
                shared_ptr<REAL_MUTUX>  fCritSec_;
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_CopyableMutex_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CopyableMutex.inl"
