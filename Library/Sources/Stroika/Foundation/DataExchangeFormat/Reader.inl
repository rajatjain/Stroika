/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_Reader_inl_
#define _Stroika_Foundation_DataExchangeFormat_Reader_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            /*
             ********************************************************************************
             ************************* DataExchangeFormat::Reader ***************************
             ********************************************************************************
             */
            inline  Reader::Reader (shared_ptr<_IRep> rep)
                : fRep_ (rep)
            {
            }
            inline  Memory::VariantValue    Reader::Read (const Streams::BinaryInputStream& in)
            {
                return fRep_->Read (in);
            }
            inline  Memory::VariantValue    Reader::Read (const Streams::TextInputStream& in)
            {
                return fRep_->Read (in);
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_Reader_inl_*/
