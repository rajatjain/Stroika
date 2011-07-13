/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PCXLate.cc,v 1.12 1992/12/07 03:23:53 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PCXLate.cc,v $
 *		Revision 1.12  1992/12/07  03:23:53  lewis
 *		Renamed Catch/Try to lower case.
 *
 *		Revision 1.11  1992/12/05  17:45:12  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug, and
 *		qGCC_StaticInitializerForArrayOfObjectsWithCTORBug.
 *
 *		Revision 1.10  1992/10/30  01:47:14  lewis
 *		Use new Options stuff.
 *		Use spin cursor for mac.
 *		Move stuff from Headers/DirectoryTreeTranslater.hh here to this file.
 *		On failures to translate, just do a copy.
 *		Minor output cleanups.
 *
 *		Revision 1.9  1992/10/21  04:58:50  lewis
 *		Update for new GenClass support. Got to build under AUX.
 *
 *		Revision 1.8  1992/09/27  18:49:01  lewis
 *		Got rid of debug messages.
 *
 *		Revision 1.7  1992/09/26  22:24:08  lewis
 *		Ported to A/UX and gcc 2.2.1.
 *
 *		Revision 1.5  1992/07/14  20:13:04  lewis
 *		CHanges for new filesystem stuff - used this file to debug new filesystem
 *		stuff. Mostly added more printfs.
 *
 *		Revision 1.4  1992/07/07  06:51:01  lewis
 *		Added verbose flag, and added debug messages and attempts to work around
 *		make only bug with op+ for Pathnames by using GetFullPathName () - still
 *		needs more work, though...
 *
 *		Revision 1.3  1992/07/06  20:53:33  lewis
 *		Add verbose flag, and a few other minor cleanups - works on macos!!!.
 *
 *		Revision 1.2  1992/07/03  04:38:08  lewis
 *		Renamed Strings.hh -> String.hh.
 *
 *		Revision 1.10  92/03/30  17:26:25  17:26:25  lewis (Lewis Pringle)
 *		Wrap Translates in try clauses to catch errors - errors come from reading
 *		files with nulls, like those generated by mac for resoruce forks thru nfs.
 *		
 *
 *
 */
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/PCXLate.cc,v 1.12 1992/12/07 03:23:53 lewis Exp $";

/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#include    "OSInit.hh"
#pragma push
    #pragma force_active    on
    #if     qGCC_SimpleClassStaticInitializerBug
        static  OSInit  sOSInit;
    #else
        static  OSInit  sOSInit =   OSInit ();
    #endif
#pragma pop

#include	"Stroika-Foundation-Globals.hh"

#include	<stdlib.h>
#include	<fstream.h>

#include	"OSRenamePre.hh"
	#if		qMacOS
		#include	<CursorCtl.h>
	#endif
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"File.hh"
#include	"Options.hh"
#include	"StreamUtils.hh" 
#include	"String.hh"

#include	"CPlusLexer.hh" 
#include	"GlobalSymbolTranslater.hh"
#include	"IncludeTranslater.hh" 
#include	"StringDictionary.hh"
#include	"Translater.hh"



enum {
	eVerbose,
	eFromDirectory,
	eToDirectory,
	eReadDictionary,
	eReadIncludesDictionary,
	eReadGlobalsDictionary,
};
#if		qGCC_StaticInitializerForArrayOfObjectsWithCTORBug
	#if		qRealTemplatesAvailable
	static	Bag<OptionDefinition>	BogusFunction()
	#else
	static	Bag(OptionDefinition)	BogusFunction()
	#endif
{
#endif
#if		!qGCC_StaticInitializerForArrayOfObjectsWithCTORBug
const
#endif
OptionDefinition	kOD[] = {
	OptionDefinition ("Verbose", "v", eVerbose, False),
	OptionDefinition ("From", "from", eFromDirectory, False),
	OptionDefinition ("To", "to", eToDirectory, False),
	OptionDefinition ("Dictionary", "", eReadDictionary, False),
	OptionDefinition ("IncludesDictionary", "di", eReadIncludesDictionary, False),
	OptionDefinition ("GlobalsDictionary", "dg", eReadGlobalsDictionary, False),
};
#if		qGCC_StaticInitializerForArrayOfObjectsWithCTORBug
	#if		qRealTemplatesAvailable
	Bag<OptionDefinition>	tmp	=	Bag<OptionDefinition> (kOD, sizeof (kOD)/sizeof (kOD[0]));
	#else
	Bag(OptionDefinition)	tmp	=	Bag(OptionDefinition) (kOD, sizeof (kOD)/sizeof (kOD[0]));
	#endif
	return (tmp);
}
#endif

