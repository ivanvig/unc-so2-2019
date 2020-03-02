use strict;
use warnings;
use CGI;
use JSON;

$SIG{__DIE__} = sub { print @_, "\n"; exit 255 };

my $cgi = CGI -> new();

my $updir = "/srv/http/upload";
my $fname = $cgi->param("file");

if ( !$fname ) {
	print "Error al subir archivo";
	exit(-1);
}

my $fhandler = $cgi->upload("file")->handle;

open ( UPLOADFILE, ">$updir/$fname" ) or die "$!"; 
binmode UPLOADFILE;

my $buffer;

while ( my $rbytes = $fhandler->read($buffer, 1024) ) {
	print UPLOADFILE $buffer;
}

close $fhandler;
close UPLOADFILE;

my $modinfo = `modinfo $updir/$fname`;

if ($? != 0){
	print "Archivo invalido";
	exit(-1);
}

my $comm = `/usr/bin/sudo /usr/bin/insmod $updir/$fname`;

if ($? != 0){
	print $comm;
	exit(-1);
}else{
	print("Modulo cargado");
}
