use strict;
use warnings;
use CGI;
use JSON;
use Data::Dumper;
use POSIX;

my $cgi = CGI->new();
print $cgi->header( -type => 'application/json' );


######### CPU #########

open (FH, "<", "/proc/cpuinfo");

my %cpuhash;
my $line;
my $sep = 0;

while ($line = <FH>) {
    if ($line =~ /((\w+ ?)+)\s*:\s+(.*)/){
        $cpuhash{$sep}{$1} = $3;
    }else{
        $sep++;
    }
}

close(FH);

######### RAM #########

open (FH, "<", "/proc/meminfo");

my %memhash;

while ($line = <FH>) {
    if ($line =~ /(.*):\s+(.*)/){
        $memhash{$1} = $2;
    }
}

close(FH);


######### UPTIME #########

open (FH, "<", "/proc/uptime");

my $uptime = <FH>;
chomp($uptime);

close(FH);


######### UPTIME #########

my $time = strftime "%Y-%m-%d %H:%M:%S", localtime time;

my %sysinfo = ('cpu' => \%cpuhash, 'mem' => \%memhash, 'uptime' => $uptime, 'time' => $time);

my $json = encode_json \%sysinfo;
print "$json\n";
