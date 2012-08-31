/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "BufferedBinaryInputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;




class   BufferedBinaryInputStream::IRep_ : public BinaryInputStream::_IRep {
public:
    IRep_ (const BinaryInputStream::_SharedIRep& realIn)
        : BinaryInputStream::_IRep ()
        , fRealIn_ (realIn) {
    }

    virtual size_t          Read (Byte* intoStart, Byte* intoEnd) override {
        return fRealIn_->Read (intoStart, intoEnd);
    }

private:
    BinaryInputStream::_SharedIRep    fRealIn_;
};






/*
 ********************************************************************************
 ************************ Streams::BinaryInputStream ****************************
 ********************************************************************************
 */
BufferedBinaryInputStream::BufferedBinaryInputStream (const shared_ptr<BinaryInputStream::_IRep>& realIn)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (realIn)))
{
}
