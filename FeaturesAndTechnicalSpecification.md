# Table of Contents #



# Output types #

ClockTamer comes with one or several of the following output types:

  * **CMOS** - single-ended output, <3V peak-to-peak. The most popular option for low output frequencies (<200 MHz)
  * **LVDS** - differential output, <0.5V peak-to-peak. Useful if you want to generate frequencies above 200 MHz. Requires correct termination of both ends of LVDS output. You cannot use one end and leave the other floating.
  * "**half-LVDS**" - single-ended output, <0.5V peak-to-peak. In half-LVDS outputs only positive end (N+) is accessible to a user, while negative end (N-) is terminated with 50 Ohm resistor on the ClockTamer PCB. This output has the same frequency limitation as CMOS (<200 MHz), but has lower peak-to-peak voltage.

Users of USRP can use either CMOS or half-LVDS outputs, though CMOS is the default and is the most popular option.

# ClockTamer-1.2 #

**ClockTamer-1.2 with a single CMOS output and no GPS:** <br />
| ![![](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-top-small.jpg)](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-top.jpg) | ![![](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-back-small.jpg)](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-back.jpg) |
|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

**ClockTamer-1.2 with all outputs and GPS:** <br />
| ![![](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-all-top-small.jpg)](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-all-top.jpg) | ![![](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-GPS-back-small.jpg)](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-GPS-back.jpg) |
|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

## Specification of ClockTamer-1.2 ##

| Output clock temperature stability | 0.28ppm in 0-70C temperature range (without GPS option)<br />or <br /> 50ppb with GPS option |
|:-----------------------------------|:---------------------------------------------------------------------------------------------|
| Outputs                            | up to 2 CMOS outputs<br /> up to 5 LVDS outputs<br /> (default is 1 CMOS and 0 LVDS outputs) |
| Output connector type              | U.FL                                                                                         |
| Input power voltage                | 3.7 - 6.0V                                                                                   |
| Input power types                  | miniUSB (enough to power ClockTamer with 1 or 2 outputs with or without GPS)<br /> or<br /> 6V 2-pin connector (it has 2 connectors to connect into daisy-chain with USRP fan) |
| Surge protection                   | 6V power input is protected with fast acting resetable fuse                                  |
| Size                               | 35mm x 86mm x 14mm                                                                           |
| Control                            | miniUSB and SPI (16-pin connector for easy connection to USRP)                               |

## Output frequencies of ClockTamer-1.2 ##

Output frequency can be set with 1Hz precision (this is a firmware limitation and could be fixed in later firmware versions). Tunning can be done in the following ranges:

| 	| CMOS	| LVDS/LVPECL |
|:-|:-----|:------------|
| 3.75 - 94.75 MHz	| X	   | X           |
| 95.20 - 113.70 MHz	| X	   | X           |
| 119.00 - 142.13 MHz	| X	   | X           |
| 158.67 - 189.50 MHz	| X	   | X           |
| 238.00 - 284.25 MHz	| 	    | X           |
| 476.00 - 568.50 MHz	| 	    | X           |
| 952.00 - 1137.00 MHz	| 	    | X           |

