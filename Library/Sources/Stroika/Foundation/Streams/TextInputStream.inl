/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextInputStream_inl_
#define _Stroika_Foundation_Streams_TextInputStream_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            //  class   BinaryInputStream::_IRep
            inline  TextInputStream::_IRep::_IRep ()
            {
            }

            //  class   TextInputStream
            inline  TextInputStream::TextInputStream (const _SharedIRep& rep)
                : TextStream (rep)
            {
            }
            inline  TextInputStream::_SharedIRep  TextInputStream::_GetRep () const
            {
                return dynamic_pointer_cast<_IRep> (TextStream::_GetRep ());
            }
            inline  size_t  TextInputStream::Read (Character* intoStart, Character* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetRep ().get ());
                return _GetRep ()->_Read (intoStart, intoEnd);
            }
            inline  Character   TextInputStream::Read () const
            {
                Character   c;
                if (Read (&c, &c + 1) == 1) {
                    return c;
                }
                return '\0';
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextInputStream_inl_*/
