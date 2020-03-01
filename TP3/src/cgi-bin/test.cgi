use strict;
use warnings;
use CGI;
use JSON;

my $cgi = CGI->new();
print $cgi->header( -type => 'application/json' );


my %rec_hash = ('a' => 1, 'b' => 2, 'c' => 3, 'd' => 4, 'e' => 5);
my $json = encode_json \%rec_hash;
print "$json\n";
