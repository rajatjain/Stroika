/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_MemoryMappedFileReader_h_
#define _Stroika_Foundation_IO_FileSystem_MemoryMappedFileReader_h_  1

#include    "../../StroikaPreComp.h"

#if         qPlatform_Windows
#include    <Windows.h>
#endif

#include    "Common.h"



/**
 * TODO:
 *
 *          @todo   implement POSIX mmap()
 *
 *          @todo   Lose GetFileStart/GetFileEnd - just begin/end/size()
 *
 *          @todo   Add API variant that allows multiple sub-regions of a file to be mapped.
 *
 *          @todo   Consider adding a writable MemoryMappedFileReader variant
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                /**
                 *  MemoryMappedFileReader is a utility to allow memory mapping of a file.
                 */
                class   MemoryMappedFileReader {
                public:
                    MemoryMappedFileReader (const String& fileName);
                    ~MemoryMappedFileReader ();

                private:
                    NO_COPY_CONSTRUCTOR (MemoryMappedFileReader);
                    NO_ASSIGNMENT_OPERATOR (MemoryMappedFileReader);

                public:
                    // OBSOLETE APIS
                    const Byte* GetFileStart () const;
                    const Byte* GetFileEnd () const;
                    size_t      GetFileSize () const;

                public:
                    /**
                     */
                    nonvirtual  const Byte* begin () const;

                public:
                    /**
                     */
                    nonvirtual  const Byte* end () const;

                public:
                    /**
                     */
                    nonvirtual  size_t size () const;

                private:
                    const Byte* fFileDataStart_;
                    const Byte* fFileDataEnd_;
#if         qPlatform_Windows
                    HANDLE      fFileHandle_;
                    HANDLE      fFileMapping_;
#endif
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
#include    "MemoryMappedFileReader.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_MemoryMappedFileReader_h_*/
