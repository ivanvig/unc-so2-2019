BINPATH=bin/client
UPDBIN=firmware_update

rm -f $BINPATH
mv firmware_update $BINPATH
exec $BINPATH
