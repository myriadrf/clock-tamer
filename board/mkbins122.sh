#!/bin/sh

SDK=0
TFILE=Tamer
ODIR=sdk-v1_22/

MODSTD=" PINS_ONE "
MODGPS=" PINS_ONE "

mkdir -p $ODIR

avr-gcc -v 2> ${ODIR}info.txt
hg id -nibt >> ${ODIR}info.txt

for I in $MODGPS; do
    echo Building with GPS support for TAMER_$I ...

    GPS=1 SDK=$SDK TAMER_VER=122 make clean
    GPS=1 SDK=$SDK TAMER_VER=122 make
    mv $TFILE.hex ${ODIR}Tamer-GPS-${I}.hex
    mv $TFILE.eep ${ODIR}Tamer-GPS-${I}.eep
done
