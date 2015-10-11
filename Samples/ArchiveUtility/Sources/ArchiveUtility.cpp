/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#if     qHasFeature_LZMA
#include    "Stroika/Foundation/DataExchange/7z/Reader.h"
#endif
#include    "Stroika/Foundation/IO/Filesystem/FileInputStream.h"
#include    "Stroika/Foundation/Streams/MemoryStream.h"
#include    "Stroika/Foundation/Streams/TextReader.h"

using   namespace std;

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Streams;


using   Characters::String;
using   Containers::Sequence;
using   Memory::Byte;
using   Memory::Optional;




namespace {
    struct  Options_ {
        enum class Operation { eList, eExtract, eCreate, eUpdate };
        Operation                   fOperation;
        String                      fArchiveFileName;
        Optional<String>            fOutputDirectory;   // applies only if extract
        Optional<Sequence<String>>  fFiles2Add;
    };
    void    Usage_ ()
    {
        cerr << "Usage: ArchiveUtility (--help | -h) | ((--list | --create | --extract |--update) ARCHIVENAME [--outputDirectory D] [FILES])" << endl;
        cerr << "    --help prints this help" << endl;
        cerr << "    -h prints this help" << endl;
        cerr << "    --list prints all the files in the argument archive" << endl;
        cerr << "    --create creates the argument ARHCIVE and adds the argument FILES to it" << endl;
        cerr << "    --extract extracts all the files from the argument ARHCIVE and to the output directory specified by --ouptutDirectory (defaulting to .)" << endl;
        cerr << "    --update adds to the argument ARHCIVE and adds the argument FILES to it" << endl;
    }
    // Emits errors to stderr, and Usage, etc, if needed, and Optional<> IsMissing()
    Optional<Options_>  ParseOptions_ (int argc, const char* argv[])
    {
        Options_    result {};
        Sequence<String>  args    =   Execution::ParseCommandLine (argc, argv);
        Optional<Options_::Operation>   operation;
        Optional<String>    archiveName;
        for (auto argi = args.begin (); argi != args.end(); ++argi) {
            if (argi == args.begin ()) {
                continue;   // skip argv[0] - command name
            }
            DbgTrace (L"xx=%s", argi->c_str ());
            if (Execution::MatchesCommandLineArgument (*argi, L"h") or Execution::MatchesCommandLineArgument (*argi, L"help")) {
                Usage_ ();
                return Optional<Options_> {};
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"list")) {
                operation = Options_::Operation::eList;
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"create")) {
                operation = Options_::Operation::eCreate;
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"extract")) {
                operation = Options_::Operation::eExtract;
            }
            else if (Execution::MatchesCommandLineArgument (*argi, L"update")) {
                operation = Options_::Operation::eUpdate;
            }
            else if (archiveName.IsMissing ()) {
                archiveName = *argi;
            }
            // else more cases todo
        }
        if (not archiveName) {
            cerr << "Missing name of archive" << endl;
            Usage_ ();
            return Optional<Options_> {};
        }
        if (not operation) {
            cerr << "Missing operation" << endl;
            Usage_ ();
            return Optional<Options_> {};
        }
        result.fOperation = *operation;
        result.fArchiveFileName = *archiveName;
        // @todo add more.. - files etc
        return result;
    }
}

namespace {
    DataExchange::ArchiveReader OpenArchive_ (const String& archiveName)
    {
        // @todo - must support other formats, have a registry, and autodetect
#if     qHasFeature_LZMA
        return move (_7z::ArchiveReader { IO::FileSystem::FileInputStream { archiveName } });
#endif
        throw "no fmt";
    }

}

namespace {
    void    ListArchive_ (const String& archiveName)
    {
        for (String i : OpenArchive_ (archiveName).GetContainedFiles ()) {
            cout << i.AsNarrowSDKString () << endl;
        }
    }
}




int     main (int argc, const char* argv[])
{
    if (Optional<Options_> o = ParseOptions_ (argc, argv)) {
        try {
            switch (o->fOperation) {
                case Options_::Operation::eList:
                    ListArchive_ (o->fArchiveFileName);
                    break;
            }
            // rest NYI
        }
        catch (...) {
            cerr << "Exception - terminating..." << endl;
            return EXIT_FAILURE;
        }
    }
    else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
