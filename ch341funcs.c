//
// ch341eeprom programmer version 0.1 (Beta)
//
//  Programming tool for the 24Cxx serial EEPROMs using the Winchiphead CH341A IC
//
// (c) December 2011 asbokid <ballymunboy@gmail.com>
// (c) December 2023 aystarik <aystarik@gmail.com>
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

#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "ch341eeprom.h"

extern FILE *debugout, *verbout;

struct xxx {
  uint8_t ibuf[32];
  uint8_t obuf[32];
} i2c_dev;

// --------------------------------------------------------------------------
// ch341configure()
//      lock USB device for exclusive use
//      claim default interface
//      set default configuration
//      retrieve device descriptor
//      identify device revision
// returns *usb device handle

struct libusb_device_handle *ch341configure(uint16_t vid, uint16_t pid)
{
    int32_t ret = libusb_init(NULL);
    if (ret < 0) {
        fprintf(stderr, "Couldnt initialise libusb\n");
        return NULL;
    }
#if LIBUSBX_API_VERSION < 0x01000106
    libusb_set_debug(NULL, 3);  // maximum debug logging level
#else
    libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, 3);
#endif

    fprintf(verbout, "Searching USB buses for WCH CH341a i2c EEPROM programmer [%04x:%04x]\n", USB_LOCK_VENDOR, USB_LOCK_PRODUCT);

    struct libusb_device_handle *devHandle;
    if (!(devHandle = libusb_open_device_with_vid_pid(NULL, USB_LOCK_VENDOR, USB_LOCK_PRODUCT))) {
        fprintf(stderr, "Couldn't open device [%04x:%04x]\n", USB_LOCK_VENDOR, USB_LOCK_PRODUCT);
        return NULL;
    }

    struct libusb_device *dev;
    if (!(dev = libusb_get_device(devHandle))) {
        fprintf(stderr, "Couldnt get bus number and address of device\n");
        return NULL;
    }

    fprintf(verbout, "Found [%04x:%04x] as device [%d] on USB bus [%d]\n",
            USB_LOCK_VENDOR, USB_LOCK_PRODUCT, libusb_get_device_address(dev), libusb_get_bus_number(dev));

    fprintf(verbout, "Opened device [%04x:%04x]\n", USB_LOCK_VENDOR, USB_LOCK_PRODUCT);

    if (libusb_kernel_driver_active(devHandle, DEFAULT_INTERFACE)) {
        ret = libusb_detach_kernel_driver(devHandle, DEFAULT_INTERFACE);
        if (ret) {
            fprintf(stderr, "Failed to detach kernel driver: '%s'\n", strerror(-ret));
            return NULL;
        } else
            fprintf(verbout, "Detached kernel driver\n");
    }

    int32_t currentConfig = 0;
    ret = libusb_get_configuration(devHandle, &currentConfig);
    if (ret) {
        fprintf(stderr, "Failed to get current device configuration: '%s'\n", strerror(-ret));
        return NULL;
    }

    if (currentConfig != DEFAULT_CONFIGURATION)
        ret = libusb_set_configuration(devHandle, currentConfig);

    if (ret) {
        fprintf(stderr, "Failed to set device configuration to %d: '%s'\n", DEFAULT_CONFIGURATION, strerror(-ret));
        return NULL;
    }

    ret = libusb_claim_interface(devHandle, DEFAULT_INTERFACE); // interface 0

    if (ret) {
        fprintf(stderr, "Failed to claim interface %d: '%s'\n", DEFAULT_INTERFACE, strerror(-ret));
        return NULL;
    }

    fprintf(verbout, "Claimed device interface [%d]\n", DEFAULT_INTERFACE);

    uint8_t ch341DescriptorBuffer[0x12];
    ret = libusb_get_descriptor(devHandle, LIBUSB_DT_DEVICE, 0x00, ch341DescriptorBuffer, 0x12);

    if (ret < 0) {
        fprintf(stderr, "Failed to get device descriptor: '%s'\n", strerror(-ret));
        return NULL;
    }

    fprintf(verbout, "Device reported its revision [%d.%02d]\n", ch341DescriptorBuffer[12], ch341DescriptorBuffer[13]);

    for (unsigned i = 0; i < 0x12; ++i)
        fprintf(debugout, "%02x ", ch341DescriptorBuffer[i]);
    fprintf(debugout, "\n");

    return devHandle;
}

// --------------------------------------------------------------------------
//  ch341setstream()
//      set the i2c bus speed (speed: 0 = 20kHz; 1 = 100kHz, 2 = 400kHz, 3 = 750kHz)
int32_t ch341setstream(struct libusb_device_handle *devHandle, uint32_t speed)
{
    int32_t actuallen = 0;

    i2c_dev.obuf[0] = mCH341A_CMD_I2C_STREAM;
    i2c_dev.obuf[1] = mCH341A_CMD_I2C_STM_SET | (speed & 0x3);

    int ret = libusb_bulk_transfer(devHandle, BULK_WRITE_ENDPOINT, i2c_dev.obuf, 2, &actuallen, DEFAULT_TIMEOUT);

    if (ret < 0) {
        fprintf(stderr, "ch341setstream(): Failed write %d bytes '%s'\n", 2, strerror(-ret));
        return -1;
    }

    fprintf(debugout, "ch341setstream(): Wrote 2 bytes: ");
    for (unsigned i = 0; i < 2; ++i)
        fprintf(debugout, "%02x ", i2c_dev.obuf[i]);
    fprintf(debugout, "\n");
    return 0;
}

