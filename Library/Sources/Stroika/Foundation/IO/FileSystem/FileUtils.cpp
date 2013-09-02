/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <sys/types.h>
#include    <sys/stat.h>
#include    <ctime>
#include    <limits>
#include    <fstream>
#include    <fcntl.h>
#include    <cstdio>

#if     qPlatform_Windows
#include    <aclapi.h>
#include    <io.h>
#include    <shlobj.h>
#include    <windows.h>
#elif   qPlatform_POSIX
#include    <unistd.h>
#endif

#include    "../../Characters/Format.h"
#include    "../../Characters/StringUtils.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#include    "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include    "../../Containers/Common.h"
#include    "../../Containers/STL/SetUtils.h"
#include    "../../Debug/Trace.h"
#include    "../../IO/FileAccessException.h"
#include    "../../IO/FileBusyException.h"
#include    "../../IO/FileFormatException.h"
#include    "../../Memory/SmallStackBuffer.h"
#include    "PathName.h"
#include    "WellKnownLocations.h"

#include    "FileUtils.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Containers::STL;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;
using   namespace   Stroika::Foundation::Memory;


#if     qPlatform_Windows
using   Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif



/*
 * Stuff  INSIDE try section raises exceptions. Catch and rethow SOME binding in a new filename (if none was known).
 * Otehr exceptions just ignore (so they auto-propagate)
 */
#define     CATCH_REBIND_FILENAMES_HELPER_(USEFILENAME) \
    catch (const FileBusyException& e) {    \
        if (e.GetFileName ().empty ()) {\
            Execution::DoThrow (FileBusyException (USEFILENAME));\
        }\
        Execution::DoReThrow ();\
    }\
    catch (const FileAccessException& e) {  \
        if (e.GetFileName ().empty ()) {\
            Execution::DoThrow (FileAccessException (USEFILENAME, e.GetFileAccessMode ()));\
        }\
        Execution::DoReThrow ();\
    }\
    catch (const FileFormatException& e) {  \
        if (e.GetFileName ().empty ()) {\
            Execution::DoThrow (FileFormatException (USEFILENAME));\
        }\
        Execution::DoReThrow ();\
    }\
     










/*
 ********************************************************************************
 ************************ FileSystem::CheckFileAccess ***************************
 ********************************************************************************
 */
void    FileSystem::CheckFileAccess (const String& fileFullPath, bool checkCanRead, bool checkCanWrite)
{
    // quick hack - not fully implemented - but since advsiory only - not too important...

    if (not FileExists (fileFullPath)) {
// FOR NOW - MIMIC OLD CODE - BUT FIX TO CHECK READ AND WRITE (AND BOTH) ACCESS DEPENDING ON ARGS) -- LGP 2009-08-15
        Execution::DoThrow (FileAccessException (fileFullPath, IO::FileAccessMode::eRead_FAM));
    }
}








/*
 ********************************************************************************
 ************************* FileSystem::ResolveShortcut **************************
 ********************************************************************************
 */
