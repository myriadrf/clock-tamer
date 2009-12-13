#!/bin/sh

echo "RST" >/dev/ttyACM0 
sleep 1
echo "SET,VCO,MIN,1590" > /dev/ttyACM0 
sleep 1
echo "SET,VCO,MAX,1700" > /dev/ttyACM0 
sleep 1
echo "SET,VCO,KBT,2100" > /dev/ttyACM0 
sleep 1
echo "SET,,OSC,26000000" > /dev/ttyACM0 
sleep 1
echo "SET,,OUT,52000000" > /dev/ttyACM0 
sleep 1
echo "SET,LMK,PRT,64" > /dev/ttyACM0 
sleep 1
echo "SET,,AUT,1" > /dev/ttyACM0 
sleep 1
echo "STE" >/dev/ttyACM0 

