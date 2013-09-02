/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_TemporaryFile_h_
#define _Stroika_Foundation_IO_FileSystem_TemporaryFile_h_  1

#include    "../../StroikaPreComp.h"

#include    <set>
#include    <vector>

#if         qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../Characters/TChar.h"
#include    "../../Configuration/Common.h"
#include    "../../Debug/Assertions.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/Thread.h"
#include    "../../Time/DateTime.h"

#include    "../FileAccessMode.h"
#include    "Common.h"
#include    "Directory.h"



/**
 * TODO:
 *      @todo   Very rough draft based on code used in HealthFrame. Major cleanups required.
 *
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                namespace   Private {
                    class   FileUtilsModuleData_;
                }
                class   AppTempFileManager {
                private:
                    AppTempFileManager ();
                    ~AppTempFileManager ();

                public:
                    static  AppTempFileManager& Get ();
                public:
                    nonvirtual  String GetMasterTempDir () const;

                public:
                    nonvirtual  String GetTempFile (const String& fileNameBase);
                    nonvirtual  String GetTempDir (const String& fileNameBase);

                private:
                    String fTmpDir;

                private:
                    friend  class   Private::FileUtilsModuleData_;
                };

#if 0

                class   TempFileLibrarian {
                public:
                    TempFileLibrarian (const String& privateDirectory, bool purgeDirectory, bool makeTMPDIRRel = true, bool deleteFilesOnDescruction = true);
                    ~TempFileLibrarian ();

                public:
                    nonvirtual  String GetTempFile (const String& fileNameBase);
                    nonvirtual  String GetTempDir (const String& fileNameBase);

                private:
                    set<String>                 fFiles;
                    String                      fPrivateDirectory;
                    bool                        fMakeTMPDIRRel;
                    bool                        fDeleteFilesOnDescruction;
                    mutex                       fCriticalSection_;
                };
#endif


                class   ScopedTmpDir {
                private:
                    ScopedTmpDir (const ScopedTmpDir&);
                    const ScopedTmpDir& operator= (const ScopedTmpDir&);
                public:
                    ScopedTmpDir (const String& fileNameBase);
//                    ScopedTmpDir (TempFileLibrarian& tfl, const String& fileNameBase);
                    ~ScopedTmpDir ();
                public:
                    nonvirtual  operator Directory () const;
                    nonvirtual  Directory GetDirectory () const;
                private:
                    Directory     fTmpDir;
                };

                class   ScopedTmpFile {
                private:
                    ScopedTmpFile (const ScopedTmpFile&);
                    const ScopedTmpFile& operator= (const ScopedTmpFile&);
                public:
                    ScopedTmpFile (const String& fileNameBase);
                    //                  ScopedTmpFile (TempFileLibrarian& tfl, const String& fileNameBase);
                    ~ScopedTmpFile ();
                public:
                    operator String () const;
                private:
                    String fTmpFile;
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
#include    "TemporaryFile.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_TemporaryFile_h_*/