class	Options : public OptionParser {
	public:
		Options () :
			#if		qGCC_StaticInitializerForArrayOfObjectsWithCTORBug
					OptionParser (BogusFunction()),
			#else
				#if		qRealTemplatesAvailable
					OptionParser (Bag<OptionDefinition> (kOD, sizeof (kOD)/sizeof (kOD[0]))),
				#else
					OptionParser (Bag(OptionDefinition) (kOD, sizeof (kOD)/sizeof (kOD[0]))),
				#endif
			#endif
			fVerbose						(False),
			fMapIncludes 					(False),
			fMapGlobalSymbols 				(False),
			fReadIncludeDictionary 			(PathName::kBad),
			fReadGlobalSymbolsDictionary 	(PathName::kBad),
			fFromDirectory 					(PathName::kBad),
			fToDirectory 					(PathName::kBad)
			{
			}

		override	void	ParseOption_ (OptionID optionID)
		{
			switch (optionID) {
				case	eVerbose:				fVerbose = True; break;
				case	eFromDirectory:			fFromDirectory = SnagNextPathNameArg (); break;
				case	eToDirectory:			fToDirectory = SnagNextPathNameArg (); break;
				case	eReadDictionary:
					{
						String	kind	=	SnagNextArg ();
						if (toupper (kind) == toupper ("Includes")) {
							fMapIncludes = True;
							fReadIncludeDictionary = SnagNextPathNameArg ();
						}
						else if (toupper (kind) == toupper ("Globals")) {
							fMapGlobalSymbols = True;
							fReadGlobalSymbolsDictionary = SnagNextPathNameArg ();
						}
						else {
							default:	sBadArgs.Raise ("Unexpected argument to Dictionary");
						}
						fToDirectory = SnagNextPathNameArg ();
					}
					break;
				case	eReadIncludesDictionary:	fMapIncludes = True;		fReadIncludeDictionary = SnagNextPathNameArg (); break;
				case	eReadGlobalsDictionary:		fMapGlobalSymbols = True;	fReadGlobalSymbolsDictionary = SnagNextPathNameArg (); break;
				default:	RequireNotReached();
			}
		}

		override	void	AddIthParam_ (const String& param, unsigned i)
		{
			switch (i) {
				case	1:	fFromDirectory = FileSystem::Get ().ParseStringToPathName (param); break;
				case	2:	fToDirectory = FileSystem::Get ().ParseStringToPathName (param); break;
				default:	sBadArgs.Raise ("Unexpected third (non-option) arg");
			}
		}

		Boolean		fVerbose;
		Boolean		fMapIncludes;
		Boolean		fMapGlobalSymbols;

		PathName	fReadIncludeDictionary;
		PathName	fReadGlobalSymbolsDictionary;

		PathName	fFromDirectory;
		PathName	fToDirectory;
};

static	Boolean	gVerbose	=	False;			// cuz traversal done in proc which does not see this Options object...
												// This was the most painless hack to get it to know about verbose flag...






class DirectoryTreeTranslater {
	public:
		DirectoryTreeTranslater (Boolean include, Boolean symbols,
								 IncludeTranslater& theIncludeTranslater,
								 GlobalSymbolTranslater& theGlobalSymbolTranslater,
								 Mapping(String,String)& theIncludeDictionary,
								 PathName& temporary) :
			fMapIncludes (include),
			fMapGlobalSymbols (symbols),
			fIncludeTranslater (theIncludeTranslater),
			fGlobalSymbolTranslater (theGlobalSymbolTranslater),
			fIncludeDictionary (theIncludeDictionary),
			fTemporaryFile (temporary)
		{
		}

