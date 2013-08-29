/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

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
#endif
#if     qPlatform_POSIX
#include    <unistd.h>
#endif

#include    "../../Characters/Format.h"
#include    "../../Characters/StringUtils.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#endif
#include    "../../Containers/Common.h"
#include    "../../Containers/STL/SetUtils.h"
#include    "../../Debug/Trace.h"
#include    "../../IO/FileAccessException.h"
#include    "../../Memory/SmallStackBuffer.h"
#include    "FileUtils.h"
#include    "WellKnownLocations.h"

#include    "Directory.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;

#if     qPlatform_Windows
using   Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif






Directory::Directory (const String& fileFullPath)
    : fFileFullPath_ (fileFullPath)
{
}

void    Directory::AssureExists (bool createParentComponentsIfNeeded) const
{
    FileSystem::CreateDirectory (fFileFullPath_, createParentComponentsIfNeeded);
}

void    Directory::AssureDeleted (bool autoDeleteContentsAsNeeded) const
{
    if (autoDeleteContentsAsNeeded) {
        FileSystem::DeleteAllFilesInDirectory (fFileFullPath_);
    }
#if     qPlatform_Windows
    ThrowIfFalseGetLastError (::RemoveDirectoryW (fFileFullPath_.c_str ()));
#elif   qPlatform_POSIX
    Execution::ThrowErrNoIfNegative (::rmdir (fFileFullPath_.c_str ()));
#endif
}
