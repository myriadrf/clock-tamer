# Table of Contents #


# Environment setup #

Our main development platform is Ubuntu and to date it's the only tested build environment. We welcome any additions and corrections to this howto, especially build instructions for other platforms.

```
# Install tools required for building firmware.
sudo aptitude install gcc-avr avr-libc unzip
```

Instructions for installing software for flashing are included into respective sections about flashing, because they depend on the tools you use.

# Source code preparation #

To check out source code and prepare it for building, run following commands in a directory of your choice:
```
# Check out clock-tamer project
hg clone https://clock-tamer.googlecode.com/hg/ clock-tamer
cd clock-tamer/board

# Download LUFA 091122, unpack to clock-tamer/LUFA directory and patch it
wget http://www.fourwalledcubicle.com/files/MyUSB/LUFA%20091122.zip
unzip "LUFA 091122.zip"
ln -s "LUFA 091122" LUFA-CUR
patch -p0 <patches/lufa.diff
```

# User code image (for USB flashing) #

This image only has usercode without USB bootloader. It can be flash with usual mini-USB cable without any expensive ISP programmer needed.

## Building ##

Depending on the ClockTamer modification you have to define one of the following defines:
```
Define                            HWI
------                            ---
TAMER_1515_20                     LMX=1515 LMK=1010 OSC=20
TAMER_2080_20                     LMX=2080 LMK=1010 OSC=20
TAMER_2080_10                     LMX=2080 LMK=1010 OSC=10
TAMER_2080_VCTXCO_26_GPS          LMX=2080 LMK=1010 OSC=26 VCTXCO GPS
TAMER_1650_26_GPS                 LMX=1650 LMK=1010 OSC=26 GPS
```

You can lookup your hardware info by executing [HWI command](ControlProtocol.md).

To build flash image for ClockTamer define the variable and run `make` in `clock-tamer/board` directory. For the "LMX=2080 LMK=1010 OSC=20" device it is
```
CFLAGS="-DTAMER_2080_20" make
```

If everything went fine, you should get a lot of output, ending with this:

```
Size after:
AVR Memory Usage
----------------
Device: at90usb162

Program:    6808 bytes (41.6% Full)
(.text + .data + .bootloader)

Data:        152 bytes (29.7% Full)
(.data + .bss + .noinit)

EEPROM:      411 bytes (80.3% Full)
(.eeprom)



-------- end --------
```

## Flashing ##

After a successful build in the `clock-tamer` directory you will find generated `Tamer-sdk-v1.hex` and `Tamer-sdk-v1.eep` files, which is ready to be flashed into your ClockTamer. You can also use `Tamer-sdk-v1.elf` if your programmer supports this.

Actual flashing instructions depend on a type of flasher application you have. We use `dfu-programmer`, which is readily available in Ubuntu. If you use **Ubuntu 9.10 or later**, just install it from repository:
```
sudo aptitude install dfu-programmer
```
And if you use **Ubuntu 9.04 or your `dfu-programmer` version is <0.5** we advice you to install it from [sources](https://sourceforge.net/projects/dfu-programmer/files/).

Now, when you have `dfu-programmer` ready, follow this steps:

  1. Power off ClockTamer - disconnect power cord AND mini-USB cable.
  1. Short-circuit `nSS` pin of `GND` (full connector pinout is available [here](ProtocolSPI#ClockTamer_pinout.md)).<br />![http://wiki.clock-tamer.googlecode.com/hg/images/misc/clocktamer-nSS.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/clocktamer-nSS.png)
  1. Connect ClockTamer to a computer with mini-USB cable. LED should light up immediately (WITHOUT flashing once first).
  1. Remove short-circuit from `nSS` to `GND`.
  1. In "`board`" directory run
```
make dfu
```
  1. If you want to flash EEPROM data too, run
```
make dfu-ee
```

# Full image (for ISP flashing) #

This image contains full code, including USB bootloader, and is suitable for ISP flashing.

**Note**, that to be able to flash ClockTamer over USB after you flash full image, you need to generate your own SDK header. Differences in compiler versions will likely cause different placement of bootloader functions and make standard SDK header useless.

## Building ##

The building procedure is similar to those of the standard image, but you should set environment variable "SDK=0", e.g.
```
SDK=0 CFLAGS="-DTAMER_2080_20" make
```

If everything went fine, you'll get something like this:

```
Size after:
AVR Memory Usage
----------------
Device: at90usb162

Program:   10618 bytes (64.8% Full)
(.text + .data + .bootloader)

Data:        152 bytes (29.7% Full)
(.data + .bss + .noinit)

EEPROM:      437 bytes (85.4% Full)
(.eeprom)



-------- end --------
```

## Flashing ##

After a successful build in the `clock-tamer` directory you will find generated `Tamer.hex` and `Tamer.eep` files, which is ready to be flashed into your ClockTamer. You can also use `Tamer.elf` if your programmer supports this.

Actual flashing instructions depend on a type of flasher and application you have. To flash full image we usually use [AVR Dragon](http://www.atmel.com/dyn/Products/tools_card.asp?tool_id=3891). If you have AVR Dragon too, follow this steps:
  1. Download "avrdude" from svn and build it. Prebuilt avrdude 5.2 crashes when you try to use it with AVR Dragon, because of [this bug](http://savannah.nongnu.org/bugs/?27507). This bug is fixed in [rev887](http://svn.savannah.gnu.org/viewvc?view=rev&root=avrdude&revision=887) and [rev895](http://svn.savannah.gnu.org/viewvc?view=rev&root=avrdude&revision=895) in avrdude svn, so make sure you use one of newer revisions.
  1. Connect ClockTamer to AVR Dragon, according to SPI pinout.
  1. In "`board`" directory run
```
SDK=0 make program
```
  1. If you connected everything correctly, you should have new image uploaded to ClockTamer now.

Note, that this operation erases EEPROM and thus you loose your calibration. If may want to write down your calibrated values somewhere or disable EEPROM write by commenting out this line in `makefile`:
```
AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep
```