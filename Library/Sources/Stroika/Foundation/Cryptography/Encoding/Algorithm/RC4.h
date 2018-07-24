/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_RC4_h_
#define _Stroika_Foundation_Cryptography_RC4_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Configuration/Common.h"
#include "../../../Memory/BLOB.h"
#include "../../../Streams/InputStream.h"
#include "../../../Streams/OutputStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   REDO THIS API - USELESS AS IS. Pass in DervivedKey object - but only after I've made it
 *              more portable.
 *
 *
 *
 */

namespace Stroika::Foundation::Cryptography::Encoding::Algorithm {

#if qHasFeature_OpenSSL
    /**
     */
    Streams::InputStream<Memory::Byte>::Ptr DecodeRC4 (const Memory::BLOB& key, const Streams::InputStream<Memory::Byte>::Ptr& in);
    Memory::BLOB                            DecodeRC4 (const Memory::BLOB& key, const Memory::BLOB& in);
#endif

#if qHasFeature_OpenSSL
    /**
     */
    Streams::InputStream<Memory::Byte>::Ptr EncodeRC4 (const Memory::BLOB& key, const Streams::InputStream<Memory::Byte>::Ptr& in);
    Memory::BLOB                            EncodeRC4 (const Memory::BLOB& key, const Memory::BLOB& in);
#endif

#if qHasFeature_OpenSSL
    /**
     */
    Streams::OutputStream<Memory::Byte>::Ptr RC4Decoder (const Memory::BLOB& key, const Streams::OutputStream<Memory::Byte>::Ptr& out);
#endif

#if qHasFeature_OpenSSL
    /**
     */
    Streams::OutputStream<Memory::Byte>::Ptr RC4Encoder (const Memory::BLOB& key, const Streams::OutputStream<Memory::Byte>::Ptr& out);
#endif
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_RC4_h_*/
