#!/bin/sh

AWK=gawk
SYMBOLS="RunBootloader USB_CurrentlySelfPowered USB_RemoteWakeupEnabled USB_ConfigurationNumber USB_ControlEndpointSize USB_IsInitialized USB_DeviceState USB_ControlRequest eeHWInfo"
#SYMBOLS="$SYMBOLS  LanguageString "


FUNCS="__vector_11 USB_Device_ProcessControlRequest USB_Init USB_ResetInterface USB_ShutDown USB_INT_ClearAllInterrupts USB_INT_DisableAllInterrupts USB_USBTask "
FUNCS="$FUNCS   Endpoint_ConfigureEndpoint_Prv Endpoint_ClearStatusStage Endpoint_ClearEndpoints  Endpoint_BytesToEPSizeMaskDynamic Endpoint_Write_Control_PStream_LE "
FUNCS="$FUNCS   LanguageString"

IN=Tamer.map

OUT=Out.txt
XFILE=avr3sharedboot.x
OFILE=avr3-sdk-new.x

echo "" > $OUT


LINES=`wc -l $XFILE | $AWK -F" " '{print $1}' `


SDATASTART=`cat $IN | grep __data_start | $AWK -F" " '{ print $1}' | head -n1`
SDATAEND=`cat $IN | grep __noshared_start | $AWK -F" " '{ print $1}'`

SDW1=`echo $SDATASTART | $AWK -F"x" '{print toupper($2)}'`
SDW2=`echo $SDATAEND | $AWK -F"x" '{print toupper($2)}'`

DELTA=`echo "obase=16 ; ibase=16 ; $SDW2 - $SDW1" | bc`

echo "Shared data located from $SDW1 to $SDW2  ( $DELTA bytes)" 

for I in  $SYMBOLS ; do
	cat $IN | grep "0x0" | grep $I | $AWK -F" " '{ print " "$2" = "$1"  ;"  }' >> $OUT
done

echo "" >> $OUT

for I in  $FUNCS ; do
	cat $IN | grep "0x0" | grep $I | $AWK -F" " '{ print " "$2" = "$1"  ;"  }'  >> $OUT
done

cat $OUT


head -n 3 $XFILE                         > $OFILE
cat $OUT                                >> $OFILE
echo "__shared_data_skip = 0x0$DELTA ;" >> $OFILE
tail -n `expr $LINES - 5` $XFILE        >> $OFILE


