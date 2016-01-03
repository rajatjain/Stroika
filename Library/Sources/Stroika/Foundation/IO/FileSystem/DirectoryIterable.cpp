/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Traversal/Iterable.h"
#include    "DirectoryIterator.h"

#include    "DirectoryIterable.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;
using   namespace   Stroika::Foundation::Traversal;





/*
 ********************************************************************************
 ******************** IO::FileSystem::DirectoryIterable *************************
 ********************************************************************************
 */
DirectoryIterable::DirectoryIterable (const String& directoryName)
    : Iterable<String> (MakeIterableFromIterator (DirectoryIterator (directoryName)))
{
}
