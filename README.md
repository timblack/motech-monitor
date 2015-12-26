# motech-monitor
Software to poll Motech and Carlo Gavazzi Grid-Tie Inverters with the option to push the data to a PVOutput.org account. The application is written in C, and can be compiled for x86, x64 and mips-based platforms.

# Examples

Scan for Inverter Address using USB Serial port at 9600bps <br />
./motech -b 1 -s /dev/ttyUSB0 -l <br />

Poll Inverter for data and publish to PVOutput with System ID and API Key specified <br />
./motech -g -p -i 4c4580c965e6f137f2630d93dd7ecdde -k 82712 <br />

# Motivation

Software to automatically poll Motech and Carlo Gavazzi inverters and upload the data to PVOutput.org was not available prior to December 2015. <br />

# Installation

Compiling from source: <br />
1) Clone a copy of the code. <br />
2) Open project in Eclipse. <br />
3) Compile application. <br />

# License

This code is licensed under the GPLv3 license. Please refer to the LICENSE file for more information.
