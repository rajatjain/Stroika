/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_BinaryFileInputStream_h_
#define _Stroika_Foundation_IO_FileSystem_BinaryFileInputStream_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Streams/BinaryInputStream.h"



/**
 *  \file
 *
 * TODO:
 *      @todo   Unclear if we need the mutexes here. Probably yes (necause our API promises re-entrancy but I'm unclear
 *              on filessytem reads/writes).
 *
 *      @todo   This code is just a first (probably working) draft. But it needs cleanup. Review
 *              older FileReader/FileWriter code - and see if the windows code should use CreateFile instead of s_open...
 *
 *      @todo   We need a BinaryFileInputOutputStream
 *
 *      @todo   Document these are not buffered, (well document). Document how easy to wrap buffered stream around.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                using   Characters::String;


                /**
                 */
                class BinaryFileInputStream : public Streams::BinaryInputStream {
                private:
                    typedef BinaryInputStream   inherited;
                private:
                    class   Rep_;
                public:
                    BinaryFileInputStream (const String& fileName);
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
#include    "BinaryFileInputStream.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_BinaryFileInputStream_h_*/
