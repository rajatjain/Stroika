#!/usr/bin/perl 

my $BLD_TRG = $ARGV[0];
if ($BLD_TRG eq '') {
	$BLD_TRG = 'Build';
}

print ("Building library...\n");

if ("$^O" eq "linux") {
    my $useBld = lc ($BLD_TRG);
    if ($useBld eq "build") {
	$useBld = "all";
    }
    system ("cd ../IntermediateFiles/Platform_Linux/Lib; make $useBld");
}
else {
    system ("cd Projects/VisualStudio.Net-2010; perl buildall.pl $BLD_TRG");
}