		nonvirtual void DoIt (const PathName& fromDirectory, const PathName& toDirectory);
		nonvirtual void DoPlainFileCopy (const PathName& from, const PathName& to);

	private:
		Boolean					fMapIncludes;
		Boolean 				fMapGlobalSymbols;

		IncludeTranslater		fIncludeTranslater;
		GlobalSymbolTranslater	fGlobalSymbolTranslater;

		Mapping(String,String)&	fIncludeDictionary;
		PathName				fTemporaryFile;
};



int		main (const int argc, const char* argv[])
{
#if		qMacOS
	::SpinCursor(1);
#endif

	Options	theOptions;

	try {
		theOptions.Parse (argc, argv);
	}
	catch1 (Options::sBadArgs) {
		cout << Options::sBadArgs.GetMessage () << endl;
		const	String	kUsage	=
			"PCXLate \n"
			"\t-v\n"
			"\t[-Dictionary <Includes | GlobalSymbols> <dictionaryFile>] {-di,-dg}\n"
			"\t[-From <inputDirectory>]\n"
			"\t[-To <outputDirectory>]";
		cout << "Usage: " << kUsage << endl;
		::exit (1);
	}

#if		qRealTemplatesAvailable
	Mapping<String,String>	includeDictionary;
	Mapping<String,String>	globalSymbolDictionary;
#else
	Mapping(String,String)	includeDictionary;
	Mapping(String,String)	globalSymbolDictionary;
#endif

#if		qMacOS
	::SpinCursor(1);
#endif

	if (theOptions.fMapIncludes) {
		includeDictionary = ReadMap (theOptions.fReadIncludeDictionary);
	}
#if		qMacOS
	::SpinCursor(1);
#endif

	if (theOptions.fMapGlobalSymbols) {
		globalSymbolDictionary = ReadMap (theOptions.fReadGlobalSymbolsDictionary);
	}

#if		qMacOS
	::SpinCursor(1);
#endif

	/*
	 * If we must do two translations, we will need an intermediate file.
	 */
	PathName	temporaryFile	=	PathName::kBad;


	if (theOptions.fMapIncludes and theOptions.fMapGlobalSymbols) {
		temporaryFile = FileSystem::Get ().GetTemporaryDirectory () + PathComponent ("pcxlate.tmp");
	}

	IncludeTranslater		doIncludes	=	IncludeTranslater (includeDictionary, kBadSequenceIndex);
	GlobalSymbolTranslater	doSymbols	=	GlobalSymbolTranslater (globalSymbolDictionary);

	if (theOptions.fVerbose) {
		cout << "Building DirectoryTreeTranslater" << endl;
	}
	gVerbose = theOptions.fVerbose;
	DirectoryTreeTranslater you (theOptions.fMapIncludes, theOptions.fMapGlobalSymbols,
								 doIncludes, doSymbols,
								 includeDictionary, temporaryFile);
	if (theOptions.fVerbose) {
		cout << "Doing translation from " << theOptions.fFromDirectory << " to " << theOptions.fToDirectory << endl;
	}
	you.DoIt (theOptions.fFromDirectory, theOptions.fToDirectory);

	if (theOptions.fVerbose) {
		cout << "Done" << endl;
	}

	return (0);
}

