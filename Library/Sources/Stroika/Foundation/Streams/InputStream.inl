/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStream_inl_
#define _Stroika_Foundation_Streams_InputStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {



            /*
             ********************************************************************************
             **************************** InputStream<ELEMENT_TYPE> *************************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            inline  InputStream<ELEMENT_TYPE>::InputStream (const _SharedIRep& rep)
                : Stream (rep)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  InputStream<ELEMENT_TYPE>::InputStream (nullptr_t)
                : Stream (nullptr)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  auto    InputStream<ELEMENT_TYPE>::_GetRep () const -> _SharedIRep
            {
                return dynamic_pointer_cast<_IRep> (Stream::_GetRep ());
            }
            template    <typename ELEMENT_TYPE>
            inline  SeekOffsetType  InputStream<ELEMENT_TYPE>::GetOffset () const
            {
                return _GetRep ()->GetOffset ();
            }
            template    <typename ELEMENT_TYPE>
            SeekOffsetType  InputStream<ELEMENT_TYPE>::GetOffsetToEndOfStream () const
            {
                SeekOffsetType  savedReadFrom = GetOffset ();
                SeekOffsetType  size =  Seek (Whence::eFromEnd, 0);
                Seek (Whence::eFromStart, savedReadFrom);
                Assert (size >= savedReadFrom);
                size -= savedReadFrom;
                return size;
            }
            template    <typename ELEMENT_TYPE>
            inline  SeekOffsetType    InputStream<ELEMENT_TYPE>::Seek (SignedSeekOffsetType offset) const
            {
                return _GetRep ()->Seek (Whence::eFromStart, offset);
            }
            template    <typename ELEMENT_TYPE>
            inline  SeekOffsetType    InputStream<ELEMENT_TYPE>::Seek (Whence whence, SignedSeekOffsetType offset) const
            {
                return _GetRep ()->Seek (whence, offset);
            }
            template    <typename ELEMENT_TYPE>
            inline  auto  InputStream<ELEMENT_TYPE>::Read () const -> Memory::Optional<ElementType> {
                ElementType b {};
                RequireNotNull (_GetRep ().get ());
                if (_GetRep ()->Read (&b, &b + 1) == 0)
                {
                    return Memory::Optional<ElementType> ();
                }
                else {
                    return b;
                }
            }
            template    <typename ELEMENT_TYPE>
            inline  size_t  InputStream<ELEMENT_TYPE>::Read (ElementType* intoStart, ElementType* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetRep ().get ());
                return _GetRep ()->Read (intoStart, intoEnd);
            }
            template    <typename ELEMENT_TYPE>
            size_t  InputStream<ELEMENT_TYPE>::Read (SeekOffsetType* offset, ElementType* intoStart, ElementType* intoEnd) const
            {
                SeekOffsetType  savedOffset =   GetOffset ();
                Seek (*offset);
                size_t  result = Read (intoStart, intoEnd);
                Seek (savedOffset);
                *offset += result;
                return result;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_InputStream_inl_*/
