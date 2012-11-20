#!/usr/bin/perl
#use strict;
#use warnings;

### GIVE UP ON PERL - CANNOT EASILY INSTALL XML READER - SO JUST USE 
### tesxt reading..

my $intermediateFiles	=	GetThisScriptDir() . "/../IntermediateFiles";
my $masterXMLConfigFile	=	"$intermediateFiles/Configuration.xml";

require (GetThisScriptDir () + "StringUtils.pl");

# Declare the subroutines
sub	GetThisScriptDir {
	use File::Basename;
	use Cwd 'abs_path';
	my $p = __FILE__;
	my $A = abs_path ($p);
	my $dirname = dirname($A);
	return $dirname;
}

sub	GetAllOfStringToEndTag($) {
	my $string = shift;
	my $eos = index($string, "<");
	if ($eos == -1) {
		$string;
	}
	else {
		return substr ($string, 0, $eos);
	}
}

my %configuration = ();


sub	ReadValue_ {
	my $line = shift;
	my $tag = shift;
	my $startOff = index($line, $tag);
	if ($startOff != -1) {
		my $tmp = substr ($line, $startOff + length($tag));
		return GetAllOfStringToEndTag ($tmp);
	}
	return undef;
}

sub	ReadConfigFile_ {

	my $myDirName	=	GetThisScriptDir ();

	my @useExtraCDefines;
	my @useExtraMakeDefines;

	open (FILE, "$masterXMLConfigFile") or die("Unable to open $masterXMLConfigFile");
	my @data = <FILE>;
	close(FILE);
	foreach $line (@data) {
		my $pps = ReadValue_($line, "<ProjectPlatformSubdir>");
		if (defined $pps) {
			$configuration {'ProjectPlatformSubdir'} = $pps;
		}
		my $pps = ReadValue_($line, "<Platform>");
		if (defined $pps) {
			$configuration {'Platform'} = $pps;
		}
		my $pps = ReadValue_($line, "<Target>");
		if (defined $pps) {
			$configuration {'Target'} = $pps;
		}
		my $pps = ReadValue_($line, "<CompilerDriver>");
		if (defined $pps) {
			$configuration {'CompilerDriver'} = $pps;
		}
		my $pps = ReadValue_($line, "<qHasLibrary_Xerces>");
		if (defined $pps) {
			$configuration {'qHasLibrary_Xerces'} = $pps;
		}
		my $pps = ReadValue_($line, "<qHasFeature_libcurl>");
		if (defined $pps) {
			$configuration {'qHasFeature_libcurl'} = $pps;
		}
		my $pps = ReadValue_($line, "<qHasFeature_WinHTTP>");
		if (defined $pps) {
			$configuration {'qHasFeature_WinHTTP'} = $pps;
		}
		my $pps = ReadValue_($line, "<IncludeDebugSymbtolsInExecutables>");
		if (defined $pps) {
			$configuration {'IncludeDebugSymbtolsInExecutables'} = $pps;
		}
		my $pps = ReadValue_($line, "<STATIC_LINK_GCCRUNTIME>");
		if (defined $pps) {
			$configuration {'STATIC_LINK_GCCRUNTIME'} = $pps;
		}
		my $pps = ReadValue_($line, "<STATIC_LINK_GCCRUNTIME>");
		if (defined $pps) {
			$configuration {'STATIC_LINK_GCCRUNTIME'} = $pps;
		}
		my $pps = ReadValue_($line, "<CDefine>");
		if (defined $pps) {
			$useExtraCDefines[@useExtraCDefines] = $pps;
		}
		my $pps = ReadValue_($line, "<MakeDefine>");
		if (defined $pps) {
			$useExtraMakeDefines[@useExtraMakeDefines] = $var;
		}
	}
	$configuration {'ExtraCDefines'} = $useExtraCDefines;
	$configuration {'ExtraMakeDefines'} = $useExtraMakeDefines;
}




### PUBLIC APIS
sub	GetProjectPlatformSubdir {
	if ($configuration{'ProjectPlatformSubdir'} eq "") {
		die ("GetProjectPlatformSubdir () EMPTY\n");
	}
	return $configuration{'ProjectPlatformSubdir'};
}

sub	GetConfigurationParameter {
	my $paramName = shift;
	return $configuration{$paramName};
}








###PRIVATE
sub Prepare_ () {
	if ($#configuration < 0) {
		ReadConfigFile_ ();
	}
	#comment in to see values
	#print "CONFIGURE-ProjectPlatformSubdir: ", GetProjectPlatformSubdir(), "\n";
	return 1;
}



Prepare_ ();