void	DirectoryTreeTranslater::DoIt (const PathName& from, const PathName& to)
{
	Assert (FileSystem::Get().Exists (from));
#if		qDontNestEnumUsedInTemplate
	Assert (FileSystem::Get().GetFileType (from) == eDirectoryFileType);
#else
	Assert (FileSystem::Get().GetFileType (from) == FileSystem::eDirectoryFileType);
#endif

	if (not FileSystem::Get().Exists (to)) {
		if (gVerbose) {
			cout << "Creating directory: " << to << endl;
		}
		FileSystem::Get().CreateDirectory(to);
	}

	ForEach (PathComponent, It, FileSystem::Get ().MakeDirectoryIterator (from)) {
#if		qMacOS
	::SpinCursor(1);
#endif

		PathName 		fromFile = from + It.Current ();
		String			tail	 = It.Current ().GetName ();

		String			newtail  = "";
		PathName		toFile   = PathName::kBad;

		if (fIncludeDictionary.Lookup (tail, &newtail)) {
			toFile = to + newtail;
		}
		else {
			toFile = to + tail;
		}
#if		qDontNestEnumUsedInTemplate
		if (FileSystem::Get().GetFileType (fromFile) == eDirectoryFileType) {
#else
		if (FileSystem::Get().GetFileType (fromFile) == FileSystem::eDirectoryFileType) {
#endif
			DoIt (fromFile, toFile);
		}
		else {
			if (fMapIncludes) {
				if (fMapGlobalSymbols) {
					Assert (fTemporaryFile != PathName::kBad);
					{
						try {
							if (gVerbose) {
								cout << "Using IncludeFile translator to map from " << fromFile << " to " << fTemporaryFile << endl;
							}
							fIncludeTranslater.TranslateFile (fromFile, fTemporaryFile);
						}
						catch () {
							if (gVerbose) {
								cout << "Error translating includes for file: " << fromFile << "-- copying." << endl;
							}
							DoPlainFileCopy (fromFile, fTemporaryFile);
						}
					}
					{
						try {
							if (gVerbose) {
								cout << tab << "and then Global Symbol to map from " << fTemporaryFile << " to " << toFile << endl;
							}
							fGlobalSymbolTranslater.TranslateFile (fTemporaryFile, toFile);
						}
						catch () {
							if (gVerbose) {
								cout << "Error translating globals for file: " << fromFile << "-- copying." << endl;
							}
							DoPlainFileCopy (fTemporaryFile, toFile);
						}
					}
				}
				else {
					try {
						if (gVerbose) {
							cout << "Using IncludeFile translator to map from " << fromFile << " to " << toFile << endl;
						}
						fIncludeTranslater.TranslateFile (fromFile, toFile);
					}
					catch () {
						if (gVerbose) {
							cout << "Error translating file: " << fromFile << "-- copying." << endl;
						}
						DoPlainFileCopy (fromFile, toFile);
					}
				}
			}
			else {
				Assert (fMapGlobalSymbols);

				try {
					if (gVerbose) {
						cout << "Using Global Symbol translator to map from " << fromFile << " to " << toFile << endl;
					}
					fGlobalSymbolTranslater.TranslateFile (fromFile, toFile);
				}
				catch () {
					if (gVerbose) {
						cout << "Error translating file: " << fromFile << "-- copying." << endl;
					}
					DoPlainFileCopy (fromFile, toFile);
				}
			}
		}
	}
}

void	DirectoryTreeTranslater::DoPlainFileCopy (const PathName& from, const PathName& to)
{
#if		qMacOS
	::SpinCursor(1);
#endif

	try {
		int	ifd = FileSystem::Get ().Open (from, O_RDONLY);
		ifstream	inFile = int (ifd);
		int	ofd = 0;
		ofstream	outFile;
		if (to != PathName::kBad) {
#if		qMacOS
			ofd = FileSystem::Get_MacOS ().Open (to, O_WRONLY | O_CREAT | O_TRUNC, 'MPS ', 'TEXT');
#else
			ofd = FileSystem::Get ().Open (to, O_WRONLY | O_CREAT | O_TRUNC);
#endif
#if		qGCC_LibgPlusPlusIOStreamAttatchMissingBug
			outFile.rdbuf ()->attach (ofd);
#else
			outFile.attach (ofd);
#endif
		}
		while (inFile) {
			outFile.put (inFile.get ());
		}

		outFile.flush ();
		::close (ifd);					// not sure needed???
		if (ofd != 0) {
			::close (ofd);				// not sure needed???
		}
	}
	catch () {
		cout << "Error plain-file copying " << from << " to " << to << endl;
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

