#!/bin/sh

TAMER_DEV="/dev/ttyACM0"
TAMER_VCO="2080"

tamer_echo()
{
    echo $* >>$TAMER_DEV
    sleep 0.2
}

tamer_read()
{
    head -n 1 $TAMER_DEV
}

tamer_exec()
{
    echo "<- $1"
    tamer_echo $1
    tamer_tmp=$(tamer_read)
    echo "-> $tamer_tmp"
    if test -n "$2" -a "$tamer_tmp" != "$2" ; then
        echo "*** ERROR ***"
        echo "Reply is not equal to \"$2\""
        exit 1
    fi
}

set_vco()
{
case "$1" in
1515)
    tamer_exec "SET,VCO,MIN,1450" "OK"
    tamer_exec "SET,VCO,MAX,1580" "OK"
    tamer_exec "SET,VCO,KBT,2000" "OK"
    ;;
2080)
    tamer_exec "SET,VCO,MIN,1904" "OK"
    tamer_exec "SET,VCO,MAX,2274" "OK"
    tamer_exec "SET,VCO,KBT,4500" "OK"
    ;;
*)
    tamer_exec "Unknown VCO: \"$1\". Aborting."
    exit 1
    ;;
esac
}

tamer_exec "RST" "OK"
set_vco $TAMER_VCO
tamer_exec "SET,,OSC,20000000"
tamer_exec "SET,,OUT,52000000" "OK"
tamer_exec "SET,LMK,PRT,64" "OK"
tamer_exec "SET,,AUT,1" "OK"
tamer_exec "STE" "OK"
