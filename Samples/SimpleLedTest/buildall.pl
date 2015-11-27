#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";
require "../../ScriptsLib/BuildUtils.pl";

# these environment flags appear to confuse nmake
delete $ENV{'MAKEFLAGS'};

my $activeConfig = $ENV{'CONFIGURATION'};
my $projectPlatformSubdir = GetProjectPlatformSubdir ($activeConfig);
my $useBld = NormalizeBuildArg ($ARGV[0]);
my $useProjectDir= "Projects/" . $projectPlatformSubdir;

print("   Building Samples/SimpleLedTest...\n");
if (index($projectPlatformSubdir, "VisualStudio") != -1) {
	if ($activeConfig eq "Debug-U-32" || $activeConfig eq "Release-U-32") {
		my $curConfig	=	`../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;
		my $extraArgs = GetMSBuildArgs();
		RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs SimpleLedTest.vcxproj /p:$curConfig /target:$useBld");
	}
}
