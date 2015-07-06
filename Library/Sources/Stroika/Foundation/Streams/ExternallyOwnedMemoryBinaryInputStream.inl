/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_inl_
#define _Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_inl_ 1


#include    "../Traversal/Iterator.h"


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /*
             ********************************************************************************
             ****************** ExternallyOwnedMemoryBinaryInputStream **********************
             ********************************************************************************
             */
            template    <typename   BYTE_RANDOM_ACCESS_ITERATOR>
            inline  ExternallyOwnedMemoryBinaryInputStream::ExternallyOwnedMemoryBinaryInputStream (BYTE_RANDOM_ACCESS_ITERATOR start, BYTE_RANDOM_ACCESS_ITERATOR end)
                : ExternallyOwnedMemoryBinaryInputStream (static_cast<const Memory::Byte*> (Traversal::Iterator2Pointer (start)), static_cast<const Memory::Byte*> (Traversal::Iterator2Pointer (start) + (end - start)))
            {
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_inl_*/
