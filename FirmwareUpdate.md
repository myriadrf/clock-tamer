# ClockTamer 1.30 #

  1. Download dfu-programmer http://dfu-programmer.sourceforge.net/ (available both for linux and windows)
  1. On windows you need to unstall libusb drivers (included in dfu-programmer
  1. Download latest firmware [Tamer.hex](https://clock-tamer.googlecode.com/hg-history/new-lufa/board/FW/Tamer.hex) and [Tamer.eep](https://clock-tamer.googlecode.com/hg-history/new-lufa/board/FW/Tamer.eep)
  1. Enter firmware update mode.
    1. Power off ClockTamer - disconnect power cord AND mini-USB cable.
    1. Short-circuit `nSS` pin of `GND` (full connector pinout is available [here](ProtocolSPI#ClockTamer_pinout.md)). You can short these pins on the bottom side header using any wire or office clip.<br />![http://wiki.clock-tamer.googlecode.com/hg/images/misc/clocktamer-nSS.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/clocktamer-nSS.png)
    1. Connect ClockTamer to a computer with mini-USB cable. LED should be flashing immediately.
    1. Remove short-circuit from `nSS` to `GND`.
  1. Update flash (e.g. on windows)
```
dfu-programmer.exe atmega32u2 flash --debug=100 --suppress-bootloader-mem c:\tools\clock-tamer\board\FW\Tamer.hex
```
  1. Update eeprom (e.g. on windows)
```
dfu-programmer.exe atmega32u2 flash-eeprom --debug=100 c:\tools\clock-tamer\board\FW\Tamer.eep
```

NOTE: Since there's no option to read firmware back from device on ClockTamer these commands fail. But it should fail only on validating operation, like this:

```
c:\tools\dfu-programmer-win-0.6.0>dfu-programmer.exe atmega32u2 flash --debug=100 --suppress-bootloader-mem c:\tools\clock-tamer\board\FW\Tamer.hex
     target: atmega32u2
    chip_id: 0x2ff0
  vendor_id: 0x03eb
    command: flash
      quiet: false
      debug: 100
device_type: AVR
------ command specific below ------
   validate: true
   hex file: c:\tools\clock-tamer\board\FW\Tamer.hex

commands.c:327: write 14488/28672 bytes
atmel.c:1010: atmel_flash( 0022FEAC, 01A40048, 0, 28672, 128, false )
atmel.c:847: atmel_select_page( 0022FEAC, 0 )
atmel.c:1085: valid block length: 11836, (0 - 11836)
atmel.c:1222: atmel_flash_block( 0022FEAC, 01A40048, 0, 1024, false )
atmel.c:1179: atmel_flash_populate_header( 0022F8B0, 0, 1024, false )
atmel.c:1246: 1024 bytes to MCU 000000
atmel.c:1132: atmel_flash_populate_footer( 0022F8B0, 0022FCD0, 65535, 65535, 65535 )
atmel.c:1256: message length: 1072
atmel.c:1105: Next first: 1024
atmel.c:1107: valid block length: 10812
atmel.c:1222: atmel_flash_block( 0022FEAC, 01A40848, 1024, 1024, false )
atmel.c:1179: atmel_flash_populate_header( 0022F8B0, 1024, 1024, false )
atmel.c:1246: 1024 bytes to MCU 000400
atmel.c:1132: atmel_flash_populate_footer( 0022F8B0, 0022FCD0, 65535, 65535, 65535 )
atmel.c:1256: message length: 1072
atmel.c:1105: Next first: 2048
atmel.c:1107: valid block length: 9788
atmel.c:1222: atmel_flash_block( 0022FEAC, 01A41048, 2048, 1024, false )
atmel.c:1179: atmel_flash_populate_header( 0022F8B0, 2048, 1024, false )
atmel.c:1246: 1024 bytes to MCU 000800
atmel.c:1132: atmel_flash_populate_footer( 0022F8B0, 0022FCD0, 65535, 65535, 65535 )
atmel.c:1256: message length: 1072
atmel.c:1105: Next first: 3072
....
atmel.c:1246: 572 bytes to MCU 002c00
atmel.c:1132: atmel_flash_populate_footer( 0022F8B0, 0022FB0C, 65535, 65535, 65535 )
atmel.c:1256: message length: 620
atmel.c:1105: Next first: 11836
atmel.c:1107: valid block length: 0
atmel.c:1109: sent: 11836, first: 11836 last: 11836
Validating...
atmel.c:579: atmel_read_flash( 0022FEAC, 0x00000000, 0x00007000, 002C4210, 28672, false )
atmel.c:847: atmel_select_page( 0022FEAC, 0 )
atmel.c:510: __atmel_read_page( 0022FEAC, 0, 28672, 002C4210, false )
atmel.c:537: result: -5
Unknown error.  Try enabling debug.
commands.c:346: Error while validating.
Error while validating.
```