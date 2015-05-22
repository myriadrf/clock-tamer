**Table of Contents**


# About #

ClockTamer is a device manufactured by Fairwaves to provide a low-cost small-size configurable clock generator with GPS sync option. It is was specifically designed to be used with USRP, but nothing prevents you from using it with other device of your choice given it has external clock input.

ClockTamer is not only Open-Source Software project, but is also an Open-Source Hardware (OSHW) project. All schematics, bill of materials and components placement are published in Downloads section.

This Google Code project holds all source code you may need to work with ClockTamer and various instructions how to use and hack it.

ClockTamer may be used as a reference clock, e.g. for:
  * [GSM](http://en.wikipedia.org/wiki/GSM) (e.g. with [OpenBTS](http://openbts.sourceforge.net))
  * [TETRA](http://en.wikipedia.org/wiki/Terrestrial_Trunked_Radio)
  * [DAB](http://en.wikipedia.org/wiki/Digital_Audio_Broadcasting)
  * [DVB](http://en.wikipedia.org/wiki/Digital_Video_Broadcasting)
  * [GPS](http://en.wikipedia.org/wiki/Gps)
  * [RFID](http://en.wikipedia.org/wiki/Radio-frequency_identification)
  * [LTE](http://en.wikipedia.org/wiki/3GPP_Long_Term_Evolution)
  * [WiMAX](http://en.wikipedia.org/wiki/WiMAX)
  * many others...

Use the **[ClockTamer mailing list](https://groups.google.com/group/clocktamer)** to ask questions and for discussions.

<a href='Hidden comment: 
= Buy it! =

Purchase !ClockTamer from *[http://shop.fairwaves.ru/ Fairwaves web-shop]*.
'></a>

# News #

_12 Dec 2012_
> Added instruction how to run GUI on Windows http://code.google.com/p/clock-tamer/wiki/GUI#Running_on_Windows.

_21 May 2011_
> Oscilloscope measurements for ClockTamer-1.2 has been published at [Features and technical specification](http://code.google.com/p/clock-tamer/wiki/FeaturesAndTechnicalSpecification#Output_waveform_of_ClockTamer_-1.2) page. Thanks to [Sylvain Munaut](http://www.246tnt.com/) for this pictures.

_24 January 2011_
> Alpha version of ClockTamer control GUI is available. Check it out from our source repository, under [host/tamer-gui](http://code.google.com/p/clock-tamer/source/browse/#hg%2Fhost%2Ftamer-gui). At this moment [documentation](http://code.google.com/p/clock-tamer/wiki/GUI) is almost non-existent and we invite everyone to improve it!

> [![](http://wiki.clock-tamer.googlecode.com/hg/images/misc/tamer-gui-small.png)](http://code.google.com/p/clock-tamer/wiki/GUI)

_09 January 2011_
> ClockTamer is now available for European buyers from [Olifantasia](http://www.olifantasia.com/drupal2/en/node/12)! US coverage is still in works. If you need ClockTamer and you're not from Europe - you could get one from Olifantasia (they ship internationally) or [contact us privately](http://shop.fairwaves.ru/index.php?route=information/contact).

> Other news is that we now have a [mailing list](https://groups.google.com/group/clocktamer) for the project.  Join it now!

> ![![](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-top-small.jpg)](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-top.jpg)


_04 October 2010_
> We very much appreciate all your interest to ClockTamers and it's not very easy to say this, but start of the sales is delayed again. A fab assembling ClockTamers failed to deliver our order on time again. We're working hard to sort this out and hope to get our ClockTamers next week to make them available for sale in the next two-three weeks. I can't express how much we're frustrated with all these delays and how much we do want to get ClockTamers ready for sale as soon as possible. We appreciate your patience, please stay with us just a little bit more.

_28 September 2010_
> Update about ClockTamer production. PCB fab succeed in manufacturing our PCBs only from 3rd attempt, and so we have 3 weeks of delay. Tentative estimate is that we'll have ClockTamers available for sale at the end of Oct. Stay tunned!


_5 August 2010_
> ![http://wiki.clock-tamer.googlecode.com/hg/images/misc/blackhat-logo.png](http://wiki.clock-tamer.googlecode.com/hg/images/misc/blackhat-logo.png)<br /> ClockTamer becomes a great helper for security research! Chris Paget used ClockTamer during his BlackHat and Defcon talks about [cellphone spying](http://www.tombom.co.uk/blog/?p=262) and [extreme-range RFID reading](http://www.tombom.co.uk/blog/?p=249) (this fact is mentioned in the slides).

> Answering a popular question - we don't have any ClockTamers in stock right now. We estimate to finish production run at the end of September and stock them soon after that. Look for the news for more information. You could also drop us an e-mail [here](http://shop.fairwaves.ru/index.php?route=information/contact) and we'll notify you when ClockTamers become available.


_11 July 2010_
> Technical parameters of ClockTamer-1.1 and ClockTamer-1.2 including schematics, PCB layout and bill of materials are available at this page now: [Features and technical specification](FeaturesAndTechnicalSpecification.md)


_20 June 2010_
> Here is a picture of ClockTamer-1.1beta. A small hardware patch you may see on the picture near the USB connector is what makes it "beta" (patch doesn't affect performance, though):

> ![![](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.1/ClockTamer-1.1-top-small.jpg)](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.1/ClockTamer-1.1-top.jpg)

> It's smaller then ClockTamer-1.0, because we removed useless COM-port connector and few other unused elements. Now it's 35x86mm.


_17 June 2010_
> We've measured phase noise picture of ClockTamer-1.2 (the final version). You may find it [here](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.2/ClockTamer-1.2-phase-noise.png).


_3 May 2010_
> We've created a small Dev-kit to simplify re-flashing and debugging of ClockTamer firmware. With it you don't need to solder anything and you can connect you programmer to ClockTamer easily. We're about to start selling this kits for $35 - write us if you want one. Schematics of the Dev-kit will be published as usual.


_1 May 2010_
> Production of ClockTamer-1.1 is approaching. We've received beta-samples of ClockTamer-1.1 PCBs from the fab and tests show they're functional. Mass-production of ClockTamer-1.1 is about to start soon!


_25 February 2010_
> We finally managed to publish [schematics and component placement for ClockTamer-1.0](http://clock-tamer.googlecode.com/files/ClockTamer-1.0-schematics.zip). Check it out in Downloads section. Right now it contains component placement only for the top side and miss bill of materials. We're working on fixing this.

# Documentation #

User documentation:
  * [Features and specifications.](FeaturesAndTechnicalSpecification.md)
  * [Connecting ClockTamer to USRP.](ClockTamerUSRPInstallation.md)
  * [Calibration HOWTO.](ClockTamerCalibration.md)
  * [GUI manual.](GUI.md)
  * [Control protocol.](ControlProtocol.md)
  * [Using ClockTamer with GPS.](ClockTamerGPS.md)
  * [FAQ.](FAQ.md)

Developer documentation:
  * [How to build and flash ClockTamer under Linux.](LinuxBuildHowto.md)
  * [Description of ClockTamer SPI protocol and USRP connector pinout.](ProtocolSPI.md)
  * [Implementation of the GPS lock.](GPSLocking.md)

Other:
  * [ToDo for future development](WishList.md)
  * [Software versions history](SWVersionHistory.md)

# Licensing #

All project's source code is licensed under LGPL license.

Schematics and their derivatives are licensed under Creative Commons Attribution-Share Alike 3.0 license (CC-BY-SA).

Host part of the project is licensed under New BSD license.

We're sorry for mixing all these licenses in a single project, but we see this way as the most appropriate.

# Pictures #

![![](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.0/small/top_cable_2.jpg)](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.0/top_cable_2.jpg) ![![](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.0/small/bottom_1.jpg)](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.0/bottom_1.jpg)
![![](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.0/small/with_usrp_1.jpg)](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.0/with_usrp_1.jpg) ![![](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.0/small/with_usrp_2.jpg)](http://wiki.clock-tamer.googlecode.com/hg/images/clocktamer-v1.0/with_usrp_2.jpg)