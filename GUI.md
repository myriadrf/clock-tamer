<font size='7'>ClockTamer control GUI manual</font>

# Table of Contents #


![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui.png)

# Introduction #

ClockTamer GUI is a very simple application to control ClockTamer without typing all those boring commands in a terminal. It's written with Python and Qt4.

Current limitations:
  * Works only under Linux.
  * Assumes that your ClockTamer is connected with miniUSB and appears on your system as `/dev/ttyACM0`.

Feel free to improve it!

# Prerequisites #

To build and run ClockTamer GUI you need:
  * Mercurial version control system
  * PyQt4 development packages
  * pyserial python module

To get those packages on Ubuntu, run:
```
sudo aptitude install mercurial python-qt4-dev pyqt4-dev-tools python-numeric
```

# Building and running on Linux #

After you have installed all prerequisites, you're ready to build the ClockTamer GUI. Download ClockTamer sources from our Mercurial repository:
```
hg clone https://code.google.com/p/clock-tamer/  clock-tamer
```

Compile the GUI:
```
cd clock-tamer/host/tamer-gui
./mk.sh
```

If everything is ok, compilation will run silently and produce a bunch of additional `.py` files. Now run the GUI:
```
./tb.py
```

# Running on Windows #

You need to install:
  * mercurail (e.g. TortoiseHG http://tortoisehg.bitbucket.org/)
  * pyhton (e.g. python-2.7 http://www.python.org/download/releases/2.7.3/)
  * pyqt4 (http://www.riverbankcomputing.co.uk/software/pyqt/download for your version of python)
  * pyserial (http://sourceforge.net/projects/pyserial/files/pyserial/2.5/)

Then clone https://code.google.com/p/clock-tamer/ and run **tbwin.cmd**. (May be you need to update tbwin.cmd to actual path of your python installation)


# Description #

## Console output ##

If you run the GUI from a console, you will see all commands the GUI sends to a ClockTamer and responses to them. This output can be useful for learning [ClockTamer control protocol usage](ControlProtocol.md).

## Outputs numbering ##
![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-pic.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-pic.png)

Picture at the top of the GUI window shows a ClockTamer and outputs numbering scheme. **LVDS differential outputs** are numbered as `N+` and `N-` for positive and negative wires correspondingly, where `N` is the output number. **CMOS outputs** have only one connector and are numbered with a number without `+` or `-` near it.
> Note, that latest ClockTamers have so-called **"half-LVDS"** outputs where only positive wire (`N+`) is accessible to user and negative wire (`N-`) is properly terminated on the ClockTamer PCB. Refer to the [Features and technical specification](FeaturesAndTechnicalSpecification#Output_types.md) page for more details on half-LVDS usage.

## Hardware and software versions ##
![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-hwi-ver.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-hwi-ver.png)

This section of the GUI shows you **version of the ClockTamer hardware (`HWI`)** and **version of the ClockTamer firmware (`VER`)**. Refer to [Control Protocol documentation](ControlProtocol#Version_(%22VER%22).md) for details. This section is read-only.
<br />

## Enabled outputs ##
![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-outputs.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-outputs.png)

This section of the GUI shows you currently enabled outputs and let you to enable/disable them. Set checkbox to enable corresponding output, clear checkbox to disable it. Checkboxes with "LVDS/P+" label correspond to half-LVDS outputs.

Note, that power consumption of the ClockTamer increases considerably with increase of number of enabled outputs. Make sure you provide enough power to the ClockTamer if you enable more then 1 output. E.g. you have to remove current-limiting resistor on USRP1 if you power the ClockTamer from USRP1 fan connector. Alternatively you can use miniUSB cable connected to USB port with 500mA power supply.

_All changes are applied only when you press "Set" button (see below)._
<br />

## Frequency generation options ##
![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-fosc-fout.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-fosc-fout.png)

This section of the GUI shows current values of `Fosc` and `Fout` ClockTamer parameters.

**`Fosc`** parameter should be equal to the real frequency of the reference TCXO/VCTCXO in Hz. Change this value to calibrate your ClockTamer before use. For 20MHz TCXO 1Hz change to `Fosc` equals to 50ppb (1/20e6). Refer to [ClockTamer Calibration](ClockTamerCalibration.md) page for details about calibration calculations.

**`Fout`** parameter equals to ClockTamer output frequency in Hz. Actual output frequency may differ from this value up to calibration precision. Closer `Fosc` value is to the real frequency of the reference TCXO/VCTCXO, closer the actual output frequency is to the `Fout` value.

**`Disable internal oscillator`** checkbox let you disable internal ClockTamer oscillator in case you want to connect an external reference clock source. Most users don't need to do this.

If **`Start on power up`** checkbox is set, ClockTamer will start generating output frequency right after you provide power to it. Otherwise you have to manually start frequency generation. Default is to start frequency generation on power up.

_All changes are applied only when you press "Set" button (see below)._
<br />

## GPS information ##
![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-gps.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-gps.png)

This section is read-only and shows you the state of GPS synchronization. The section is not visible if your ClockTamer doesn't have GPS module.

**`Keep in sync with GPS`** checkbox let you enable/disable automatic synchronization with GPS. Note, that automatic synchronization will not actually occur until GPS module locks to satellites. Satellite lock usually takes less then 60s, so if you don't get lock within this time consider checking GPS antenna connection and sky view. One frequent problem with GPS lock in-doors is window glass with metallic shielding - in this case you have to place antenna outside. _Changes are applied only when you press "Set" button (see below)._

**`In sync`** (green) and **`Out of sync`** (red) labels show current status of GPS lock.

**`1PPS count`** shows the number of 1pps pulses received from the GPS module. This value should increase once a second if GPS is locked to satellites.

**`Calculated Fout`** shows an estimated output frequency. This value becomes meaningful in few seconds after GPS lock acquisition. ClockTamer uses difference between this value and desired `Fout` value for calibration when "`Keep in sync with GPS`" option is enabled.
<br />

## Control buttons ##
![http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-buttons.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-buttons.png)

**`Set`** button applies changes you made in the GUI to the ClockTamer. No changes are applied until you press this button. Changes are NOT saved permanently with this button and will be lost if you power down your ClockTamer.

**`Get`** button reads values from the ClockTamer. Use this button if you want to reset changed you've made in the GUI, but haven't applied to the ClockTamer.

**`Store to EEPROM`** button stores current ClockTamer settings to the permanent storage, which will allow them to survive power down. **Store to EEPROM if you don't want to lose your settings!** Note, that "`Store to EEPROM`" button store to EEPROM current ClockTamer settings, so if you have made any changes in the GUI you have to apply them with "`Set`" button first.

**`Load from EEPROM`** button loads ClockTamer settings from EEPROM, undoing all changes which were made since last "`Store to EEPROM`". If you haven't done "`Store to EEPROM`" yet, this will reset to factory settings.
<br />