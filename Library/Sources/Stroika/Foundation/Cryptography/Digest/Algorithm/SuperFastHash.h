/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_SuperFastHash_h_
#define _Stroika_Foundation_Cryptography_Digest_SuperFastHash_h_ 1

#include "../../../StroikaPreComp.h"

#include <cstdint>

#include "../Digester.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  @see    http://www.azillionmonkeys.com/qed/hash.html
 *
 * TODO:
 *      @todo   See if we need to wrap this whole file in #ifndef for some 'license' issue, since I'm not sure
 *              this associated license is OK...
 *
 */

namespace Stroika::Foundation::Cryptography::Digest {

    namespace Algorithm {
        // Just a name to select template implementation
        struct SuperFastHash {
        };
        template <>
        struct DigesterDefaultTraitsForAlgorithm<SuperFastHash> {
            using ReturnType = uint32_t;
        };
    }

    template <>
    struct Digester<Algorithm::SuperFastHash, uint32_t> {
        using ReturnType = uint32_t;

        static ReturnType ComputeDigest (const Streams::InputStream<std::byte>::Ptr& from);
        static ReturnType ComputeDigest (const std::byte* from, const std::byte* to);
        static ReturnType ComputeDigest (const BLOB& from);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SuperFastHash.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_SuperFastHash_h_*/
