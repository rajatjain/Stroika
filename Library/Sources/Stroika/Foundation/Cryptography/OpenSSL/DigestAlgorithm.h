/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_DigestAlgorithm_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_DigestAlgorithm_h_ 1

#include    "../../StroikaPreComp.h"

#if     qHasFeature_OpenSSL
extern "C" {
    using  EVP_MD   =   struct env_md_st;
}
#endif

#include    "../../Configuration/Common.h"
#include    "../../Execution/StringException.h"
#include    "../../Memory/Common.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   AddLookupByName functions.
 *
 *
 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace OpenSSL {


#if     qHasFeature_OpenSSL
                /**
                 *  ROUGH DRAFT
                 */
                enum    class   DigestAlgorithm  {
                    eDSS,
                    eMD5,
                    eSHA1,
                    eSHA224,
                    eSHA256,

                    Stroika_Define_Enum_Bounds(eDSS, eSHA256)
                };
#endif



#if     qHasFeature_OpenSSL
                /**
                 *  \req valid algorithm from above enum, and \ens not nullptr.
                 *
                 *  \note - the returned pointer is immutable, and the data remains valid until the end of the program.
                 */
                const EVP_MD*   Convert2OpenSSL (DigestAlgorithm digestAlgorithm);
#endif


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "DigestAlgorithm.inl"
#if     qHasFeature_OpenSSL
#if     !qCompilerAndStdLib_constexpr_Buggy
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            template<>
            const EnumNames<Cryptography::OpenSSL::DigestAlgorithm>   DefaultNames<Cryptography::OpenSSL::DigestAlgorithm>::k {
                Configuration::EnumNames<DigestAlgorithm>::BasicArrayInitializer {{
                        { Cryptography::OpenSSL::DigestAlgorithm::eDSS, L"eDSS" },
                        { Cryptography::OpenSSL::DigestAlgorithm::eMD5, L"eMD5" },
                        { Cryptography::OpenSSL::DigestAlgorithm::eSHA1, L"eSHA1" },
                        { Cryptography::OpenSSL::DigestAlgorithm::eSHA224, L"eSHA224" },
                        { Cryptography::OpenSSL::DigestAlgorithm::eSHA256, L"eSHA256" },
                    }
                }
            };
        }
    }
}
#endif
#endif
#endif  /*_Stroika_Foundation_Cryptography_OpenSSL_DigestAlgorithm_h_*/
