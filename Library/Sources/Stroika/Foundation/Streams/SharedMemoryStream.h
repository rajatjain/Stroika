/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_SharedMemoryStream_h_
#define _Stroika_Foundation_Streams_SharedMemoryStream_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Memory/BLOB.h"

#include "InputOutputStream.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo https://stroika.atlassian.net/browse/STK-584 - 
 *            provide option for SharedMemoryStream so not seekable - less memory
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             *  \brief  SharedMemoryStream<> is an InputOutputStream<> like MemoryStream<> but supporting concurrency
             *
             *  SharedMemoryStream is Seekable.
             *
             *  Since SharedMemoryStream keeps its data all in memory, it has the limitation that
             *  attempts to seek or write more than will fit in RAM will fail (with an exception).
             *
             *  \note   SharedMemoryStream is suitable for synchonized reading and writing between two threads (producer / consumer pattern).
             *          Reads will block at the end of the stream until some thread calls SharedMemoryStream><>::CloseWrite ()
             *
             *          @see MemoryStream
             *
             *  \note currently all SharedMemoryStreams are seekable, but future versions may allow options to be specified
             *        so they are not seekable
             *
             *  @see ExternallyOwnedMemoryInputStream
             *
             *  \par Example Usage
             *      \code
             *           SharedMemoryStream<unsigned int>::Ptr pipe = SharedMemoryStream<unsigned int>::New ();
             *           unsigned                              sum{};
             *           static constexpr unsigned int         kStartWith{1};
             *           static constexpr unsigned int         kUpToInclusive_{1000};
             *           Thread::Ptr                           consumer = Thread::New ([&]() {
             *                  while (auto o = pipe.Read ()) {
             *                      sum += *o;
             *                  }
             *              },
             *              Thread::eAutoStart);
             *           Thread::Ptr producer = Thread::New ([&]() {
             *                  for (unsigned int i = kStartWith; i <= kUpToInclusive_; i++) {
             *                      pipe.Write (i);
             *                  };
             *                  pipe.CloseWrite ();    // critical or consumer hangs on final read
             *              },
             *              Thread::eAutoStart);
             *           Thread::WaitForDone ({consumer, producer});
             *           Assert (sum == (1 + kUpToInclusive_) * (kUpToInclusive_ - 1 + 1) / 2); // not a race
             *      \endcode
             *
             */
            template <typename ELEMENT_TYPE>
            class SharedMemoryStream : public InputOutputStream<ELEMENT_TYPE> {
            public:
                /**
                 *  'SharedMemoryStream' is a quasi-namespace: use Ptr or New () members.
                 */
                SharedMemoryStream ()                          = delete;
                SharedMemoryStream (const SharedMemoryStream&) = delete;

            public:
                class Ptr;

            public:
                /**
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Rep-Inside-Ptr-Is-Internally-Syncrhonized">Rep-Inside-Ptr-Is-Internally-Syncrhonized</a>
                 */
                static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized = Execution::eInternallySynchronized);
                static Ptr New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
                static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
                template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if<is_same<TEST_TYPE, Memory::Byte>::value>::type>
                static Ptr New (const Memory::BLOB& blob);
                template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if<is_same<TEST_TYPE, Memory::Byte>::value>::type>
                static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const Memory::BLOB& blob);

            private:
                class Rep_;
            };

            /**
             *  Ptr is a copyable smart pointer to a SharedMemoryStream.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
             */
            template <typename ELEMENT_TYPE>
            class SharedMemoryStream<ELEMENT_TYPE>::Ptr : public InputOutputStream<ELEMENT_TYPE>::Ptr {
            private:
                using inherited = typename InputOutputStream<ELEMENT_TYPE>::Ptr;

            public:
                /**
                */
                Ptr ()                = default;
                Ptr (const Ptr& from) = default;

            private:
                Ptr (const shared_ptr<Rep_>& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;

            public:
                [[deprecated ("USE CloseWrite () - deprecated v2.0a213")]] void CloseForWrites ()
                {
                    CloseWrite (false);
                }

            public:
                /**
                 *  Convert the current contents of this SharedMemoryStream into one of the "T" representations.
                 *  T can be one of:
                 *      o   vector<ElementType>
                 *
                 *  And if ElementType is Memory::Byte, then T can also be one of:
                 *      o   Memory::BLOB
                 *      o   string
                 *
                 *  And if ElementType is Characters::Character, then T can also be one of:
                 *      o   String
                 */
                template <typename T>
                nonvirtual T As () const;

            private:
                friend class SharedMemoryStream;
            };

            template <>
            template <>
            Memory::BLOB SharedMemoryStream<Memory::Byte>::Ptr::As () const;
            template <>
            template <>
            string SharedMemoryStream<Memory::Byte>::Ptr::As () const;
            template <>
            template <>
            vector<Memory::Byte> SharedMemoryStream<Memory::Byte>::Ptr::As () const;

            template <>
            template <>
            Characters::String SharedMemoryStream<Characters::Character>::Ptr::As () const;
            template <>
            template <>
            vector<Characters::Character> SharedMemoryStream<Characters::Character>::Ptr::As () const;
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SharedMemoryStream.inl"

#endif /*_Stroika_Foundation_Streams_SharedMemoryStream_h_*/
