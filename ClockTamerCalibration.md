# Introduction #

Calibration is a procedure which helps you to maintain output frequency stability of your ClockTamer within specified bounds. It is performed by measuring actual frequency of the ClockTamer's internal TCXO and specifying it in the ClockTamer settings. E.g. internal TCXO specified as 20 MHz may actually have frequency 20'000'005 Hz. This 5 Hz offset at 20 MHz correspond to 5/20e6=0.25 ppm (parts per million) and will translate to (5Hz/20MHz)`*`900MHz = 225Hz at 900 MHz (low GSM band).

The GSM Standard specifies 50ppb = 0.05ppm frequency accuracy for a macro-BTS reference clock and 100ppb = 0.1ppm for femtocells. In our experience, offsets up to 500 Hz at 900MHz are ok for most 90% of phones to work without any problems, but we still recommend you to calibrate ClockTamer to 100ppb to ensure everything goes smooth.

Note, that ClockTamer you need to recalibrate ClockTamer after significant temperature changes and every few months. We also recommend yo calibrate it before the first use.

# Calibration with Kalibrate #

_Disclaimer: Fairwaves is not an author of Kalibrate and cannot help you if you have troubles with it._

Use [Kalibrate](http://thre.at/kalibrate/) or [Kalibrate-UHD](http://ttsou.github.com/kalibrate-uhd/) to measure frequency offset (read Kalibrate documentation for command line options syntax):

```
[root@openBTS kal-0.2]# ./kal -f 946600000 -u
USRP side: B
FPGA clock: 52000000
Decimation: 192
Antenna: RX2
Sample rate: 270833.343750
average [min, max] (range, stddev)
501.351685 [461, 546] (85, 19.314581)
overruns: 0
not found: 0
```

Nominal frequency in MHz of the oscillator installed in a ClockTamer is a value of `FOSC` in `HWI` output. You could check it with the [GUI](GUI.md):
![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-hwi-ver.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-hwi-ver.png)

or with [`HWI`](ControlProtocol#Hardware_information_(%22HWI%22).md) command manually:
```
LMX=2080 LMK=1010 OSC=20
```

In this example oscillator frequency is 20MHz, so you should introduce calibration offset of about -500Hz/946MHz\*20MHz=-10Hz. 19999990Hz looks like a good approximation of real oscillator frequency here. Set it with the [GUI](GUI.md) (Fosc input field):
![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-fosc-fout.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-fosc-fout.png)

or manually with the [`SET,,OSC`](ControlProtocol#Set_(%22SET%22).md) command:
```
SET,,OSC,19999990
```

Check frequency offset with Kalibrate again. Resulting average offset should be no more then 20-30Hz now.

Do not forget to store calibrated value to EEPROM when you're done. This can be done with "Set" button in the [GUI](GUI.md):
![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-buttons.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-buttons.png)

Or with [`STE`](ControlProtocol#Store_EEPROM_(%22STE%22).md) command manually:
```
STE
```

# See also #

  1. [FAQ: "What is a difference between "SET,,OSC" and "SET,,OUT" commands?"](FAQ#What_is_a_difference_between_&quot;_SET,,OSC_&quot;_and_&quot;_S.md)
  1. [ClockTamer GUI](GUI.md)
  1. [ClockTamer control protocol](ControlProtocol.md)