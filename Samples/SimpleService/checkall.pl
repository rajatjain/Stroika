#!/usr/bin/perl 

require "../Scripts/checkallHelpers.pl";

my $activeConfig = $ENV{'CONFIGURATION'};

if ("$^O" eq "linux") {
	# nothing to check - not supported on Linux
}
else {
	if ($activeConfig eq "Debug-U-32" || $activeConfig eq "Release-U-32" || $activeConfig eq "Debug-U-64" || $activeConfig eq "Release-U-64") {
		CheckFileExists ("../../Builds/$activeConfig/Samples-SimpleService/SimpleService.exe");
	}
	else {
		print "[SKIPPED]\r\n";
		return;
	}
}
print "[SUCCEEDED]\r\n";
