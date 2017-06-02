/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Collection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief Collection_stdforward_list<T> is an std::forward_list (singly linked list)-based concrete implementation of the Collection<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T>
                class Collection_stdforward_list : public Collection<T> {
                private:
                    using inherited = Collection<T>;

                public:
                    Collection_stdforward_list ();
                    Collection_stdforward_list (const T* start, const T* end);
                    Collection_stdforward_list (const Collection<T>& collection);
                    Collection_stdforward_list (const Collection_stdforward_list<T>& collection);

                    nonvirtual Collection_stdforward_list<T>& operator= (const Collection_stdforward_list<T>& collection);

                private:
                    class Rep_;

                private:
                    nonvirtual void AssertRepValidType_ () const;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Collection_stdforward_list.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_ */
