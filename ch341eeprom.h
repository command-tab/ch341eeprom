// libUSB driver for the ch341a in i2c mode
//
// Copyright 2011 asbokid <ballymunboy@gmail.com>

#define CH341TOOLVERSION            "0.5"

#define USB_LOCK_VENDOR             0x1a86 // Dev : (1a86) QinHeng Electronics
#define USB_LOCK_PRODUCT            0x5512 //       (5512) CH341A in i2c mode


#define MAX_EEPROM_SIZE             131072 /* For 24c1024*/

#define BULK_WRITE_ENDPOINT         0x02
#define BULK_READ_ENDPOINT          0x82
#define DEFAULT_INTERFACE           0x00

#define DEFAULT_CONFIGURATION       0x01
#define DEFAULT_TIMEOUT             300    // 300mS for USB timeouts

#define IN_BUF_SZ                   0x100
#define EEPROM_WRITE_BUF_SZ         0x2b   // only for 24c64 / 24c32 ??
#define EEPROM_READ_BULKIN_BUF_SZ   0x20
#define EEPROM_READ_BULKOUT_BUF_SZ  0x65

// Based on (closed-source) DLL V1.9 for USB by WinChipHead (c) 2005.
// Supports USB chips: CH341, CH341A

#define	mCH341_PACKET_LENGTH		32
#define	mCH341_PKT_LEN_SHORT		8

#define	mCH341_ENDP_INTER_UP		0x81
#define	mCH341_ENDP_INTER_DOWN		0x01
#define	mCH341_ENDP_DATA_UP		0x82
#define	mCH341_ENDP_DATA_DOWN		0x02

#define	mCH341_VENDOR_READ		0xC0
#define	mCH341_VENDOR_WRITE		0x40

#define	mCH341_PARA_INIT		0xB1
#define	mCH341_I2C_STATUS		0x52
#define	mCH341_I2C_COMMAND		0x53

#define	mCH341_PARA_CMD_R0		0xAC
#define	mCH341_PARA_CMD_R1		0xAD
#define	mCH341_PARA_CMD_W0		0xA6
#define	mCH341_PARA_CMD_W1		0xA7
#define	mCH341_PARA_CMD_STS		0xA0

#define	mCH341A_CMD_SET_OUTPUT		0xA1
#define	mCH341A_CMD_IO_ADDR		0xA2
#define	mCH341A_CMD_PRINT_OUT		0xA3
#define	mCH341A_CMD_SPI_STREAM		0xA8
#define	mCH341A_CMD_SIO_STREAM		0xA9
#define	mCH341A_CMD_I2C_STREAM		0xAA
#define	mCH341A_CMD_UIO_STREAM		0xAB

#define	mCH341A_BUF_CLEAR		0xB2
#define	mCH341A_I2C_CMD_X		0x54
#define	mCH341A_DELAY_MS		0x5E
#define	mCH341A_GET_VER			0x5F

#define	mCH341_EPP_IO_MAX		( mCH341_PACKET_LENGTH - 1 )
#define	mCH341A_EPP_IO_MAX		0xFF

#define	mCH341A_CMD_IO_ADDR_W		0x00
#define	mCH341A_CMD_IO_ADDR_R		0x80

#define	mCH341A_CMD_I2C_STM_STA		0x74
#define	mCH341A_CMD_I2C_STM_STO		0x75
#define	mCH341A_CMD_I2C_STM_OUT		0x80
#define	mCH341A_CMD_I2C_STM_IN		0xC0
#define	mCH341A_CMD_I2C_STM_MAX		( min( 0x3F, mCH341_PACKET_LENGTH ) )
#define	mCH341A_CMD_I2C_STM_SET		0x60
#define	mCH341A_CMD_I2C_STM_US		0x40
#define	mCH341A_CMD_I2C_STM_MS		0x50
#define	mCH341A_CMD_I2C_STM_DLY		0x0F
#define	mCH341A_CMD_I2C_STM_END		0x00

#define	mCH341A_CMD_UIO_STM_IN		0x00
#define	mCH341A_CMD_UIO_STM_DIR		0x40
#define	mCH341A_CMD_UIO_STM_OUT		0x80
#define	mCH341A_CMD_UIO_STM_US		0xC0
#define	mCH341A_CMD_UIO_STM_END		0x20

#define	mCH341_PARA_MODE_EPP		0x00
#define	mCH341_PARA_MODE_EPP17		0x00
#define	mCH341_PARA_MODE_EPP19		0x01
#define	mCH341_PARA_MODE_MEM		0x02


#define CH341_I2C_LOW_SPEED 0               // low speed - 20kHz               
#define CH341_I2C_STANDARD_SPEED 1          // standard speed - 100kHz
#define CH341_I2C_FAST_SPEED 2              // fast speed - 400kHz
#define CH341_I2C_HIGH_SPEED 3              // high speed - 750kHz

// CH341a READ EEPROM setup packet for the 24c64
//   this needs putting into a struct to allow convenient access to individual elements

#define CH341_EEPROM_READ_SETUP_CMD "\xaa\x74\x83\xa0\x00\x00\x74\x81\xa1\xe0\x00\x00\x06\x04\x00\x00" \
                                    "\x00\x00\x00\x00\x40\x00\x00\x00\x11\x4d\x40\x77\xcd\xab\xba\xdc" \
                                    "\xaa\xe0\x00\x00\xc4\xf1\x12\x00\x11\x4d\x40\x77\xf0\xf1\x12\x00" \
                                    "\xd9\x8b\x41\x7e\x00\xf0\xfd\x7f\xf0\xf1\x12\x00\x5a\x88\x41\x7e" \
                                    "\xaa\xe0\x00\x00\x2a\x88\x41\x7e\x06\x04\x00\x00\x11\x4d\x40\x77" \
                                    "\xe8\xf3\x12\x00\x14\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00" \
                                    "\xaa\xdf\xc0\x75\x00" 

