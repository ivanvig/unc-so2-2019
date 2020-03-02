use warnings;
use strict;
use CGI;
use JSON;

my $cgi = CGI -> new();

my $result = `lsmod`;

my @resultarray = split('\n', $result);
my @parsedresult;

foreach my $line (@resultarray){
	if ($line =~ /(\w+)\s+(\d+)\s+(.*)/){
		push @parsedresult, {
			"mod" => $1,
			"size" => $2,
			"used" => $3
		};
	}
}

print $cgi->header( -type => 'application/json' );
my $json = encode_json \@parsedresult;
print "$json\n";
