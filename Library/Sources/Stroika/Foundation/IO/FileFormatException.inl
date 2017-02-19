/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileFormatException_inl_
#define _Stroika_Foundation_IO_FileFormatException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace IO {

            inline String FileFormatException::GetFileName () const
            {
                return fFileName_;
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_FileFormatException_inl_*/