String FileSystem::ResolveShortcut (const String& path2FileOrShortcut)
{
#if     qPlatform_Windows
    // NB: this requires COM, and for now - I don't want the support module depending on the COM module,
    // so just allow this to fail if COM isn't initialized.
    //      -- LGP 2007-09-23
    //
    {
        SHFILEINFO   info;
        memset (&info, 0, sizeof (info));
        if (::SHGetFileInfo (path2FileOrShortcut.AsTString ().c_str (), 0, &info, sizeof (info), SHGFI_ATTRIBUTES) == 0) {
            return path2FileOrShortcut;
        }
        // not a shortcut?
        if (!(info.dwAttributes & SFGAO_LINK)) {
            return path2FileOrShortcut;
        }
    }

    // obtain the IShellLink interface
    IShellLink*     psl =   nullptr;
    IPersistFile*   ppf =   nullptr;
    try {
        if (FAILED (::CoCreateInstance (CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl))) {
            return path2FileOrShortcut;
        }
        if (SUCCEEDED (psl->QueryInterface (IID_IPersistFile, (LPVOID*)&ppf))) {
            if (SUCCEEDED (ppf->Load (path2FileOrShortcut.c_str (), STGM_READ))) {
                // Resolve the link, this may post UI to find the link
                if (SUCCEEDED (psl->Resolve(0, SLR_NO_UI))) {
                    TChar   path[MAX_PATH + 1];
                    memset (path, 0, sizeof (path));
                    if (SUCCEEDED (psl->GetPath (path, NEltsOf (path), nullptr, 0))) {
                        ppf->Release ();
                        ppf = nullptr;
                        psl->Release ();
                        psl = nullptr;
                        return String::FromTString (path);
                    }
                }
            }
        }
    }
    catch (...) {
        if (ppf != nullptr) {
            ppf->Release ();
        }
        if (psl != nullptr) {
            psl->Release ();
        }
        Execution::DoReThrow ();
    }
    if (ppf != nullptr) {
        ppf->Release ();
    }
    if (psl != nullptr) {
        psl->Release ();
    }
    return path2FileOrShortcut;
#else
    // interpret as slink follow - readlink
    AssertNotImplemented ();
    return String ();
#endif
}










/*
 ********************************************************************************
 ******************* FileSystem::FileSizeToDisplayString ************************
 ********************************************************************************
 */
String FileSystem::FileSizeToDisplayString (FileOffset_t bytes)
{
    if (bytes < 1000) {
        return Format (L"%d bytes", static_cast<int> (bytes));
    }
    else if (bytes < 1000 * 1024) {
        return Format (L"%.1f K", static_cast<double> (bytes) / 1024.0f);
    }
    else {
        return Format (L"%.1f MB", static_cast<double> (bytes) / (1024 * 1024.0f));
    }
}





/*
 ********************************************************************************
 *************************** FileSystem::GetFileSize ****************************
 ********************************************************************************
 */
