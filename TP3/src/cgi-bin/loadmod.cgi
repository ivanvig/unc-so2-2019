use strict;
use warnings;
use CGI;
use JSON;

my $cgi = CGI -> new();

my $updir = "/srv/http/upload";
my $fname = $cgi->param("data");

if ( !$fname ) {
	print "Error al subir archivo";
	exit(-1);
}

my $fhandle = $cgi->upload("data");

open ( UPLOADFILE, ">$updir/$fname" ) or die "$!"; binmode UPLOADFILE;

while ( <$fhandle> ) {
	print UPLOADFILE;
}
close $fhandle;
close UPLOADFILE;

my $modinfo = `modinfo $updir/$fname`;

if ($? != 0){
	print "Archivo invalido";
	exit(-1);
}

my $comm = `sudo insmod $updir/$fname`;

if ($? != 0){
	print "Error al carga modulo";
	exit(-1);
}else{
	print("Modulo cargado");
}
