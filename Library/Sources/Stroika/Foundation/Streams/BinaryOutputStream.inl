/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryOutputStream_inl_
#define _Stroika_Foundation_Streams_BinaryOutputStream_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            //  class   BinaryOutputStream
            inline  BinaryOutputStream::BinaryOutputStream (const shared_ptr<_IRep>& rep)
                : BinaryStream (rep)
            {
            }
            inline  shared_ptr<BinaryOutputStream::_IRep>   BinaryOutputStream::GetRep () const
            {
                return dynamic_pointer_cast<BinaryOutputStream::_IRep> (BinaryStream::GetRep ());
            }
            inline  void    BinaryOutputStream::Write (const Byte* start, const Byte* end)
            {
                RequireNotNull (start);
                Require ((end - start) >= 1);
                RequireNotNull (GetRep ().get ());
                GetRep ()->Write (start, end);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryOutputStream_inl_*/
