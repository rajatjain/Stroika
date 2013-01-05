/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Execution/CriticalSection.h"
#include    "../Streams/BinaryInputStream.h"

#include    "BLOB.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;





Memory::BLOB::BasicRep_::BasicRep_ (const Byte* start, const Byte* end)
    : fData (end - start)
{
    memcpy (fData.begin (), start, end - start);
}

pair<const Byte*, const Byte*>   Memory::BLOB::BasicRep_::GetBounds () const
{
    return pair<const Byte*, const Byte*> (fData.begin (), fData.end ());
}






pair<const Byte*, const Byte*>   Memory::BLOB::ZeroRep_::GetBounds () const
{
    return pair<const Byte*, const Byte*> (nullptr, nullptr);
}








int  Memory::BLOB::compare (const BLOB& rhs) const
{
    pair<const Byte*, const Byte*>   l =   fRep_->GetBounds ();
    pair<const Byte*, const Byte*>   r =   rhs.fRep_->GetBounds ();

    size_t  lSize = l.second - l.first;
    size_t  rSize = r.second - r.first;
    size_t  nCommonBytes = min (lSize, rSize);
    int tmp = ::memcmp (l.first, r.first, nCommonBytes);
    if (tmp != 0) {
        return tmp;
    }
    // if tmp is zero, and same size - its really zero. But if lhs shorter than right, say lhs < right
    if (lSize == rSize) {
        return 0;
    }
    return (lSize < rSize) ? -1 : 1;
}


namespace {
    using namespace Streams;
    // @todo - add seekability to this helper!!!
    struct BLOBBINSTREAM_ : BinaryInputStream {
        BLOBBINSTREAM_ (const Memory::BLOB& b)
            : BinaryInputStream (_SharedIRep (DEBUG_NEW REP (b))) {
        }
        struct REP : BinaryInputStream::_IRep, public Seekable::_IRep  {
            REP (const Memory::BLOB& b)
                : fCur (b.begin ())
                , fStart (b.begin ())
                , fEnd (b.end ()) {
            }
            virtual size_t  Read (Byte* intoStart, Byte* intoEnd)  override {
                RequireNotNull (intoStart);
                RequireNotNull (intoEnd);
                Require (intoStart < intoEnd);
                size_t bytesToRead  =   intoEnd - intoStart;
                size_t bytesLeft    =   fEnd - fCur;
                bytesToRead = min (bytesLeft, bytesToRead);
                (void)::memcpy (intoStart, fCur, bytesToRead);
                fCur += bytesToRead;
                return bytesToRead;
            }
            virtual SeekOffsetType  GetOffset () const override {
                return fCur - fStart;
            }
            virtual SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) override {
                switch (whence) {
                    case    Whence::eFromStart: {
                            if (offset < 0) {
                                Execution::DoThrow (std::range_error ("seek"));
                            }
                            if (offset > (fEnd - fStart)) {
                                Execution::DoThrow (std::range_error ("seek"));
                            }
                            fCur = fStart + offset;
                        }
                        break;
                    case    Whence::eFromCurrent: {
                            Streams::SeekOffsetType         curOffset   =   fCur - fStart;
                            Streams::SignedSeekOffsetType   newOffset   =   curOffset + offset;
                            if (newOffset < 0) {
                                Execution::DoThrow (std::range_error ("seek"));
                            }
                            if (newOffset > (fEnd - fStart)) {
                                Execution::DoThrow (std::range_error ("seek"));
                            }
                            fCur = fStart + newOffset;
                        }
                        break;
                    case    Whence::eFromEnd: {
                            Streams::SeekOffsetType         curOffset   =   fCur - fStart;
                            Streams::SignedSeekOffsetType   newOffset   =   (fEnd - fStart) + offset;
                            if (newOffset < 0) {
                                Execution::DoThrow (std::range_error ("seek"));
                            }
                            if (newOffset > (fEnd - fStart)) {
                                Execution::DoThrow (std::range_error ("seek"));
                            }
                            fCur = fStart + newOffset;
                        }
                        break;
                }
                Ensure ((fStart <= fCur) and (fCur <= fEnd));
                return GetOffset ();
            }
            const Byte* fCur;
            const Byte* fStart;
            const Byte* fEnd;

            DECLARE_USE_BLOCK_ALLOCATION (REP);
        };
    };

}

template    <>
Streams::BinaryInputStream Memory::BLOB::As () const
{
    return BLOBBINSTREAM_ (*this);
}
