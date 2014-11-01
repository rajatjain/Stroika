/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_CharacterDelimitedLines_Reader_h_
#define _Stroika_Foundation_DataExchange_CharacterDelimitedLines_Reader_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Containers/Sequence.h"
#include    "../../Containers/Set.h"
#include    "../../Streams/BinaryInputStream.h"
#include    "../../Streams/TextInputStream.h"

#include    "../Reader.h"
#include    "../VariantValue.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Early-Alpha">Early-Alpha</a>
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   CharacterDelimitedLines {


                using   Containers::Sequence;
                using   Containers::Set;
                using   Characters::Character;
                using   Characters::String;
                using   Traversal::Iterable;


                /**
                 *  @todo add example usage (and docs).
                 *
                 *  This COULD be easily used to read CSV files, for example.
                 */
                class   Reader : public DataExchange::Reader {
                private:
                    using   inherited   =   DataExchange::Reader;

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                public:
                    Reader (const Set<Character>& columnDelimiters);

                public:
                    nonvirtual  Iterable<Sequence<String>>  ReadAs2DArray (const Streams::BinaryInputStream& in) const;
                    nonvirtual  Iterable<Sequence<String>>  ReadAs2DArray (const Streams::TextInputStream& in) const;

                public:
                    /*
                     *  assumes 2D-array input, and creates key-value pairs
                     */
                    nonvirtual  Mapping<String, String>  ReadAsMapping (const Streams::BinaryInputStream& in) const;
                    nonvirtual  Mapping<String, String>  ReadAsMapping (const Streams::TextInputStream& in) const;
                };


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_DataExchange_CharacterDelimitedLines_Reader_h_*/
