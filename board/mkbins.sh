#!/bin/sh

SDK=0
TFILE=Tamer
ODIR=out-bin/

MODS=" 1515_20 2080_20 2080_10 2080_VCTXCO_26_GPS 1650_26_GPS "

mkdir -p $ODIR

avr-gcc -v 2> ${ODIR}info.txt
hg id -nibt >> ${ODIR}info.txt

for I in $MODS; do
    echo Building for TAMER_$I ...

    SDK=$SDK CFLAGS="-DTAMER_${I}" make clean
    SDK=$SDK CFLAGS="-DTAMER_${I}" make
    mv $TFILE.hex ${ODIR}Tamer-${I}.hex
    mv $TFILE.eep ${ODIR}Tamer-${I}.eep
done