int ch341_i2c_read(struct libusb_device_handle *devHandle, struct i2c_msg *msg)
{
    unsigned byteoffset = 0;
    while (msg->len - byteoffset > 0) {
        unsigned bytestoread = msg->len - byteoffset;
        if (bytestoread > 31) // reserve first byte for status
            bytestoread = 31;
        uint8_t *ptr = i2c_dev.obuf;
        *ptr++ = mCH341A_CMD_I2C_STREAM;
        *ptr++ = mCH341A_CMD_I2C_STM_STA;
        *ptr++ = mCH341A_CMD_I2C_STM_OUT; // ask for status
        *ptr++ = (msg->addr << 1) | 1;
        if (bytestoread > 1) {
            *ptr++ = mCH341A_CMD_I2C_STM_IN | (bytestoread - 1);
        }
        *ptr++ = mCH341A_CMD_I2C_STM_IN;
        *ptr++ = mCH341A_CMD_I2C_STM_STO;

        int actuallen = 0;
        int ret = libusb_bulk_transfer(devHandle, BULK_WRITE_ENDPOINT, i2c_dev.obuf, ptr - i2c_dev.obuf, &actuallen, DEFAULT_TIMEOUT);
        if (ret < 0) {
            fprintf(stderr, "USB write error : %s\n", strerror(-ret));
            return ret;
        }
        ret = libusb_bulk_transfer(devHandle, BULK_READ_ENDPOINT, i2c_dev.ibuf, 32, &actuallen, DEFAULT_TIMEOUT);
        if (ret < 0) {
            fprintf(stderr, "USB read error : %s\n", strerror(-ret));
            return ret;
        }
        if (actuallen != bytestoread + 1) {
            fprintf(stderr, "actuallen != bytestoread\b");
            return -1;
        }
        if (i2c_dev.ibuf[0] & 0x80) {
            return -1;
        }
        memcpy(&msg->buf[byteoffset], &i2c_dev.ibuf[1], bytestoread);
        byteoffset += bytestoread;
    }
    return 0;
}

int ch341_i2c_write(struct libusb_device_handle *devHandle, struct i2c_msg *msg) {
    unsigned left = msg->len;
    uint8_t *ptr = msg->buf;
    bool first = true;
    do {
        uint8_t *outptr = i2c_dev.obuf;
        *outptr++ = mCH341A_CMD_I2C_STREAM;
        if (first) { // Start packet
            *outptr++ = mCH341A_CMD_I2C_STM_STA;
        }
        uint8_t *lenptr = outptr++;
        if (first) {
            *outptr++ = msg->addr << 1;
        }
        unsigned avail = 32 - (outptr - i2c_dev.obuf);
        unsigned wlen = avail;
        if (left < avail) {
            wlen = left;
        } else if (left == avail) {
            wlen = avail - 1;
        }
        memcpy(outptr, ptr, wlen);
        outptr += wlen;
        ptr += wlen;
        left -= wlen;
        wlen = outptr - lenptr - 1;
        *lenptr = mCH341A_CMD_I2C_STM_OUT | wlen;
        if (left == 0) {  // Stop packet
            *outptr++ = mCH341A_CMD_I2C_STM_STO;
        }

        wlen = outptr - i2c_dev.obuf;
        assert(wlen <= 32);
        for (unsigned i = 0; i < wlen; ++i) {
            if (!(i % 0x10))
                fprintf(debugout, "\n%02x : ", i);
            fprintf(debugout, "%02x ", i2c_dev.obuf[i]);
        }
        fprintf(debugout, "\n");

        first = false;
        int actuallen = 0, ret = 0;
        ret = libusb_bulk_transfer(devHandle, BULK_WRITE_ENDPOINT, i2c_dev.obuf, wlen, &actuallen, DEFAULT_TIMEOUT);

        if (ret < 0) {
            fprintf(stderr, "Failed to write to I2C: '%s'\n", strerror(-ret));
            return -1;
        }
    } while (left);

    return 0;
}

int ch341_i2c_xfer(struct libusb_device_handle *devHandle, struct i2c_msg *msg, unsigned num) {
    for (unsigned i = 0; i < num; ++i) {
        if (msg[i].flags & I2C_M_RD) {
            int ret = ch341_i2c_read(devHandle, &msg[i]);
            if (ret) return ret;
        } else {
            int ret = ch341_i2c_write(devHandle, &msg[i]);
            if (ret) return ret;
        }
    }
    return 0;
}

