//
//   ch341eeprom programmer version 0.6 (Beta)
//
//   Programming tool for the 24Cxx serial EEPROMs using the Winchiphead CH341A IC
//
//   (c) December 2011 asbokid <ballymunboy@gmail.com> 
//   (c) December 2023 aystarik <aystarik@gmail.com>
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#define _POSIX_C_SOURCE 1

#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <limits.h>
#include <sys/mman.h>

#include "ch341eeprom.h"

FILE *debugout = NULL, *verbout = NULL;

int main(int argc, char **argv)
{
    int i, eepromsize = 0, bytesread = 0;
    uint8_t debug = false, verbose = false;
    struct libusb_device_handle *devHandle = NULL;
    const char *filename = NULL, *eepromname = NULL;
    char operation = 0;
    uint32_t speed = CH341_I2C_STANDARD_SPEED;
    uint8_t *verifybuf;
    uint8_t verify_failed = false;
    FILE *fp;

    const struct EEPROM *eeprom_info;

    static char version_msg[] =
        "ch341eeprom - an i2c EEPROM programming tool for the WCH CH341a IC\n"
        "Version " CH341TOOLVERSION
        " copyright (c) 2011 asbokid <ballymunboy@gmail.com>\n\n"
        "This program comes with absolutely no warranty; This is free software,\n"
        "and you are welcome to redistribute it under certain conditions:\n" "GNU GPL v3 License: http://www.gnu.org/licenses/gpl.html\n";

    static char usage_msg[] =
        "Usage:\n"
        " -h, --help              display this text\n"
        " -v, --verbose           verbose output\n"
        " -d, --debug             debug output\n"
        " -s, --size              size of EEPROM {24c01|24c02|24c04|24c08|24c16|24c32|24c64|24c128|24c256|24c512|24c1024}\n"
        " -e, --erase             erase EEPROM (fill with 0xff)\n"
        " -p, --speed             i2c speed (low|fast|high) if different than standard which is default\n"
        " -w, --write  <filename> write EEPROM with image from filename\n"
        " -r, --read   <filename> read EEPROM and save image to filename\n"
        " -V, --verify <filename> verify EEPROM contents against image in filename\n\n" "Example: ch341eeprom -v -s 24c64 -w bootrom.bin\n";

    static struct option longopts[] = {
        { "help", no_argument, 0, 'h' },
        { "verbose", no_argument, 0, 'v' },
        { "debug", no_argument, 0, 'd' },
        { "erase", no_argument, 0, 'e' },
        { "size", required_argument, 0, 's' },
        { "speed", required_argument, 0, 'p' },
        { "read", required_argument, 0, 'r' },
        { "write", required_argument, 0, 'w' },
        { "verify", required_argument, 0, 'V' },
        { "scan", no_argument, 0, 'x' },
        { 0, 0, 0, 0 }
    };

    static int speed_table[] = { 20, 100, 400, 750 };

    while (true) {
        int32_t optidx = 0;
        int8_t c = getopt_long(argc, argv, "hvdexs:p:w:r:V:", longopts, &optidx);
        if (c == -1)
            break;

        switch (c) {
        case 'h':
            fprintf(stdout, "%s\n%s", version_msg, usage_msg);
            return 0;
        case 'v':
            verbose = true;
            break;
        case 'd':
            debug = true;
            break;
        case 's':
            if ((eepromsize = parseEEPsize(optarg, &eeprom_info)) > 0)
                eepromname = optarg;
            break;
        case 'p':
            if (strstr(optarg, "low"))
                speed = CH341_I2C_LOW_SPEED;
            else if (strstr(optarg, "fast"))
                speed = CH341_I2C_FAST_SPEED;
            else if (strstr(optarg, "high"))
                speed = CH341_I2C_HIGH_SPEED;
            else
                speed = CH341_I2C_STANDARD_SPEED;
            break;
        case 'e':
            if (!operation)
                operation = 'e';
            else {
                fprintf(stderr, "Conflicting command line options\n");
                return -1;
            }
            break;
        case 'r':
            if (!operation) {
                operation = 'r';
                filename = optarg;
            } else {
                fprintf(stderr, "Conflicting command line options\n");
                return -1;
            }
            break;
        case 'w':
            if (!operation) {
                operation = 'w';
                filename = optarg;
            } else {
                fprintf(stderr, "Conflicting command line options\n");
                return -1;
            }
            break;
        case 'V':
            if (!operation) {
                operation = 'V';
                filename = optarg;
            } else {
                fprintf(stderr, "Conflicting command line options\n");
                return -1;
            }
            break;
        case 'x':
            if (!operation)
                operation = 'x';
            else {
                fprintf(stderr, "Conflicting command line options\n");
                return -1;
            }
            break;

        default:
        case '?':
            fprintf(stdout, "%s", version_msg);
            fprintf(stderr, "%s", usage_msg);
            return 0;
        }
    }

    debugout = (debug == true) ? stdout : fopen("/dev/null", "w");
    verbout = (verbose == true) ? stdout : fopen("/dev/null", "w");
    fprintf(debugout, "Debug Enabled\n");

    if (!operation) {
        fprintf(stderr, "%s\n%s", version_msg, usage_msg);
        return -1;
    }

    if (eepromsize <= 0 && operation != 'x') {
        fprintf(stderr, "Invalid EEPROM size\n");
        return -1;
    }

    uint8_t *readbuf = (uint8_t *)malloc(eepromsize);      // space to store loaded EEPROM
    if (!readbuf) {
        fprintf(stderr, "Couldnt malloc space needed for EEPROM image\n");
        return -1;
    }

    if (!(devHandle = ch341configure(USB_LOCK_VENDOR, USB_LOCK_PRODUCT))) {
        fprintf(stderr, "Couldnt configure USB device with vendor ID: %04x product ID: %04x\n", USB_LOCK_VENDOR, USB_LOCK_PRODUCT);
        goto free_readbuf;
    }
    fprintf(verbout, "Configured USB device with vendor ID: %04x product ID: %04x\n", USB_LOCK_VENDOR, USB_LOCK_PRODUCT);

    if (ch341setstream(devHandle, speed) < 0) {
        fprintf(stderr, "Couldnt set i2c bus speed\n");
        goto shutdown;
    }
    fprintf(verbout, "Set i2c bus speed to [%d kHz]\n", speed_table[speed]);

    switch (operation) {
    case 'r': // read
        memset(readbuf, 0xff, eepromsize);

        if (ch341readEEPROM(devHandle, readbuf, eepromsize, eeprom_info)
            < 0) {
            fprintf(stderr, "Couldnt read [%d] bytes from [%s] EEPROM\n", eepromsize, eepromname);
            goto shutdown;
        }
        fprintf(stdout, "Read [%d] bytes from [%s] EEPROM\n", eepromsize, eepromname);
        for (i = 0; i < eepromsize; i++) {
            if (!(i % 16))
                fprintf(debugout, "\n%04x: ", i);
            fprintf(debugout, "%02x ", readbuf[i]);
        }
        fprintf(debugout, "\n");

        if (!(fp = fopen(filename, "wb"))) {
            fprintf(stderr, "Couldnt open file [%s] for writing\n", filename);
            goto shutdown;
        }

        fwrite(readbuf, 1, eepromsize, fp);
        if (ferror(fp)) {
            fprintf(stderr, "Error writing file [%s]\n", filename);
            if (fp)
                fclose(fp);
            goto shutdown;
        }
        fclose(fp);
        fprintf(stdout, "Wrote [%d] bytes to file [%s]\n", eepromsize, filename);
        break;
    case 'V': // verify
        memset(readbuf, 0xff, eepromsize);

        if (ch341readEEPROM(devHandle, readbuf, eepromsize, eeprom_info)
            < 0) {
            fprintf(stderr, "Couldnt read [%d] bytes from [%s] EEPROM\n", eepromsize, eepromname);
            goto shutdown;
        }
        fprintf(stdout, "Read [%d] bytes from [%s] EEPROM\n", eepromsize, eepromname);
        for (i = 0; i < eepromsize; i++) {
            if (!(i % 16))
                fprintf(debugout, "\n%04x: ", i);
            fprintf(debugout, "%02x ", readbuf[i]);
        }
        fprintf(debugout, "\n");

        if (!(fp = fopen(filename, "rb"))) {
            fprintf(stderr, "Couldnt open file [%s] for reading\n", filename);
            goto shutdown;
        }

        verifybuf = mmap(NULL, eepromsize, PROT_READ, MAP_PRIVATE, fileno(fp), 0);
        if (!verifybuf) {
            fprintf(stderr, "Error mapping file [%s]\n", filename);
            if (fp)
                fclose(fp);
            goto shutdown;
        }

        for (unsigned i = 0; i < eepromsize; ++i) {
            if (readbuf[i] != verifybuf[i]) {
                verify_failed = true;
                break;
            }
        }
        if (verify_failed)
            fprintf(stdout, "Verification against file [%s] failed at offset [%d], EEPROM: %02hhX, file: %02hhX\n",
                    filename, i, readbuf[i], verifybuf[i]);
        else
            fprintf(stdout, "Verified [%d] bytes against file [%s]\n", eepromsize, filename);

        munmap(verifybuf, eepromsize);
        fclose(fp);
        break;
    case 'w': // write
        if (!(fp = fopen(filename, "rb"))) {
            fprintf(stderr, "Couldnt open file [%s] for reading\n", filename);
            goto shutdown;
        }
        memset(readbuf, 0xff, eepromsize);
        bytesread = fread(readbuf, 1, eepromsize, fp);
        if (ferror(fp)) {
            fprintf(stderr, "Error reading file [%s]\n", filename);
            if (fp)
                fclose(fp);
            goto shutdown;
        }
        fclose(fp);
        fprintf(stdout, "Read [%d] bytes from file [%s]\n", bytesread, filename);

        if (bytesread < eepromsize)
            fprintf(stdout, "Padded to [%d] bytes for [%s] EEPROM\n", eepromsize, eepromname);

        if (bytesread > eepromsize)
            fprintf(stdout, "Truncated to [%d] bytes for [%s] EEPROM\n", eepromsize, eepromname);

        if (ch341writeEEPROM(devHandle, readbuf, eepromsize, eeprom_info) < 0) {
            fprintf(stderr, "Failed to write [%d] bytes from [%s] to [%s] EEPROM\n", eepromsize, filename, eepromname);
            goto shutdown;
        }
        fprintf(stdout, "Wrote [%d] bytes to [%s] EEPROM\n", eepromsize, eepromname);
        break;
    case 'e': // erase
        memset(readbuf, 0xff, eepromsize);
        if (ch341writeEEPROM(devHandle, readbuf, eepromsize, eeprom_info) < 0) {
            fprintf(stderr, "Failed to erase [%d] bytes of [%s] EEPROM\n", eepromsize, eepromname);
            goto shutdown;
        }
        fprintf(stdout, "Erased [%d] bytes of [%s] EEPROM\n", eepromsize, eepromname);
        break;
    case 'x': {// scan
        for (unsigned i = 0x8; i < 0x80; ++i) {
            ch341_quick_write(devHandle, i);
        }
        break;
    }
    default:
        fprintf(stderr, "Unknown option\n");
    }

shutdown:
    libusb_release_interface(devHandle, DEFAULT_INTERFACE);
    fprintf(debugout, "Released device interface [%d]\n", DEFAULT_INTERFACE);
    libusb_close(devHandle);
    fprintf(verbout, "Closed USB device\n");
    libusb_exit(NULL);
free_readbuf:
    free(readbuf);
    return 0;
}
