/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_MD5_h_
#define _Stroika_Foundation_Cryptography_Digest_MD5_h_ 1

#include "../../../StroikaPreComp.h"

#include "../Digester.h"
#include "../ResultTypes.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Cryptography {
            namespace Digest {

                namespace Algorithm {
                    // Just a name to select template implementation
                    struct MD5 {
                    };
                    template <>
                    struct DigesterDefaultTraitsForAlgorithm<MD5> {
                        using ReturnType = Result128BitType;
                    };
                }

                /**
                 *  BLOB    blob2Hash   =   ...;
                 *  BLOB    b           =   Digester<Algorithm::MD5>::ComputeDigest (blob2Hash);
                 */
                template <>
                struct Digester<Algorithm::MD5, Result128BitType> {
                    using ReturnType = Result128BitType;

                    static ReturnType ComputeDigest (const Streams::InputStream<Byte>& from);
                    static ReturnType ComputeDigest (const Byte* from, const Byte* to);
                    static ReturnType ComputeDigest (const BLOB& from);
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
#include "MD5.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_MD5_h_*/
