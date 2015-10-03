/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_CharacterDelimitedLines_Reader_h_
#define _Stroika_Foundation_DataExchange_CharacterDelimitedLines_Reader_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Containers/Sequence.h"
#include    "../../Containers/Set.h"
#include    "../../Streams/InputStream.h"

#include    "../VariantReader.h"
#include    "../VariantValue.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
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
                class   Reader : public VariantReader {
                private:
                    using   inherited   =   DataExchange::VariantReader;

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                public:
                    Reader (const Set<Character>& columnDelimiters);

                public:
                    nonvirtual  Iterable<Sequence<String>>  ReadMatrix (const Streams::InputStream<Memory::Byte>& in) const;
                    nonvirtual  Iterable<Sequence<String>>  ReadMatrix (const Streams::InputStream<Character>& in) const;
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
