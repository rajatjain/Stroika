/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicBinaryInputOutputStream_h_
#define _Stroika_Foundation_Streams_BasicBinaryInputOutputStream_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/BLOB.h"

#include    "BinaryInputOutputStream.h"
#include    "MemoryStream.h"
#include    "Seekable.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


//@todo pragma obsolete

#if 1
            using BasicBinaryInputOutputStream = MemoryStream;
#else

            /**
             *  \brief  Simplest to use BinaryInputOutputStream; BasicBinaryInputOutputStream can be written to, and then the BLOB of data retrieved.
             *
             *  BinaryInputOutputStream is threadsafe - meaning Read () or Write() can safely be called from
             *  multiple threads at a time freely, and the results are interleaved without corruption. There
             *  is no guarantee of ordering, but one will finish before the next starts writing (so if they
             *  are writing structured messages, those will remain in-tact).
             *
             *  BasicBinaryInputOutputStream is Seekable.
             *
             *  Since BasicBinaryInputOutputStream keeps its data all in memory, it has the limitation that
             *  attempts to seek or write more than will fit in RAM will fail (with an exception).
             *
             *  NB: This class COULD have been called MemoryStream, or MemoryInputOutputStream.
             */
            class   BasicBinaryInputOutputStream : public BinaryInputOutputStream {
            private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                class   IRep_;
            public:
                BasicBinaryInputOutputStream ();

            public:
                /**
                 *  Convert the current contents of this BasicBinaryInputOutputStream into one of the "T" representations.
                 *  T can be one of:
                 *      Memory::BLOB
                 *      vector<Byte>
                 *      string
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
            };


            template    <>
            Memory::BLOB    BasicBinaryInputOutputStream::As () const;
            template    <>
            vector<Byte>    BasicBinaryInputOutputStream::As () const;
            template    <>
            string          BasicBinaryInputOutputStream::As () const;
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "BasicBinaryInputOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BasicBinaryInputOutputStream_h_*/
