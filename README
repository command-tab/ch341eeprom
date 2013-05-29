
About   
=====

	Beta libusb code for EEPROM programmers based on the WinChipHead CH341a IC

Author
======

	Written by asbokid and released under the terms of the GNU GPL, version 3, or later.
	
	Copyright Dec 2011, asbokid <ballymunboy@gmail.com>

Licence
=======

	This is free software: you can redistribute it and/or modify it under the terms of
	the latest GNU General Public License as published by the Free Software Foundation.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.
	If not, see <http://www.gnu.org/licenses/>.

Requires
========

	gcc
	GNU make
	libusb-1.0 and the libusb library development files. See http://libusb.wiki.sourceforge.net/
    libusb-1.0-dev  (on debian-based distros)

Compiling
=========

	gcc -o ch341eeprom ch341eeprom.c ch341funcs.c -lusb-1.0
		
Running
=======

    asbokid@u50si1:$ ./ch341eeprom 

    ch341eeprom - an i2c EEPROM programming tool for the WCH CH341a IC
    Version 0.5 copyright (c) 2011  asbokid <ballymunboy@gmail.com>

    This program comes with asbolutely no warranty; This is free software,
    and you are welcome to redistribute it under certain conditions:
    GNU GPL v3 License: http://www.gnu.org/licenses/gpl.html

    Usage:
     -h, --help             display this text
     -v, --verbose          verbose output
     -d, --debug            debug output
     -s, --size             size of EEPROM {24c32|24c64}
     -e, --erase            erase EEPROM (fill with 0xff)
     -w, --write <filename> write EEPROM with image from filename
     -r, --read  <filename> read EEPROM and save image to filename

    Example:  ch341eeprom -v -s 24c64 -w bootrom.bin

    asbokid@u50si1:$ sudo ./ch341eeprom -v -s 24c64 -e

    Searching USB buses for WCH CH341a i2c EEPROM programmer [1a86:5512]
    Found [1a86:5512] as device [7] on USB bus [2]
    Opened device [1a86:5512]
    Claimed device interface [0]
    Device reported its revision [3.03]
    Configured USB device
    Set i2c bus speed to [100kHz]
    Erased [8192] bytes of [24c64] EEPROM
    Closed USB device

    asbokid@u50si1:$ sudo ./ch341eeprom -v -s 24c64 -r output.bin

    Searching USB buses for WCH CH341a i2c EEPROM programmer [1a86:5512]
    Found [1a86:5512] as device [7] on USB bus [2]
    Opened device [1a86:5512]
    Claimed device interface [0]
    Device reported its revision [3.03]
    Configured USB device
    Set i2c bus speed to [100kHz]
    Read [8192] bytes from [24c64] EEPROM
    Wrote [8192] bytes to file [output.bin]
    Closed USB device

    asbokid@u50si1:$ xxd -l 128 output.bin

    0000000: ffff ffff ffff ffff ffff ffff ffff ffff  ................
    0000010: ffff ffff ffff ffff ffff ffff ffff ffff  ................
    0000020: ffff ffff ffff ffff ffff ffff ffff ffff  ................
    0000030: ffff ffff ffff ffff ffff ffff ffff ffff  ................
    0000040: ffff ffff ffff ffff ffff ffff ffff ffff  ................
    0000050: ffff ffff ffff ffff ffff ffff ffff ffff  ................
    0000060: ffff ffff ffff ffff ffff ffff ffff ffff  ................
    0000070: ffff ffff ffff ffff ffff ffff ffff ffff  ................

    asbokid@u50si1:$ ./mktestimg > testimg24c64.bin

    asbokid@u50si1:$ xxd -l 128 testimg24c64.bin

    0000000: 0000 0000 0000 0000 0000 0000 0000 0000  ................
    0000010: 1111 1111 1111 1111 1111 1111 1111 1111  ................
    0000020: 2222 2222 2222 2222 2222 2222 2222 2222  """"""""""""""""
    0000030: 3333 3333 3333 3333 3333 3333 3333 3333  3333333333333333
    0000040: 4444 4444 4444 4444 4444 4444 4444 4444  DDDDDDDDDDDDDDDD
    0000050: 5555 5555 5555 5555 5555 5555 5555 5555  UUUUUUUUUUUUUUUU
    0000060: 6666 6666 6666 6666 6666 6666 6666 6666  ffffffffffffffff
    0000070: 7777 7777 7777 7777 7777 7777 7777 7777  wwwwwwwwwwwwwwww

	asbokid@u50si1:$ sudo ./ch341eeprom -v -s 24c64 -w testimg24c64.bin 

    Searching USB buses for WCH CH341a i2c EEPROM programmer [1a86:5512]
    Found [1a86:5512] as device [7] on USB bus [2]
    Opened device [1a86:5512]
    Claimed device interface [0]
    Device reported its revision [3.03]
    Configured USB device
    Set i2c bus speed to [100kHz]
    Read [8192] bytes from file [testimg24c64.bin]
    Wrote [8192] bytes to [24c64] EEPROM
    Closed USB device

    asbokid@u50si1:$ sudo ./ch341eeprom -v -s 24c64 -r output.bin

    Searching USB buses for WCH CH341a i2c EEPROM programmer [1a86:5512]
    Found [1a86:5512] as device [7] on USB bus [2]
    Opened device [1a86:5512]
    Claimed device interface [0]
    Device reported its revision [3.03]
    Configured USB device
    Set i2c bus speed to [100kHz]
    Read [8192] bytes from [24c64] EEPROM
    Wrote [8192] bytes to file [output.bin]
    Closed USB device

    asbokid@u50si1:$ xxd -l 128 output.bin 

    0000000: 0000 0000 0000 0000 0000 0000 0000 0000  ................
    0000010: 1111 1111 1111 1111 1111 1111 1111 1111  ................
    0000020: 2222 2222 2222 2222 2222 2222 2222 2222  """"""""""""""""
    0000030: 3333 3333 3333 3333 3333 3333 3333 3333  3333333333333333
    0000040: 4444 4444 4444 4444 4444 4444 4444 4444  DDDDDDDDDDDDDDDD
    0000050: 5555 5555 5555 5555 5555 5555 5555 5555  UUUUUUUUUUUUUUUU
    0000060: 6666 6666 6666 6666 6666 6666 6666 6666  ffffffffffffffff
    0000070: 7777 7777 7777 7777 7777 7777 7777 7777  wwwwwwwwwwwwwwww


Concluding Notes	
================

    The code handles the 3 byte addressing used by EEPROMS of 32kbit and greater (24c32-)
    It uses asynchronous USB transfers but should be portable to Microsoft Windows.

	All comments and contributions welcomed!
	
	asbokid <ballymunboy@gmail.com> - Dec 2011