## Schematics, PCB layout and bill of materials of ClockTamer-1.2 ##

  * Schematics: [(pdf)](http://wiki.clock-tamer.googlecode.com/hg/schematics/clocktamer-1.2/ClockTamer-1.2-schematics.pdf)
  * PCB layout: [top (png)](http://wiki.clock-tamer.googlecode.com/hg/schematics/clocktamer-1.2/ClockTamer-1.2-PCB-top.png), [bottom (png)](http://wiki.clock-tamer.googlecode.com/hg/schematics/clocktamer-1.2/ClockTamer-1.2-PCB-bottom.png)
  * Bill of materials: [(xls)](http://wiki.clock-tamer.googlecode.com/hg/schematics/clocktamer-1.2/ClockTamer-1.2-BOM.xls)

Links to datasheets of the most important parts:
  * Atmel ATMega (AT90USB162/AT90USB82) - [(pdf)](http://www.atmel.com/dyn/resources/prod_documents/doc7707.pdf)
  * Connor Winfield D75A (TCXO, 10MHz/20MHz 0.28ppm) - [(pdf)](http://www.conwin.com/datasheets/tx/tx236.pdf)
  * National Semiconductor LMX2531LQ1515E/LMX2531LQ2080E (PLL + fractional VCO) - [(pdf)](http://www.national.com/profile/snip.cgi/openDS=LMX2531)
  * National Semiconductor LMK01000/LMK01010 (Clock Buffer, Divider, and Distributor) - [(pdf)](http://www.national.com/profile/snip.cgi/openDS=LMK01000)

## Phase noise of ClockTamer-1.2 ##

Measured for 52MHz output.

![http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-phase-noise.png](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-phase-noise.png)

## Output waveform of ClockTamer-1.2 ##

Measurements made by [Sylvain Munaut](http://www.246tnt.com/).

```
 USRP is modified with 51R resistor across the SMA socket on the USRP.

 Measured with a 350 MHz scope and a 4GHz active probe freshly
 calibrated with as short a ground lead as you can make.
 
 It shows a dead center 2 Vpp, low ringing, and fast enough rise time
 to meet the USRP clock chip input requirements.
```

<a href='http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/measurements/clocktamer_cmos_at_usrp.png'><img src='http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/measurements/clocktamer_cmos_at_usrp.png' width='400px></a'>

<b>Measurement for ClockTamer CMOS output</b>

<a href='http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/measurements/clocktamer_lvds_at_usrp.png'><img src='http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/measurements/clocktamer_lvds_at_usrp.png' width='400px></a'>

<b>Measurement for ClockTamer single-ended LVDS output</b>

<br />
<br />
<hr />

<h1>ClockTamer-1.1</h1>

<a href='http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.1/ClockTamer-1.1-top.jpg'><img src='http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.1/ClockTamer-1.1-top-small.jpg' /></a>

<h2>Specification of ClockTamer-1.1</h2>

<table><thead><th> Output clock temperature stability </th><th> 0.28ppm in 0-70C temperature range (without GPS option)<br />or <br /> 50ppb with GPS sync option </th></thead><tbody>
<tr><td> Outputs                            </td><td> up to 2 CMOS outputs<br /> up to 3 LVPECL/LVCMOS outputs<br /> up to 2 LVDS outputs<br /> (default is 1 CMOS, 0 LVPECL/LVCMOS and 0 LVDS outputs) </td></tr>
<tr><td> Output connector type              </td><td> U.FL                                                                                              </td></tr>
<tr><td> Power                              </td><td> miniUSB<br /> or<br /> 6V 2-pin connector (2 inputs to connect into daisy-chain with USRP fan)    </td></tr>
<tr><td> Surge protection                   </td><td> 6V power input is protected with fast acting resetable fuse                                       </td></tr>
<tr><td> Size                               </td><td> 35mm x 86mm x 14mm                                                                                </td></tr>
<tr><td> Control                            </td><td> miniUSB and SPI (16-pin connector for easy connection to USRP)                                    </td></tr></tbody></table>

<h2>Output frequencies of ClockTamer-1.1</h2>

Output frequency can be set with 1Hz precision (this is a firmware limitation and could be fixed in later firmware versions). Tunning can be done in the following ranges:<br>
<br>
<table><thead><th> 	</th><th> CMOS	</th><th> LVDS/LVPECL </th></thead><tbody>
<tr><td> 1.42 - 32.92 MHz	</td><td> X	   </td><td> X           </td></tr>
<tr><td> 32.95 - 35.91 MHz	</td><td> X	   </td><td> X           </td></tr>
<tr><td> 36.25 - 39.50 MHz	</td><td> X	   </td><td> X           </td></tr>
<tr><td> 40.28 - 43.89 MHz	</td><td> X	   </td><td> X           </td></tr>
<tr><td> 45.31 - 49.38 MHz	</td><td> X	   </td><td> X           </td></tr>
<tr><td> 51.79 - 56.43 MHz	</td><td> X	   </td><td> X           </td></tr>
<tr><td> 60.42 - 65.83 MHz	</td><td> X	   </td><td> X           </td></tr>
<tr><td> 72.50 - 79.00 MHz	</td><td> X	   </td><td> X           </td></tr>
<tr><td> 90.63 - 98.75 MHz	</td><td> X	   </td><td> X           </td></tr>
<tr><td> 120.83 - 131.67 MHz	</td><td> X	   </td><td> X           </td></tr></tbody></table>

<h2>Schematics, PCB layout and bill of materials of ClockTamer-1.1</h2>

<ul><li>Bill of materials: <a href='http://wiki.clock-tamer.googlecode.com/hg/schematics/clocktamer-1.1/ClockTamer-1.1-BOM.xls'>(xls)</a></li></ul>

Schematics is the same as in ClockTamer-1.2, PCB layout has only minor changes comparing to ClockTamer-1.2. <a href='#Schematics,_PCB_layout_and_bill_of_materials_of_ClockTamer_-1.2.md'>Find !ClockTamer-1.2 schematics and PCB layout here</a>.