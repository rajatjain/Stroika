/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Bits_h_
#define _Stroika_Foundation_Memory_Bits_h_ 1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"



/**
 *  \file
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /**
             *  bitNumber's start with 0, not 1.
             *
             *  EXAMPLES:
             *      VerifyTestResult (Bit (0) == 0x1);
             *      VerifyTestResult (Bit (1) == 0x2);
             *      VerifyTestResult (Bit (3) == 0x8);
             *      VerifyTestResult (Bit (15) == 0x8000);
             *      VerifyTestResult (Bit<int> (1, 2) == 0x6);
             *      VerifyTestResult (Bit<int> (1, 2, 15) == 0x8006);
             */
            template    <typename INT_TYPE = unsigned int>
            constexpr   INT_TYPE    Bit (unsigned int bitNumber);
            template    <typename INT_TYPE, typename... BIT_ARGS>
            constexpr   INT_TYPE    Bit (unsigned int bitNumber, const BIT_ARGS& ... args);


            /**
             *  Capture the bits from 'bitField' - starting at bit 'startOffset' (zero-based),
             *  extending to endOffset (also zero based). The number of bits captured
             *  is endOffset-startOffset, so:
             *      \req    startOffset < endOffset
             *
             *  \par Example Usage
             *      \code
             *      VerifyTestResult (BitSubstring_NEW (0x3, 0, 1) == 1);
             *      VerifyTestResult (BitSubstring_NEW (0x3, 1, 2) == 1);
             *      VerifyTestResult (BitSubstring_NEW (0x3, 2, 3) == 0);
             *      VerifyTestResult (BitSubstring_NEW (0x3, 0, 3) == 0x3);
             *      VerifyTestResult (BitSubstring_NEW (0xff, 0, 8) == 0xff);
             *      VerifyTestResult (BitSubstring_NEW (0xff, 8, 16) == 0x0);
             *      \endcode
             *
             *  \note   This was previously named TakeNBitsFrom()
             */
            template    <typename INT_TYPE>
            constexpr   INT_TYPE    BitSubstring_NEW (INT_TYPE bitField, unsigned int startOffset, unsigned int endOffset);
            template    <typename INT_TYPE>
            _Deprecated_ ("USE BitSubstring_NEW temporarily during API transition")            constexpr   INT_TYPE    BitSubstring (INT_TYPE bitField, unsigned int offset, unsigned int nBits)
            {
                return BitSubstring_NEW (bitField, offset, offset + nBits);
            }


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Bits.inl"

#endif  /*_Stroika_Foundation_Memory_Bits_h_*/
