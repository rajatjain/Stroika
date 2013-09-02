/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileUtils_h_
#define _Stroika_Foundation_IO_FileSystem_FileUtils_h_  1

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
 *
 *          o   This file is obsolete, and will be gradually replaced - moving its contents out to other modules

 **         o   Great IDEA FROM KDJ. I USED TO support abstractfilesystem in stroika. Maybe in old code base. Used for Win32 FS versus UNIX versus MacOS FS.
 *              KDJ's point is this idea should be resurected cuz its useful for stuff like TARFILEs and ZIPFILES or ISO files which act like a FS, and can be treated
 *              that way.
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                // doesn't actually open the file. It's purely advisory. But its helpful to assure
                // a consistent set of error reporting across different styles of opens. Just call this first,
                // and it will throw exceptions if the file doesn't exist, or has access privileges issues.
                void    CheckFileAccess (const String& fileFullPath, bool checkCanRead = true, bool checkCanWrite = false);


                String ResolveShortcut (const String& path2FileOrShortcut);

                String FileSizeToDisplayString (FileOffset_t bytes);

                FileOffset_t    GetFileSize (const String& fileName);
                DateTime        GetFileLastModificationDate (const String& fileName);
                DateTime        GetFileLastAccessDate (const String& fileName);

                void    SetFileAccessWideOpened (const String& filePathName);


                /*
                 * CreateDirectory makes sure the directory with the given name exists on the filesystem. If it already exists, this is NOT an error.
                 * If it already exists, but is a FILE (anything but a directory) - then it is an error).
                 *
                 * If createParentComponentsIfNeeded - the function braks the directory paoth into parts, and recursively applies itself to each segment from
                 * the root down.
                 *
                 * The net effect - this makes sure the given DIRECTORY exists or raises an exception.
                 *
                 *          TODO:
                 *              (o) We need an overload which takes the directory permissions as argument.
                 */
                void    CreateDirectory (const String& directoryPath, bool createParentComponentsIfNeeded = true);


                void    CreateDirectoryForFile (const String& filePath);


                String GetVolumeName (const String& driveLetterAbsPath);

                bool    FileExists (const String& filePath);

                vector<String> FindFiles (const String& path, const String& fileNameToMatch = L"*.*");


                vector<String> FindFilesOneDirUnder (const String& path, const String& fileNameToMatch = L"*.*");


                void    DeleteAllFilesInDirectory (const String& path, bool ignoreErrors = true);


                void    CopyFile (const String& srcFile, const String& destPath);


                // COULD be made portable but alot of changes needed
#if         qPlatform_Windows
                class   DirectoryChangeWatcher {
                private:
                    DirectoryChangeWatcher (const DirectoryChangeWatcher&);     // declare but not defined, to prevent copies
                    void operator= (const DirectoryChangeWatcher&);             // ''
                public:
                    DirectoryChangeWatcher (const String& directoryName, bool watchSubTree = false, DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE);
                    virtual ~DirectoryChangeWatcher ();

                protected:
                    virtual void    ValueChanged ();

                private:
                    static  void    ThreadProc (void* lpParameter);

                private:
                    String              fDirectory;
                    bool                fWatchSubTree;
                    Execution::Thread   fThread;
                    HANDLE              fDoneEvent;
                    HANDLE              fWatchEvent;
                    bool                fQuitting;
                };
#endif



                class   DirectoryContentsIterator {
                public:
                    DirectoryContentsIterator (const String& pathExpr);        // can include wildcards - see ::FindFirstFile docs
                    ~DirectoryContentsIterator ();

                private:
                    NO_COPY_CONSTRUCTOR (DirectoryContentsIterator);
                    NO_ASSIGNMENT_OPERATOR (DirectoryContentsIterator);

                public:
                    nonvirtual  bool    NotAtEnd () const;
                    nonvirtual  String  operator *() const;
                    nonvirtual  void    operator++ ();

#if         qPlatform_Windows
                private:
                    HANDLE          fHandle;
                    WIN32_FIND_DATA fFindFileData;
                    String          fDirectory;
#endif
                };


                // Should be in a PLATFORM_WINDOWS subfile or sub-namespace... And DOCUMENT!!!!
#if         qPlatform_Windows
                struct  AdjustSysErrorMode {
                    static  UINT    GetErrorMode ();
                    AdjustSysErrorMode (UINT newErrorMode);
                    ~AdjustSysErrorMode ();
                    UINT    fSavedErrorMode;
                };
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
#include    "FileUtils.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_FileUtils_h_*/
