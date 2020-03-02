use strict;
use warnings;
use CGI;
use JSON;

my $cgi = CGI -> new();

my $modname = $cgi->param("modname");
my $comm = `/usr/bin/sudo /usr/bin/rmmod $modname`;

if ($? != 0){
	print $comm;
	exit(-1);
}else{
	print("Modulo removido");
}
