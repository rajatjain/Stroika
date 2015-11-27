#!/usr/bin/perl 

require "../../ScriptsLib/ConfigurationReader.pl";
require "../../ScriptsLib/BuildUtils.pl";

my $activeConfig = $ENV{'CONFIGURATION'};
my $projectPlatformSubdir = GetProjectPlatformSubdir ($activeConfig);
my $useBld = NormalizeBuildArg ($ARGV[0]);
my $useProjectDir= "Projects/" . $projectPlatformSubdir;



print("   Building Samples/SimpleService...\n");
if (index($projectPlatformSubdir, "VisualStudio") == -1) {
	use Cwd;
	use Cwd 'abs_path';
	my $savedDir = abs_path (getcwd ());

	chdir ("../../IntermediateFiles/$activeConfig/");
	if ($useBld eq "rebuild") {
		print ("Samples_SimpleService; clobber...\n");
		system ("cd Samples_SimpleService; make -s clobber");
		$useBld = "all";
	}
	
	system ("cd Samples_SimpleService; make -s $useBld");
	chdir ($savedDir);
}
else {
	my $curConfig	=	`../../ScriptsLib/GetVisualStudioConfigLine.pl $activeConfig`;
	my $extraArgs = GetMSBuildArgs();
	RunAndPrint ("cd $useProjectDir; msbuild.exe $extraArgs SimpleService.vcxproj /p:$curConfig /target:$useBld");
}
