# Table of Contents #


# Hardware and usage #

## USRP doesn't work with ClockTamer ##

Check that you've modified USRP exactly as described at [USRP re-clocking modification](ClockTamerUSRPInstallation.md) page.

## What is a tunning range of ClockTamer? ##

It depends on what oscillator and VCO+PLL chips you have installed. Default configuration is to have 10MHz of 20MHz TCXO and LMX2531LQ2080E VCO+PLL installed. This gives you 2.84-65.83MHz continous tunning range and few ranges in higher frequencies. If you have LMX2531LQ1515E VCO+PLL installed, then you should have following set of tunning ranges: 1.42-32.92MHz, 32.95-35.91MHz, 36.25-39.50MHz, 40.28-43.89MHz, 45.31-49,38MHz, 51.79-56.43MHz, 60.42-65.83MHz, 72.50-79.00MHz, 90.63-98.75MHz, 120.83-131.67MHz.

## Board with GPS does not seem to work ##

Tamers with GPS won't work with stock USRP power, because they need more current that it can give through fan connector. You either need external power or have to replace R7 current-limiting resistor on USRP with 0-Ohm resistor. R7 is located near fan connector on USRP motherboard.

## What VID/PID ClockTamer use for USB connection? ##

VID=0x03EB (ATMEL) PID=0x2FFA (ATUSB162)

## ClockTamer is not recognized as serial device on my system ##

Try following:
  1. Disconnect USB cable from ClockTamer.
  1. Power it off.
  1. Power it on.
  1. If LED is lighting, then ClockTamer booted to flashing mode. Check that it does not contact with any conductive areas and that you don't touch ClockTamer pins with your hand. In this case try to turn power off and on again with care.
  1. If LED is not lighting, then ClockTamer has booted correctly or is not working at all. Try connecting USB cable to computer and then to ClockTamer. If ClockTamer is working, LED should flash once and then light constantly.

## How much current ClockTamer draws? ##

It depends on ClockTamer version and hardware modification you have. Several common modifications for ClockTamer-v1.0 are listed further:

| modification | current |
|:-------------|:--------|
| LMX=2080 LMK=1010 OSC=20 with 1 output | 0.13-0.14 A |
| LMX=2080 LMK=1010 OSC=20 with 2 outputs | 0.15-0.16 А |
| LMX=2080 LMK=1010 OSC=26 VCTXCO GPS with 1 output | 0.31 А |

## What is the difference between two power connectors on ClockTamer? ##

They are short-circuited and thus equivalent. There is no difference to which one you connect power and to which one you connect USRP fan.

## CMOS output gives my 5V peak-to-peak output. Isn't it out of spec for USRP? ##

Actually it's supposed to be a 3.3V output.

But due to the reflection / ringing of the signal at those rise-times,
the signal voltage almost double as overshoot.

If you terminate USRP's clock input with 50 Ohm resistor, you'll get nice 2V peak-to-peak signal. Check [USRP re-clocking modification](ClockTamerUSRPInstallation.md) page to get information how to install 50 Ohm resistor, and [Features and technical specification](FeaturesAndTechnicalSpecification#Output_waveform_of_ClockTamer_-1.2.md) page for scope pictures of a correctly installed ClockTamer.

## Wavelength of 64 MHz is pretty long. Do I really need to terminate with 50 Ohm? ##

Note that the signal is a square wave, not sinus. So the rising times
are short ( ~ 15 ns [as measured](FeaturesAndTechnicalSpecification#Output_waveform_of_ClockTamer_-1.2.md) ), that means the highest frequency
component of the signal is way more than 64 MHz.



# Software #


## What is a difference between "`SET,,OSC`" and "`SET,,OUT`" commands? ##

_(Below it is assumed you have ClockTamer with 20MHz TCXO installed)_

OSC and OUT are just different values. OSC is a frequency of hardware oscillator on ClockTamer, while OUT is a desired output frequency. If OSC is set to incorrect value, then ClockTamer will generate wrong frequency at output (or fail to generate anything at all, as you saw before). So "calibration" should be done by changing OSC - then changing OUT will always give you good (calibrated) output. The only problem with OSC value is that it has 1Hz granularity at 20MHz, i.e. at output you will have 52/20\*1Hz=2.6Hz granularity or calibration and at 900MHz you will have 900/20\*1Hz=45Hz granularity of calibration. After calibration is done you can tune output with slightly better granularity (1Hz at 52MHz instead of 2.6Hz at 52MHz) by changing OUT value.

So, to make entirely clear. Changing OSC value you preserve calibration among different output frequencies. Changing OUT value you naturally can't preserve calibration among different output frequencies, but if you're interested in only one frequency to be generated, it's fine to use OUT for fine-tunning.