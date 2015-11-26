#!/usr/bin/perl

require "../ScriptsLib/ConfigurationReader.pl";
require ("ScriptsLib/TestsList.pl");

my $activeConfig = $ENV{'CONFIGURATION'};

print ("Checking Tests...\n");
my $checkallDir = "Projects/" . GetProjectPlatformSubdir ($activeConfig);

use Cwd;
use Cwd 'abs_path';
my $savedDir = abs_path (getcwd ());
chdir ($checkallDir);
	foreach $tst (GetAllTests ()) {
		my $tstName = GetTestName ($tst);
		print ("   Test $tst: $tstName:  ");
		system ("CONFIGURATION=$activeConfig cd $tst; perl checkall.pl");
	}
chdir ($savedDir);
