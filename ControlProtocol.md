# Table of Contents #


# Introduction #

You can control ClockTamer via USB or SPI. When connected with USB, ClockTamer emulates serial port, so you can control it equally with your preferred terminal emulator, from scripts and from programs. SPI connection is used when ClockTamer is connected to USRP and is useful if don't want to have two USB cables running from USRP to your computer.

ClockTamer uses text based protocol and it is basically the same for USB and SPI ClockTamer interfaces. USB and SPI versions of the protocol differ only in the way answers from ClockTamer are transfered. When using SPI connection you need to poll for answer, as described at [the SPI interface page](ProtocolSPI#Control_protocol_SPI-specific_operation.md). When using USB connection, you don't need to do anything specific to receive answer, but on Linux before doing any exchange you should turn off echo for the ClockTamer serial device. From command line you can turn it off with following command:
```
stty -F /dev/ttyACM0 -echo
```
where `/dev/ttyACM0` is your ClockTamer's serial device file.

In short, protocol has following features:
  1. It's strictly **master/slave**, with ClockTamer being slave. This means, that ClockTamer can only reply to commands sent by host and can't issue any commands or queries to host.
  1. All **commands and answers are ASCII strings, terminated by CR/LF**. This makes it easy to command ClockTamer in interactive mode with your favorite terminal emulator program.
  1. **Every command from host is followed by answer from ClockTamer**, i.e. protocol is synchronous. It's like in SD/MMC - if you've sent a command you should wait until answer is received before issuing next command. If you haven't received answer after some long time, something went wrong and it's a bug. It's recommended to reset ClockTamer and try again. And don't forget to report bugs [here](http://code.google.com/p/clock-tamer/issues/list)!

# For impatient... #

If you just want to start using your ClockTamer without getting into all fine details, here is the way to go:
```
SET,,OSC,<osc_freq>
SET,,OUT,<out_freq>
```
where you should replace:
  * `osc_freq` with the frequency (in Hz) of (VC)TCXO installed on your ClockTamer, e.g. something like 20000000. Note that you can calibrate your ClockTamer by slightly adjusting `osc_freq`. Read [ClockTamer calibration page](ClockTamerCalibration.md) for detailed instructions.
  * `out_freq` with desired output frequency (in Hz).

If you're satisfied with your settings, you can store them in EEPROM to be used after reboot:
```
STE
```

# Commands and answers structure #

Every command have format
```
CMD[,TYP[,DET[,value]]]<CR><LF>
```
where<br />
`CMD` - three letter command class. Must be non empty string.<br />
`TYP` - three letter command target type. May be empty string, even if `DET` or `value` is present (e.g. `SET,,OSC,10000200`).<br />
`DET` - three letter command target details. May be empty string, even if `value` is present (e.g. `PIN,LED,,0`).<br />
`value` - decimal or hexadecimal value. Decimal values start with a digit (e.g. `54781`), hexadecimal  values start with '`x`' letter (e.g. `x01af3d`).<br />

Depending on command class, later fields may be omitted completely, e.g.:
```
LDE       - only CMD is present
INF,,OSC  - CMD, TYP and DET are present with TYP as empty string
```


Answer format is as following:
  * If command does not require a value to be returned, then answer is
```
OK<CR><LF>
```
  * If command returns a value, answer has structure same as command:
```
CMD[,TYP[,DET[,value]]]<CR><LF>
```
> where `CMD`, `TYP` and `DET` repeats the respective fields in the command.
  * If command can't be parsed, error is returned:
```
SYNTAX ERROR<CR><LF>
```

# Command classes #

## Register ("REG") ##

Writes `value` data to appropriate registers of chips via [MicroWire bus](http://en.wikipedia.org/wiki/Microwire). Using this commands you can control every aspect of clock generation in manual mode.  Allowed target type and detail combinations:
| **`TYP`** | **`DET`** | **`value`** | **Description** |
|:----------|:----------|:------------|:----------------|
| `LMK`     | `<empty>` | 32bit value | Write to LMK (clock distributor) chip registers |
| `LMX`     | `<empty>` | 24bit value | Write to LMX (PLL+VCO) chip registers (if LMX is present) |
| `DAC`     | `<empty>` | 16-24bit value | Write to DAC (Digital-to-Analog Converter) chip registers (if DAC is present)|

## Pin ("PIN") ##

Controls pins of chips. Allowed target type and detail combinations:
| **`TYP`** | **`DET`** | **`value`** | **Description** |
|:----------|:----------|:------------|:----------------|
| `LMK`     | `ENB`     | 0/1         | `ENABLE` pin of LMK (clock distributor) chip. |
| `LMK`     | `GOE`     | 0/1         | `GOE` pin of LMK (clock distributor) chip. |
| `LMX`     | `SYN`     | 0/1         | `SYN` pin of LMX (PLL+VCO) chip. |
| `LED`     | `<empty>` | 0/1         | Turn LED on/off. |

## Set ("SET") ##

Easy to use setting of output frequency, setting oscillator calibration value, changing algorithms internal variables. The most useful target type and detail combinations:
| **`TYP`** | **`DET`** | **`value`** | **Description** |
|:----------|:----------|:------------|:----------------|
| `<empty>` | `<empty>` | `<empty>`   | Write all control registers from MCU RAM to LMX and LMK chips. |
| `<empty>` | `OSC`     | integer     | Set oscillator frequency. Used when oscillator is changed, when external frequency source changed its frequency or to calibrate oscillator. Control registers of LMK and LMX are recalculated and written to the chips with this command. |
| `<empty>` | `OUT`     | integer     | Set ClockTamer output frequency. Control registers of LMK and LMX are recalculated and written to the chips with this command. |
| `<empty>` | `AUT`     | 0/1         | If set to 1 ClockTamer will load all variables from EEPROM and start generator on boot. |
| `LMK`     | `<empty>` | `<empty>`   | Write all control registers of LMK to the chip. |
| `LMK`     | `PRT`     | integer     | Change value of LMK output mask. See description of `LMK_OutMask` EEPROM register for details. Control registers of LMK are written to the chip with this command. |
| `GPS`     | `SYN`     | `<empty>`   | One-time synchronization to GPS. |
| `GPS`     | `AUT`     | 0/1         | If set to 1 ClockTamer will keep clock synchronized to GPS. |

## Save ("SAV") ##

_NOT IMPLEMENTED YET_

## Defaults ("DEF") ##

_NOT IMPLEMENTED YET_

## Info ("INF") ##

Get value of the selected variable. Every variable (specified in `DET`), usable with `SET` has its `INF` counterpart. There is no `NFO` command with empty `DET`, though.

There are also a set of variables for debugging purposes only. You don't need to touch them if you don't change the firmware. Also their meaning may change from one firmware version to another without API version change. Currently available target type and detail combinations:
| **`TYP`** | **`DET`** | **`value`** | **Description** |
|:----------|:----------|:------------|:----------------|
| `GPS`     | `DIV`     | `<empty>`   | Debug output. Value of divider of LMK output, routed to MCU input. |
| `GPS`     | `KBT`     | `<empty>`   | Debug output.   |
| `GPS`     | `R00`     | `<empty>`   | Debug output.   |
| `GPS`     | `R01`     | `<empty>`   | Debug output. In ClockTamer-1.2 it gives you a number of received 1pps ticks. If this value is increasing, then GPS lock is acquired. |
| `GPS`     | `R02`     | `<empty>`   | Debug output.   |
| `GPS`     | `R03`     | `<empty>`   | Debug output. In ClockTamer-1.2 it gives you calculated output frequency, multiplied by 32, i.e. `Fout = R02/32`. |
| `GPS`     | `MAX`     | `<empty>`   | Debug output.   |
| `GPS`     | `MIN`     | `<empty>`   | Debug output.   |


## Version ("VER") ##

Get firmware and protocol versions.

Example:
```
ClockTamer SW=1.23 API=1
```

## Hardware information ("HWI") ##

Get detailed information about installed hardware options. This information is stored in EEPROM and may be altered only if ClockTamer is flashed over SPI. Flashing over USB can not change EEPROM data.

Values shown in HWI string incude:
| `LMX=` | Type of LMX2531 VCO+PLL. Can be "2080" for LMX2531LQ2080E or "1515" for LMX2531LQ1515E. Most ClockTamers have LMX2531LQ2080E. |
|:-------|:------------------------------------------------------------------------------------------------------------------------------|
| `LMK=` | Type of LMK01000 Clock Buffer, Divider, and Distributor. Can be "1000" for LMK01000, "1010" for LMK01010 or "1020" for LMK01020, but is usually LMK01010. |
| `OSC=`<br />`FOSC=` | Frequency of the reference TCXO or VCTCXO in MHz. Possible values are 10, 20 or 26.                                           |
| `GPS`  | ClockTamer has GPS module installed if `GPS` is present in `HWI`.                                                             |
| `VCTCXO` | ClockTamer use VCTCXO with DAC instead of TCXO with VCO+PLL is `VCTCXO` is present in `HWI`.                                  |


Example of `HWI` for a ClockTamer with LMX2531LQ2080E, LMK1010, 20MHz TCXO and GPS:
```
LMX=2080 LMK=1010 OSC=20 GPS
```

## Reset ("RST") ##

Sends reset command to LMK and LMX and clears variables in MCU RAM, but does not affect EEPROM. To start output generation after `RST` you can execute "`SET,,OSC,<val>`", "`SET,,OUT,<val>`" or "`LDE`"+"`SET`".

## Load EEPROM ("LDE") ##

Loads previously stored values from EEPROM to RAM. This command does not push them to LMK and LMX chips, so you should execute `SET` command manually.

`TYP`, `DET` and `value` should be empty.

## Store EEPROM ("STE") ##

Stores all values to EEPROM. This values will be used by default after ClockTamer reboot. They can also be loaded to RAM at any time with `LDE` command.

`TYP`, `DET` and `value` should be empty.

## Enter GPS mode ("%%%") ##

Run this command to switch from the control mode to the GPS positioning mode. In the GPS positioning mode ClockTamer acts as a usual NMEA device and does not accept any ClockTamer control commands expect "Leave GPS mode" command ("%").

Command doesn't return any status. You should start reading NMEA status right after this command.

## Leave GPS mode ("%") ##

Run this command to leave GPS positioning mode and switch back to the normal ClockTamer control mode. In the control mode this command does nothing, so it's safe to use it before you issue a command to make sure a ClockTamer is in the control mode.

Command doesn't return any status. You could issue the next command right after this command.

# EEPROM variables list #

| Variable | Description |
|:---------|:------------|
| `Fosc`   | Oscillator frequency |
| `Fout`   | Output frequency |
| `VCO_MIN` | Parameter for calculation of VCO registers for LMX. Shouldn't be changed unless you change LMX on your unit. |
| `VCO_MAX` | Parameter for calculation of VCO registers for LMX. Shouldn't be changed unless you change LMX on your unit. |
| `VCO_Kbit` | Parameter for calculation of VCO registers for LMX. Shouldn't be changed unless you change LMX on your unit. |
| `LMK_OutMask` | Enable/disable output generation on specific LMK outputs.  Set bit in the mask to 1 to enable corresponding output and to 0 to disable it. E.g. `x60` value will enable 5 and 6 outputs and disable all others. See [outputs numbering](#Outputs_numbering.md) below. |
| `AutoFreq` | Autostart generating output on boot. |
| `DacValue` | DAC value if VCTCXO is used. |
| `AutoGPSSync` | Continuously synchronize clock to GPS. |

# Outputs numbering #

## ClockTamer-1.0 ##

If you look at ClockTamer so that "ClockTamer v1.0" label reads normally, then outputs are numbered in the following way:
```
2        0
  .---.
  |LMK|  7
  .---.
         6
4   5    6*
```
where 0,2,4,5,6 - places for U.FL connectors, 6`*` - a place for SMA connector. Outputs number 1 and 3 are not present on PCB. In default configuration outputs number 0 and 2 are LVDS, while 4,5,6 are LVPECL/CMOS.

## ClockTamer-1.1 and ClockTamer-1.2 ##

Outputs numbering is as shown on the picture:
![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-pic.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-pic.png)

Outputs 1 and 6 and CMOS, while outputs 0, 3, 4, 5 and 7 are LVDS or half-LVDS. Refer to [Features and technical specification](FeaturesAndTechnicalSpecification#Output_types.md) page for more information about half-LVDS outputs.