// for 24c64
#define CH341_EEPROM_READ_NEXT_CMD  "\xaa\x74\x83\xa0\x00\x00\x74\x81\xa1\xe0\x00\x00\x10\x00\x00\x00" \
                                    "\x00\x00\x00\x00\x8c\xf1\x12\x00\x01\x00\x00\x00\x00\x00\x00\x00" \
                                    "\xaa\xe0\x00\x00\x4c\xf1\x12\x00\x5d\x22\xd7\x5a\xdc\xf1\x12\x00" \
                                    "\x8f\x04\x44\x7e\x30\x88\x41\x7e\xff\xff\xff\xff\x2a\x88\x41\x7e" \
                                    "\xaa\xe0\x00\x7e\x00\x00\x00\x00\x69\x0e\x3c\x00\x12\x01\x19\x00" \
                                    "\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x9c\x2e\x68\x00" \
                                    "\xaa\xdf\xc0\x75\x00"
#define CH341_EEPROM_READ_CMD_SZ 0x65

// for 24c02
#define CH341_EEPROM1_READ_SETUP_CMD "\xaa\x74\x82\xa0\x00\x74\x81\xa1" \
                                     "\xe0\x00\x12\x00\xc3\xef\x3d\x77" \
                                     "\x2a\xf0\x3d\x77\x28\x00\x00\x00" \
                                     "\x41\x7c\x43\x00\x01\x00\x00\x00" \
                                     "\xaa\xe0\x00\x00\x65\x41\x40\x00" \
                                     "\x00\x00\x00\x00\x00\x00\x00\x00" \
                                     "\x80\x07\x00\x00\x28\x00\x00\x00" \
                                     "\x01\x00\x00\x00\x01\x00\x00\x00" \
                                     "\xaa\xe0\x00\x00\x00\x00\x00\x00" \
                                     "\x01\x00\x00\x00\x01\x00\x00\x00" \
                                     "\xee\x00\x01\x00\x00\x00\x00\x00" \
                                     "\x53\x00\x01\x01\x00\x00\x00\x00" \
                                     "\xaa\xdf\xc0\x75\x00"

#define CH341_EEPROM1_READ_NEXT_CMD "\xaa\x74\x82\xa0\x00\x74\x81\xa1" \
                                     "\xe0\x00\x12\x00\xc3\xef\x3d\x77" \
                                     "\x2a\xf0\x3d\x77\x28\x00\x00\x00" \
                                     "\x41\x7c\x43\x00\x01\x00\x00\x00" \
                                     "\xaa\xe0\x00\x00\x65\x41\x40\x00" \
                                     "\x00\x00\x00\x00\x00\x00\x00\x00" \
                                     "\x80\x07\x00\x00\x28\x00\x00\x00" \
                                     "\x01\x00\x00\x00\x01\x00\x00\x00" \
                                     "\xaa\xe0\x00\x00\x00\x00\x00\x00" \
                                     "\x01\x00\x00\x00\x01\x00\x00\x00" \
                                     "\xee\x00\x01\x00\x00\x00\x00\x00" \
                                     "\x53\x00\x01\x01\x00\x00\x00\x00" \
                                     "\xaa\xdf\xc0\x75\x00"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define TRUE    1
#define FALSE   0

struct EEPROM {
    char *name;
    uint32_t size;
    uint16_t page_size;
    uint8_t addr_size; // Length of addres in bytes
    uint8_t i2c_addr_mask;
};

const static struct EEPROM eepromlist[] = {
  { "24c01",   128,     8,  1, 0x00}, // 16 pages of 8 bytes each = 128 bytes
  { "24c02",   256,     8,  1, 0x00}, // 32 pages of 8 bytes each = 256 bytes
  { "24c04",   512,    16,  1, 0x01}, // 32 pages of 16 bytes each = 512 bytes
  { "24c08",   1024,   16,  1, 0x03}, // 64 pages of 16 bytes each = 1024 bytes
  { "24c16",   2048,   16,  1, 0x07}, // 128 pages of 16 bytes each = 2048 bytes
  { "24c32",   4096,   32,  2, 0x00}, // 32kbit = 4kbyte
  { "24c64",   8192,   32,  2, 0x00},
  { "24c128",  16384,  32/*64*/,  2, 0x00},
  { "24c256",  32768,  32/*64*/,  2, 0x00},
  { "24c512",  65536,  32/*128*/, 2, 0x00},
  { "24c1024", 131072, 32/*128*/, 2, 0x01},
  { 0, 0, 0, 0 }
};


int32_t ch341readEEPROM(struct libusb_device_handle *devHandle, uint8_t *buf, uint32_t bytes, struct EEPROM* eeprom_info);
int32_t ch341writeEEPROM(struct libusb_device_handle *devHandle, uint8_t *buf, uint32_t bytes, struct EEPROM* eeprom_info);
struct libusb_device_handle *ch341configure(uint16_t vid, uint16_t pid);
int32_t ch341setstream(struct libusb_device_handle *devHandle, uint32_t speed);
int32_t parseEEPsize(char* eepromname, struct EEPROM *eeprom);

// callback functions for async USB transfers
void cbBulkIn(struct libusb_transfer *transfer);
void cbBulkOut(struct libusb_transfer *transfer);