FileOffset_t    FileSystem::GetFileSize (const String& fileName)
{
#if     qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA   fileAttrData;
    (void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
    Execution::Platform::Windows::ThrowIfFalseGetLastError (::GetFileAttributesEx (fileName.AsTString ().c_str (), GetFileExInfoStandard, &fileAttrData));
    return fileAttrData.nFileSizeLow + (static_cast<FileOffset_t> (fileAttrData.nFileSizeHigh) << 32);
#else
    AssertNotImplemented ();
    return 0;
#endif
}






/*
 ********************************************************************************
 ********************* FileSystem::GetFileLastModificationDate ******************
 ********************************************************************************
 */
DateTime        FileSystem::GetFileLastModificationDate (const String& fileName)
{
#if     qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA   fileAttrData;
    (void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
    ThrowIfFalseGetLastError (::GetFileAttributesEx (fileName.AsTString ().c_str (), GetFileExInfoStandard, &fileAttrData));
    return DateTime (fileAttrData.ftLastWriteTime);
#else
    AssertNotImplemented ();
    return DateTime ();
#endif
}






/*
 ********************************************************************************
 *********************** FileSystem::GetFileLastAccessDate **********************
 ********************************************************************************
 */
DateTime    FileSystem::GetFileLastAccessDate (const String& fileName)
{
#if     qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA   fileAttrData;
    (void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
    ThrowIfFalseGetLastError (::GetFileAttributesEx (fileName.AsTString ().c_str (), GetFileExInfoStandard, &fileAttrData));
    return DateTime (fileAttrData.ftLastAccessTime);
#else
    AssertNotImplemented ();
    return DateTime ();
#endif
}




/*
 ********************************************************************************
 ************************ FileSystem::SetFileAccessWideOpened *******************
 ********************************************************************************
 */
/*
 * Sets permissions for users on a given folder to full control
 *
 *      Add 'Everyone' to have FULL ACCESS to the given argument file
 *
 */
void    FileSystem::SetFileAccessWideOpened (const String& filePathName)
{
    try {
#if     qPlatform_Windows
        static  PACL pACL = nullptr;    // Don't bother with ::LocalFree (pACL); - since we cache keeping this guy around for speed
        if (pACL == nullptr) {
            PSID pSIDEveryone = nullptr;

            {
                // Specify the DACL to use.
                // Create a SID for the Everyone group.
                SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
                if (!::AllocateAndInitializeSid (&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSIDEveryone))  {
                    return;     // if this fails - perhaps old OS with no security - just fail silently...
                }
            }

            EXPLICIT_ACCESS ea[1];
            memset (&ea, 0, sizeof (ea));

            // Set FULL access for Everyone.
            ea[0].grfAccessPermissions = GENERIC_ALL;
            ea[0].grfAccessMode = SET_ACCESS;
            ea[0].grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
            ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
            ea[0].Trustee.ptstrName = (LPTSTR) pSIDEveryone;

            if (ERROR_SUCCESS != ::SetEntriesInAcl (NEltsOf (ea), ea, nullptr, &pACL)) {
                ::FreeSid (pSIDEveryone);
                return; // silently ignore errors - probably just old OS etc....
            }
            ::FreeSid (pSIDEveryone);
        }

        // Try to modify the object's DACL.
        DWORD dwRes  = SetNamedSecurityInfo(
                           const_cast<TChar*> (filePathName.AsTString ().c_str ()),          // name of the object
                           SE_FILE_OBJECT,              // type of object
                           DACL_SECURITY_INFORMATION,   // change only the object's DACL
                           nullptr, nullptr,                  // don't change owner or group
                           pACL,                        // DACL specified
                           nullptr
                       );                       // don't change SACL
        // ignore error from this routine for now  - probably means either we don't have permissions or OS too old to support...
#elif   qPlatform_POSIX
        ////TODO: Somewhat PRIMITIVE - TMPHACK
        if (filePathName.empty ()) {
            Execution::DoThrow (StringException (L"bad filename"));
        }
        struct  stat    s;
        ThrowErrNoIfNegative (::stat (filePathName.AsTString ().c_str (), &s));

        mode_t  desiredMode =   (S_IRUSR | S_IRGRP | S_IROTH) | (S_IWUSR | S_IWGRP | S_IWOTH);
        if (S_ISDIR (s.st_mode)) {
            desiredMode |= (S_IXUSR | S_IXGRP | S_IXOTH);
        }

        int result = 0;
        // Don't call chmod if mode is already open (because doing so could fail even though we already have what we wnat if were not the owner)
        if ((s.st_mode & desiredMode) != desiredMode) {
            result = chmod (filePathName.AsTString ().c_str (), desiredMode);
        }
        ThrowErrNoIfNegative (result);
#else
        AssertNotImplemented ();
#endif
    }
    CATCH_REBIND_FILENAMES_HELPER_(filePathName);
}








/*
 ********************************************************************************
 ************************ FileSystem::CreateDirectory ***************************
 ********************************************************************************
 */
void    FileSystem::CreateDirectory (const String& directoryPath, bool createParentComponentsIfNeeded)
{
    /*
     * TODO:
     *      (o)     This implementation is HORRIBLE!!!! Major cleanup required!
     */
    try {
#if     qPlatform_Windows
        if (createParentComponentsIfNeeded) {
            // walk path and break into parts, and from top down - try to create parent directory structure.
            // Ignore any failures - and just let the report of failure (if any must result) come from original basic
            // CreateDirectory call.
            size_t  index   =   directoryPath.find ('\\');
            while (index != -1 and index + 1 < directoryPath.length ()) {
                String parentPath = directoryPath.substr (0, index);
                IgnoreExceptionsForCall (CreateDirectory (parentPath, false));
                index = directoryPath.find ('\\', index + 1);
            }
        }

        if (not ::CreateDirectoryW (directoryPath.c_str (), nullptr)) {
            DWORD error = ::GetLastError ();
            if (error != ERROR_ALREADY_EXISTS) {
                Execution::DoThrow (Execution::Platform::Windows::Exception (error));
            }
        }
#elif   qPlatform_POSIX
        if (createParentComponentsIfNeeded) {
            // walk path and break into parts, and from top down - try to create parent directory structure.
            // Ignore any failures - and just let the report of failure (if any must result) come from original basic
            // CreateDirectory call.
            vector<String> paths;
            size_t  index   =   directoryPath.find ('/');
            while (index != -1 and index + 1 < directoryPath.length ()) {
                if (index != 0) {
                    String parentPath = directoryPath.substr (0, index);
                    //IgnoreExceptionsForCall (CreateDirectory (parentPath, false));
                    paths.push_back (parentPath);
                }
                index = directoryPath.find ('/', index + 1);
            }

            // Now go in reverse order - checking if the exist - and if so - stop going back
            for (auto i = paths.rbegin (); i != paths.rend (); ++i) {
                //NB: this avoids matching files - we know dir - cuz name ends in /
                if (access(i->AsTString ().c_str (), R_OK) == 0) {
                    // ignore this one
                }
                else {
                    // THEN - starting at the one that doesn't exist - go from top-down again
                    int skipThisMany    =   (i - paths.rbegin ());
                    Assert (skipThisMany < paths . size ())
                    for (auto ii = paths.begin () + skipThisMany; ii != paths.end (); ++ii) {
                        CreateDirectory (*ii, false);
                    }
                    break;
                }
            }
        }
        // Horrible - needs CLEANUP!!! -- LGP 2011-09-26
        if (::mkdir (directoryPath.AsTString ().c_str (), 0755) != 0) {
            if (errno != 0 and errno != EEXIST) {
                Execution::DoThrow (errno_ErrorException (errno));
            }
        }
#else
        AssertNotImplemented ();
#endif
    }
    CATCH_REBIND_FILENAMES_HELPER_(directoryPath);
}







/*
 ********************************************************************************
 ******************* FileSystem::CreateDirectoryForFile *************************
 ********************************************************************************
 */
void    FileSystem::CreateDirectoryForFile (const String& filePath)
{
    if (filePath.empty ()) {
        // NOT sure this is the best exception to throw here?
        Execution::DoThrow (IO::FileAccessException ());
    }
    if (FileExists (filePath)) {
        // were done
        return;
    }
    CreateDirectory (GetFileDirectory (filePath), true);
}






/*
 ********************************************************************************
 ************************** FileSystem::GetVolumeName ***************************
 ********************************************************************************
 */
String FileSystem::GetVolumeName (const String& driveLetterAbsPath)
{
#if     qPlatform_Windows
    // SEM_FAILCRITICALERRORS needed to avoid dialog in call to GetVolumeInformation
    AdjustSysErrorMode  errorModeAdjuster (AdjustSysErrorMode::GetErrorMode () | SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

    DWORD   ignored =   0;
    TChar   volNameBuf[1024];
    memset (volNameBuf, 0, sizeof (volNameBuf));
    TChar   igBuf[1024];
    memset (igBuf, 0, sizeof (igBuf));
    BOOL    result  =   ::GetVolumeInformation (
                            AssureDirectoryPathSlashTerminated (driveLetterAbsPath).AsTString ().c_str (),
                            volNameBuf,
                            NEltsOf (volNameBuf),
                            nullptr,
                            &ignored,
                            &ignored,
                            igBuf,
                            NEltsOf (igBuf)
                        );
    if (result) {
        return String::FromTString (volNameBuf);
    }
#else
    AssertNotImplemented ();
#endif
    return String ();
}







/*
 ********************************************************************************
 ***************************** FileSystem::FileExists ***************************
 ********************************************************************************
 */
bool    FileSystem::FileExists (const String& filePath)
{
#if     qPlatform_Windows
    RequireNotNull (filePath);
    DWORD attribs = ::GetFileAttributesW (filePath.c_str ());
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return not (attribs & FILE_ATTRIBUTE_DIRECTORY);
#elif   qPlatform_POSIX
    // Not REALLY right - but an OK hack for now... -- LGP 2011-09-26
    //http://linux.die.net/man/2/access
    return access (filePath.AsTString().c_str (), R_OK) == 0;
#else
    AssertNotImplemented ();
    return false;
#endif
}






#if 0
/*
 ********************************************************************************
 ************************** FileSystem::DirectoryExists *************************
 ********************************************************************************
 */
bool    FileSystem::DirectoryExists (const String& filePath)
{
#if     qPlatform_Windows
    RequireNotNull (filePath);
    DWORD attribs = ::GetFileAttributesW (filePath.c_str ());
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return !! (attribs & FILE_ATTRIBUTE_DIRECTORY);
#elif   qPlatform_POSIX
    struct  stat    s;
    if (::stat (filePath.AsTString ().c_str (), &s) < 0) {
        // If file doesn't exist - or other error reading, just say not exist
        return false;
    }
    return S_ISDIR (s.st_mode);
#else
    AssertNotImplemented ();
    return false;
#endif
}
#endif






/*
 ********************************************************************************
 ******************************* FileSystem::CopyFile ***************************
 ********************************************************************************
 */
void    FileSystem::CopyFile (const String& srcFile, const String& destPath)
{
#if     qPlatform_Windows
// see if can be/should be rewritten to use Win32 API of same name!!!
//
// If I DONT do that remapping to Win32 API, then redo this at least to copy / rename through tmpfile
    if (not FileExists (srcFile)) {
        Execution::DoThrow (FileAccessException (srcFile, IO::FileAccessMode::eRead_FAM));
    }
    CreateDirectoryForFile (destPath);
    ThrowIfFalseGetLastError (::CopyFile (destPath.AsTString ().c_str (), srcFile.AsTString ().c_str (), false));
#else
    AssertNotImplemented ();
#endif
}









/*
 ********************************************************************************
 ***************************** FileSystem::FindFiles ****************************
 ********************************************************************************
 */
vector<String> FileSystem::FindFiles (const String& path, const String& fileNameToMatch)
{
    vector<String> result;
    if (path.empty ()) {
        return result;
    }
#if     qPlatform_Windows
    String usePath         =   AssureDirectoryPathSlashTerminated (path);
    String matchFullPath   =   usePath + (fileNameToMatch.empty () ? L"*" : fileNameToMatch);
    WIN32_FIND_DATA fd;
    memset (&fd, 0, sizeof (fd));
    HANDLE hFind = ::FindFirstFile (matchFullPath.AsTString ().c_str (), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        try {
            do {
                String fileName = String::FromTString (fd.cFileName);
                if (not (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    result.push_back (usePath + fileName);
                }
            }
            while (::FindNextFile (hFind, &fd));
        }
        catch (...) {
            ::FindClose (hFind);
            Execution::DoReThrow ();
        }
        ::FindClose (hFind);
    }
#else
    AssertNotImplemented ();
#endif
    return result;
}








/*
 ********************************************************************************
 ************************* FileSystem::FindFilesOneDirUnder *********************
 ********************************************************************************
 */
vector<String> FileSystem::FindFilesOneDirUnder (const String& path, const String& fileNameToMatch)
{
    if (path.empty ()) {
        return vector<String> ();
    }

    set<String>    resultSet;
#if     qPlatform_Windows
    String usePath =   AssureDirectoryPathSlashTerminated (path);
    WIN32_FIND_DATA fd;
    memset (&fd, 0, sizeof (fd));
    HANDLE hFind = ::FindFirstFile ((usePath + L"*").AsTString ().c_str (), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            TString fileName = (LPCTSTR) &fd.cFileName;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                String fileName = String::FromTString ((LPCTSTR) &fd.cFileName);
                const   String kDOT    =   L".";
                const   String kDOTDOT =   L"..";
                if ((fileName != kDOT) and (fileName != kDOTDOT)) {
                    vector<String> subdirFiles =   FindFiles (usePath + fileName, fileNameToMatch);
                    resultSet = resultSet + set<String> (subdirFiles.begin (), subdirFiles.end ());
                }
            }
        }
        while (::FindNextFile (hFind, &fd));
        ::FindClose (hFind);
    }
#else
    AssertNotImplemented ();
#endif
    return vector<String> (resultSet.begin (), resultSet.end ());
}








/*
 ********************************************************************************
 ***************** FileSystem::DeleteAllFilesInDirectory ************************
 ********************************************************************************
 */
void    FileSystem::DeleteAllFilesInDirectory (const String& path, bool ignoreErrors)
{
#if     qPlatform_Windows
    if (path.empty ()) {
        Execution::DoThrow (Execution::Platform::Windows::Exception (ERROR_INVALID_NAME));
    }
    String dir2Use =   AssureDirectoryPathSlashTerminated (path);

    WIN32_FIND_DATA fd;
    (void)::memset (&fd, 0, sizeof (fd));
    HANDLE          hFind = ::FindFirstFile ((dir2Use + L"*").AsTString ().c_str (), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        try {
            do {
                String fileName = String::FromTString (fd.cFileName);
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if ((fileName != L".") and (fileName != L"..")) {
                        DeleteAllFilesInDirectory (dir2Use + fileName + L"\\", ignoreErrors);
                        try {
                            ThrowIfFalseGetLastError (::RemoveDirectory ((dir2Use + fileName).AsTString ().c_str ()));
                        }
                        catch (...) {
                            DbgTrace ("Exception %s calling RemoveDirectory on file '%s'", ignoreErrors ? L"(ignored)" : L"", (dir2Use + fileName).c_str ());
                            if (!ignoreErrors) {
                                Execution::DoReThrow ();
                            }
                        }
                    }
                }
                else {
                    try {
                        ThrowIfFalseGetLastError (::DeleteFile ((dir2Use + fileName).AsTString ().c_str ()));
                    }
                    catch (...) {
                        DbgTrace (TSTR ("Exception %s calling ::DeleteFile on file '%s'"), ignoreErrors ? TSTR ("(ignored)") : TSTR (""), (dir2Use + fileName).c_str ());
                        if (!ignoreErrors) {
                            Execution::DoReThrow ();
                        }
                    }
                }
            }
            while (::FindNextFile (hFind, &fd));
        }
        catch (...) {
            ::FindClose (hFind);
            Execution::DoReThrow ();
        }
        ::FindClose (hFind);
    }
#else
    AssertNotImplemented ();
#endif
}





#if     qPlatform_Windows
/*
 ********************************************************************************
 ********************* FileSystem::DirectoryChangeWatcher ***********************
 ********************************************************************************
 */
FileSystem::DirectoryChangeWatcher::DirectoryChangeWatcher (const String& directoryName, bool watchSubTree, DWORD notifyFilter)
    : fDirectory (directoryName)
    , fWatchSubTree (watchSubTree)
    , fThread ()
    , fDoneEvent (::CreateEvent (nullptr, false, false, nullptr))
    , fWatchEvent (::FindFirstChangeNotification (fDirectory.AsTString ().c_str (), fWatchSubTree, notifyFilter))
    , fQuitting (false)
{
    fThread = Execution::Thread (bind (&ThreadProc, this));
    fThread.SetThreadName (L"DirectoryChangeWatcher");
    fThread.Start ();
}

FileSystem::DirectoryChangeWatcher::~DirectoryChangeWatcher ()
{
    fQuitting = true;
    if (fDoneEvent != INVALID_HANDLE_VALUE) {
        Verify (::SetEvent (fDoneEvent));
    }
    IgnoreExceptionsForCall (fThread.AbortAndWaitForDone ());
    if (fDoneEvent != INVALID_HANDLE_VALUE) {
        Verify (::CloseHandle (fDoneEvent));
    }
    if (fWatchEvent != INVALID_HANDLE_VALUE) {
        Verify (::FindCloseChangeNotification (fWatchEvent));
    }
}

void    FileSystem::DirectoryChangeWatcher::ValueChanged ()
{
}

void    FileSystem::DirectoryChangeWatcher::ThreadProc (void* lpParameter)
{
    DirectoryChangeWatcher*     _THS_   =   reinterpret_cast<DirectoryChangeWatcher*> (lpParameter);
    while (not _THS_->fQuitting and _THS_->fWatchEvent != INVALID_HANDLE_VALUE) {
        HANDLE  events[2];
        events[0] = _THS_->fDoneEvent;
        events[1] = _THS_->fWatchEvent;
        ::WaitForMultipleObjects (NEltsOf (events), events, false, INFINITE);
        Verify (::FindNextChangeNotification (_THS_->fWatchEvent));
        if (not _THS_->fQuitting) {
            _THS_->ValueChanged ();
        }
    }
}
#endif







#if         qPlatform_Windows
/*
 ********************************************************************************
 ****************************** FileSystem::AdjustSysErrorMode **************************
 ********************************************************************************
 */
UINT    AdjustSysErrorMode::GetErrorMode ()
{
    UINT    good    =   ::SetErrorMode (0);
    ::SetErrorMode (good);
    return good;
}

AdjustSysErrorMode::AdjustSysErrorMode (UINT newErrorMode):
    fSavedErrorMode (::SetErrorMode (newErrorMode))
{
}

AdjustSysErrorMode::~AdjustSysErrorMode ()
{
    (void)::SetErrorMode (fSavedErrorMode);
}
#endif








/*
 ********************************************************************************
 ************************ DirectoryContentsIterator *****************************
 ********************************************************************************
 */
DirectoryContentsIterator::DirectoryContentsIterator (const String& pathExpr)
#if         qPlatform_Windows
    : fHandle (INVALID_HANDLE_VALUE)
    , fFindFileData ()
    , fDirectory (pathExpr)
#endif
{
#if         qPlatform_Windows
    memset (&fFindFileData, 0, sizeof (fFindFileData));
    size_t i = fDirectory.rfind ('\\');
    if (i == wstring::npos) {
        Execution::DoThrow (StringException (L"Cannot find final '\\' in directory path"));
    }
    fDirectory = fDirectory.substr (0, i + 1);
    fHandle = ::FindFirstFile (pathExpr.AsTString ().c_str (), &fFindFileData);
#else
    AssertNotImplemented ();
#endif
}

DirectoryContentsIterator::~DirectoryContentsIterator ()
{
#if         qPlatform_Windows
    if (fHandle != INVALID_HANDLE_VALUE) {
        ::FindClose (fHandle);
    }
#else
    AssertNotImplemented ();
#endif
}

bool    DirectoryContentsIterator::NotAtEnd () const
{
#if         qPlatform_Windows
    return fHandle != INVALID_HANDLE_VALUE;
#else
    AssertNotImplemented ();
    return false;
#endif
}

String DirectoryContentsIterator::operator *() const
{
    Ensure (NotAtEnd ());
#if         qPlatform_Windows
    return fDirectory + String::FromTString (fFindFileData.cFileName);
#else
    AssertNotImplemented ();
    return String ();
#endif
}

void    DirectoryContentsIterator::operator++ ()
{
#if         qPlatform_Windows
    memset (&fFindFileData, 0, sizeof (fFindFileData));
    if (::FindNextFile (fHandle, &fFindFileData) == 0) {
        ::FindClose (fHandle);
        fHandle = INVALID_HANDLE_VALUE;
    }
#else
    AssertNotImplemented ();
#endif
}


