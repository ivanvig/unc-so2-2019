use warnings;
use strict;
use CGI;
use JSON;

my $cgi = CGI -> new();

my %param = map { $_ => scalar $cgi->param($_) } $cgi->param() ;

my $result = `aws s3 ls "s3://noaa-goes16/ABI-L2-CMIPF/$param{anio}/$param{dia}/$param{hora}/" --no-sign-request`;

my @resultarray = split('\n', $result);
my $i = 0;

foreach my $line (@resultarray){
	$resultarray[$i] = [split(' ', $line)];
	$i++;
}

print $cgi->header( -type => 'application/json' );
my $json = encode_json \@resultarray;
print "$json\n";
