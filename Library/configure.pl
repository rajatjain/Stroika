#!/usr/bin/perl -w

# MUST FIX TO CHECK FOR --only-if-unconfigured - for now thats all the logic we'll support
# but should check flag...
#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;



use constant false => 0;
use constant true  => 1;



#
# For now KISS - just check if the file doesn't exist, and if so write a default value.
#
my $configFileName	=	"Sources/Stroika/Foundation/Configuration/StroikaConfig.h";


my $intermediateFiles	=	"IntermediateFiles/";
my $platform		=	"Platform_Linux";

my $forceRecreate = false;


sub mkDirWithLinks
{
	local $relPath = $_[0];
	local $makefileName = $_[1];

	mkdir "$intermediateFiles/$platform/$relPath";
	system ("ln -s ../../../Projects/Linux/$makefileName $intermediateFiles/$platform/$relPath/Makefile");
}
sub mkDirWithLinks2
{
	local $relPath = $_[0];
	local $makefileName = $_[1];

	mkdir "$intermediateFiles/$platform/$relPath";
	system ("ln -s ../../../../Projects/Linux/$makefileName $intermediateFiles/$platform/$relPath/Makefile");
}


sub MakeUnixDirs {

if ($forceRecreate) {
	system ("rm -rf $intermediateFiles");
}

unless (-e $intermediateFiles) {
	mkdir "$intermediateFiles";
	mkdir "$intermediateFiles/$platform";
	mkdir "$intermediateFiles/$platform/";
	system ("ln -s ../../Projects/Linux/Makefile-Foundation $intermediateFiles/$platform/Makefile");
	system ("cp Projects/Linux/Configuration-Default.mk $intermediateFiles/$platform/Configuration.mk");
	system ("cp Projects/Linux/SharedBuildRules-Default.mk $intermediateFiles/$platform/SharedBuildRules.mk");
	
	mkDirWithLinks("Characters", "Makefile-Foundation-Characters");
	mkDirWithLinks("Configuration", "Makefile-Foundation-Configuration");
	mkDirWithLinks("Containers", "Makefile-Foundation-Containers");
	mkDirWithLinks("Cryptography", "Makefile-Foundation-Cryptography");
	mkDirWithLinks("Database", "Makefile-Foundation-Database");
	mkDirWithLinks("DataExchangeFormat", "Makefile-Foundation-DataExchangeFormat");
	mkDirWithLinks2("DataExchangeFormat/JSON", "Makefile-Foundation-DataExchangeFormat-JSON");
	mkDirWithLinks("Debug", "Makefile-Foundation-Debug");
	mkDirWithLinks("Execution", "Makefile-Foundation-Execution");
	mkDirWithLinks("IO", "Makefile-Foundation-IO");
	mkDirWithLinks2("IO/Network", "Makefile-Foundation-IO-Network");
	mkDirWithLinks("Math", "Makefile-Foundation-Math");
	mkDirWithLinks("Memory", "Makefile-Foundation-Memory");
	mkDirWithLinks("Streams", "Makefile-Foundation-Streams");
	mkDirWithLinks("Time", "Makefile-Foundation-Time");
}
}





sub WriteDefault
{
	#open(OUT,">$configFileName");
	open(OUT,">:crlf", "$configFileName");
	print (OUT "/*\n");
	print (OUT " * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved\n");
	print (OUT " */\n");
	print (OUT "#ifndef	_Stroika_Foundation_Configuration_StroikaConfig_h_\n");
	print (OUT "#define	_Stroika_Foundation_Configuration_StroikaConfig_h_	1\n");
	print (OUT "\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "//WARNING: THIS FILE WAS AUTOGENERATED WITH configure.pl\n");
	print (OUT "\n");
	print (OUT "#include	\"Private/Defaults_Configuration_Common_.h\"\n");
	print (OUT "\n");
	
	print (OUT "//Out of alphabetic order because other defaults depend on qDebug\n");
	print (OUT "#include	\"Private/Defaults_Debug_Assertions_.h\"\n");
	print (OUT "\n");
	print (OUT "#include	\"Private/Defaults_Characters_StringUtils_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Characters_TChar_.h\"\n");
	print (OUT "#include	\"Private/Defaults_CompilerAndStdLib_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Containers_LRUCache_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Containers_TimedCache_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Database_ODBCClient_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Debug_Trace_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Memory_Common_.h\"\n");
	print (OUT "#include	\"Private/Defaults_Memory_BlockAllocated_.h\"\n");
	print (OUT "\n");
	print (OUT "#endif	/*_Stroika_Foundation_Configuration_StroikaConfig_h_*/\n");
	close(OUT);
}


foreach $var (@ARGV)
{
    if (lc ($var) eq "-force" or lc ($var) eq "--force") {
	$forceRecreate = true;
	print "Forcing recreate...\n";
    }
}




if ($forceRecreate) {
    system ("rm $configFileName");
}

unless (-e $configFileName) {
	print("Writing \"$configFileName\"...\n");
	WriteDefault ();
}

print ("OS: $^O\n");

if ("$^O" eq "linux") {
    MakeUnixDirs ();
}
