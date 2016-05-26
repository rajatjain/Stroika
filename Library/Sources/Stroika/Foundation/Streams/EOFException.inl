/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_EOFException_inl_
#define _Stroika_Foundation_Streams_EOFException_inl_  1


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
             **************************** Streams::EOFException *****************************
             ********************************************************************************
             */
            inline  EOFException::EOFException ()
                : EOFException (false)
            {
            }
            inline  bool    EOFException::GetPartialReadCompleted () const
            {
                return fPartialReadCompleted_;
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_EOFException_inl_*/
