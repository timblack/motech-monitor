# motech-monitor
Software to poll Motech and Carlo Gavazzi Grid-Tie Inverters with the option to push the data to a PVOutput.org account. The application is written in C, and can be compiled for x86, x64 and mips-based platforms. <br />

# Command-Line Arguments

```
Serial Port Arguments
	-b x		    Baud Rate(1=9600(Default), 2=19200)
	-s dev_name	    Serial Port Device Name(/dev/ttyUSB0 (Default))

Inverter Arguments
	-a x		    Inverter Address(45 (Default))
	-l		        Look/Scan for Inverter Address (0=Off(Default), 1=On)
	-g		        Get Inverter Data (0=Off(Default), 1=On)

PVOutput Arguments
	-p		        Publish Data to PVOutput (0=Off(Default), 1=On)
	-i sys_id	    PVOutput System ID
	-k api_key	    PVOutput API Key

Restart-On-Failure Arguments
	-c		        Restart on Failure Flag (0=Off(Default), 1=On)
	-c x		    Max Number of Failures before Restart(300 (Default))
	-e x		    Hour to start monitoring failures (8 (Default))
	-f x		    Hour to stop monitoring failures (16 (Default))
```

# Examples

```
Scan for Inverter Address using USB Serial port at 9600bps
./motech -b 1 -s /dev/ttyUSB0 -l

Poll Inverter for data and publish to PVOutput with System ID and API Key specified
./motech -g -p -i 4c4580c965e6f137f2630d93dd7ecdde -k 82712
```

# Installation

The application can be compiled using gcc. I've used Eclipse for Linux to manage the project.

# Putting together a low-powered Inverter poller

This process isn't for the faint of heart. However, the result can be very rewarding (being able to monitor the inverter with a device consuming <0.5 watts). The instructions below are vague, and a lot more detailed steps are involved. If in doubt, flick me an email and I'll try and provide more details.<br />

Parts:<br />
1) A wireless router capable of running OpenWrt (eg TP-Link TL-WR703N) <br />
2) CAT5 Cable (one should come with the router). <br />
3) RJ-45-to-DB9 adapter <br />
4) USB-to-Serial adapter (Prolific or FTDI)<br />

Instructions:<br />
1) Install OpenWrt on the router <a href="https://wiki.openwrt.org/toh/tp-link/tl-wr703n">(eg TL-WR703N)</a><br />
2) Install the relevant USB-to-serial drivers on the router.<br />
3) Set up the wireless router to connect to your home/office router. <br />
4) Copy over the compiled motech-monitor application to the router.<br />
5) Test the application using the -l command to find the inverter address.<br />
6) Add a cron job to the scheduled task to have the motech application run every 5 minutes with the appropriate flags.

References:<br/>
1) <a href="https://wiki.openwrt.org/toh/tp-link/tl-wr703n">OpenWRT for TP-Link TL-WR703N</a><br />
2) <a href="http://www.motechsolar.com/en/doc/inverter/ZOMG-8146DMTEU2_2012-3-14.pdf">Motech Inverter RJ-45 to DB9 Cable(Figure 3.3.3.4 on Page 30)</a><br />
3) <a href="http://www.altronics.com.au/p/p1690-de9-male-to-rj45-modular-female-adapter/">RJ-45 to DB9 Adapter</a><br />
4) <a href="http://downloads.openwrt.org/attitude_adjustment/12.09/ar71xx/generic/OpenWrt-Toolchain-ar71xx-for-mips_r2-gcc-4.6-linaro_uClibc-0.9.33.2.tar.bz2">OpenWrt Toolchain ar71xx for mips</a><br />

# Motivation

Software to automatically poll Motech and Carlo Gavazzi inverters and upload the data to PVOutput.org was not available prior to December 2015. This application is there to fill that gap.<br />

# License

This code is licensed under the GPLv3 license. Please refer to the LICENSE file for more information.