// --------------------------------------------------------------------------
// ch341readEEPROM()
//      read n bytes from device (in packets of 32 bytes)
int ch341readEEPROM(struct libusb_device_handle *devHandle, uint8_t *buffer, uint32_t bytestoread, const struct EEPROM *eeprom_info)
{
    struct i2c_msg msg[2];

    uint8_t out[2] = {0};
    msg[0].len = eeprom_info->addr_size;
    msg[0].flags = 0;
    msg[0].addr = EEPROM_I2C_BUS_ADDRESS;
    msg[0].buf = out;

    msg[1].flags = I2C_M_RD;
    msg[1].buf = buffer;
    msg[1].addr = EEPROM_I2C_BUS_ADDRESS;
    msg[1].len = bytestoread;

    return ch341_i2c_xfer(devHandle, msg, 2);
}

int ch341delay_ms(struct libusb_device_handle *devHandle, unsigned ms) {
    i2c_dev.obuf[0] = mCH341A_CMD_I2C_STREAM;
    i2c_dev.obuf[1] = mCH341A_CMD_I2C_STM_MS | (ms & 0xf);        // Wait up to 15ms
    int actuallen = 0;
    return libusb_bulk_transfer(devHandle, BULK_WRITE_ENDPOINT, i2c_dev.obuf, 2, &actuallen, DEFAULT_TIMEOUT);
}

int ch341_quick_write(struct libusb_device_handle *devHandle, uint8_t addr) {
    uint8_t * ptr = i2c_dev.obuf;
    *ptr++ = mCH341A_CMD_I2C_STREAM;
    *ptr++ = mCH341A_CMD_I2C_STM_STA;
    *ptr++ = mCH341A_CMD_I2C_STM_OUT;
#if 1
    *ptr++ = addr << 1 | 1; // READ1
    *ptr++ = mCH341A_CMD_I2C_STM_IN;
#else
    *ptr++ = addr << 1; // WRITE0
#endif
    *ptr++ = mCH341A_CMD_I2C_STM_STO;
    int actuallen = 0;
    int ret = libusb_bulk_transfer(devHandle, BULK_WRITE_ENDPOINT, i2c_dev.obuf, ptr - i2c_dev.obuf, &actuallen, DEFAULT_TIMEOUT);
    if (ret < 0) {
        fprintf(stderr, "USB write error : %s\n", strerror(-ret));
        return ret;
    }
    ret = libusb_bulk_transfer(devHandle, BULK_READ_ENDPOINT, i2c_dev.ibuf, 32, &actuallen, DEFAULT_TIMEOUT);
    if (ret < 0) {
        fprintf(stderr, "USB read error : %s\n", strerror(-ret));
        return ret;
    }
    for (unsigned i = 0; i < actuallen; ++i) {
        fprintf(debugout, "%02x ", i2c_dev.ibuf[i]);
    }
    fprintf(debugout, "0x%02x: %s\n", addr, (i2c_dev.ibuf[0] & 0x80) ? "NACK" : "ACK");
    return 0;
}
// --------------------------------------------------------------------------
// ch341writeEEPROM()
//      write n bytes to 24c32/24c64 device (in packets of 32 bytes)
int32_t ch341writeEEPROM(struct libusb_device_handle *devHandle, uint8_t *buffer, uint32_t bytesum, const struct EEPROM *eeprom_info)
{
    uint8_t msgbuffer[256 + 2]; // max EEPROM page size is 256 in 2M part, and 2 bytes for address
    struct i2c_msg msg;
    msg.addr = EEPROM_I2C_BUS_ADDRESS;
    msg.buf = msgbuffer;
    msg.flags = 0;
    unsigned offset = 0;
    while (offset < bytesum) {
        uint8_t *outptr = msgbuffer;
        unsigned wlen = eeprom_info->page_size;
        if (eeprom_info->addr_size > 1) {
            *outptr++ = offset >> 8;
        }
        *outptr++ = offset;
        if (bytesum - offset < wlen) {
            wlen = bytesum - offset;
        }
        memcpy(outptr, buffer + offset, wlen);
        outptr += wlen;
        msg.len = outptr - msgbuffer;
        int ret = 0;
        ret = ch341_i2c_xfer(devHandle, &msg, 1);
        if (ret < 0) {
            fprintf(stderr, "Failed to write to EEPROM: '%s'\n", strerror(-ret));
            return -1;
        }
        offset += wlen;

        ret = ch341delay_ms(devHandle, 10);
        if (ret < 0) {
            fprintf(stderr, "Failed to set timeout: '%s'\n", strerror(-ret));
            return -1;
        }
        fprintf(stdout, "Written %d%% [%d] of [%d] bytes\r", 100 * offset / bytesum, offset, bytesum);
    }
    return 0;
}

// --------------------------------------------------------------------------
// parseEEPsize()
//   passed an EEPROM name (case-sensitive), returns its byte size
int32_t parseEEPsize(char *eepromname, const struct EEPROM **eeprom)
{
    for (unsigned i = 0; eepromlist[i].size; ++i)
        if (strcmp(eepromlist[i].name, eepromname) == 0) {
            *eeprom = &eepromlist[i];
            return (eepromlist[i].size);
        }
    return -1;
}
