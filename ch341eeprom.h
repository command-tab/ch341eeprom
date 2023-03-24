// libUSB driver for the ch341a in i2c mode
//
// Copyright 2011 asbokid <ballymunboy@gmail.com>

#define CH341TOOLVERSION            "0.5"

#define USB_LOCK_VENDOR             0x1a86      // Dev : (1a86) QinHeng Electronics
#define USB_LOCK_PRODUCT            0x5512      //       (5512) CH341A in i2c mode

#define MAX_EEPROM_SIZE             131072      /* For 24c1024 */

#define EEPROM_I2C_BUS_ADDRESS      0x50

#define BULK_WRITE_ENDPOINT         0x02        /* bEndpointAddress 0x02  EP 2 OUT (Bulk) */
#define BULK_READ_ENDPOINT          0x82        /* bEndpointAddress 0x82  EP 2 IN  (Bulk) */
#define DEFAULT_INTERFACE           0x00

#define DEFAULT_CONFIGURATION       0x01
#define DEFAULT_TIMEOUT             300 // 300mS for USB timeouts

#define IN_BUF_SZ                   0x100
#define EEPROM_READ_BULKIN_BUF_SZ   0x20
#define EEPROM_READ_BULKOUT_BUF_SZ  0x100

/* Based on (closed-source) DLL V1.9 for USB by WinChipHead (c) 2005.
   Supports USB chips: CH341, CH341A
   This can be a problem for copyright, sure asbokid can't release this part on any GPL licence*/

#define	mCH341_PACKET_LENGTH		32      /* wMaxPacketSize 0x0020  1x 32 bytes, unused on the source */
#define	mCH341_PKT_LEN_SHORT		8       /* wMaxPacketSize 0x0008  1x 8 bytes, unused on the source */

#define	mCH341_ENDP_INTER_UP		0x81    /* bEndpointAddress 0x81  EP 1 IN (Interrupt), unused on the source */
#define	mCH341_ENDP_INTER_DOWN		0x01    /* This endpoint isn't list on my lsusb -v output, unused on the source */
#define	mCH341_ENDP_DATA_UP		0x82    /* ==BULK_READ_ENDPOINT  Why repeat it? */
#define	mCH341_ENDP_DATA_DOWN		0x02    /* ==BULK_WRITE_ENDPOINT Why repeat it? */

#define	mCH341_VENDOR_READ		0xC0    /* Unused on the source */
#define	mCH341_VENDOR_WRITE		0x40    /* Unused on the source */

#define	mCH341_PARA_INIT		0xB1    /* Unused on the source */
#define	mCH341_I2C_STATUS		0x52    /* Unused on the source */
#define	mCH341_I2C_COMMAND		0x53    /* Unused on the source */

#define	mCH341_PARA_CMD_R0		0xAC    /* Unused on the source */
#define	mCH341_PARA_CMD_R1		0xAD    /* Unused on the source */
#define	mCH341_PARA_CMD_W0		0xA6    /* Unused on the source */
#define	mCH341_PARA_CMD_W1		0xA7    /* Unused on the source */
#define	mCH341_PARA_CMD_STS		0xA0    /* Unused on the source */

#define	mCH341A_CMD_SET_OUTPUT		0xA1    /* Unused on the source */
#define	mCH341A_CMD_IO_ADDR		0xA2    /* Unused on the source */
#define	mCH341A_CMD_PRINT_OUT		0xA3    /* Unused on the source */
#define	mCH341A_CMD_SPI_STREAM		0xA8    /* Unused on the source */
#define	mCH341A_CMD_SIO_STREAM		0xA9    /* Unused on the source */
#define	mCH341A_CMD_I2C_STREAM		0xAA
#define	mCH341A_CMD_UIO_STREAM		0xAB    /* Unused on the source */

#define	mCH341A_BUF_CLEAR		0xB2    /* Unused on the source */
#define	mCH341A_I2C_CMD_X		0x54    /* Unused on the source */
#define	mCH341A_DELAY_MS		0x5E    /* Unused on the source */
#define	mCH341A_GET_VER			0x5F    /* Unused on the source */

#define	mCH341_EPP_IO_MAX		( mCH341_PACKET_LENGTH - 1 )    /* Unused on the source */
#define	mCH341A_EPP_IO_MAX		0xFF    /* Unused on the source */

#define	mCH341A_CMD_IO_ADDR_W		0x00    /* Unused on the source */
#define	mCH341A_CMD_IO_ADDR_R		0x80    /* Unused on the source */

#define	mCH341A_CMD_I2C_STM_STA		0x74
#define	mCH341A_CMD_I2C_STM_STO		0x75
#define	mCH341A_CMD_I2C_STM_OUT		0x80
#define	mCH341A_CMD_I2C_STM_IN		0xC0
#define	mCH341A_CMD_I2C_STM_MAX		( min( 0x3F, mCH341_PACKET_LENGTH ) )   /* Unused on the source */
#define	mCH341A_CMD_I2C_STM_SET		0x60
#define	mCH341A_CMD_I2C_STM_US		0x40    /* Unused on the source */
#define	mCH341A_CMD_I2C_STM_MS		0x50    /* Unused on the source */
#define	mCH341A_CMD_I2C_STM_DLY		0x0F    /* Unused on the source */
#define	mCH341A_CMD_I2C_STM_END		0x00

#define	mCH341A_CMD_UIO_STM_IN		0x00    /* Unused on the source */
#define	mCH341A_CMD_UIO_STM_DIR		0x40    /* Unused on the source */
#define	mCH341A_CMD_UIO_STM_OUT		0x80    /* Unused on the source */
#define	mCH341A_CMD_UIO_STM_US		0xC0    /* Unused on the source */
#define	mCH341A_CMD_UIO_STM_END		0x20    /* Unused on the source */

#define	mCH341_PARA_MODE_EPP		0x00    /* Unused on the source */
#define	mCH341_PARA_MODE_EPP17		0x00    /* Unused on the source */
#define	mCH341_PARA_MODE_EPP19		0x01    /* Unused on the source */
#define	mCH341_PARA_MODE_MEM		0x02    /* Unused on the source */

/* End of part based on (closed-source) DLL V1.9 for USB by WinChipHead (c) 2005.
   Since is largely unused we can replace it*/

#define CH341_I2C_LOW_SPEED 0   // low speed - 20kHz
#define CH341_I2C_STANDARD_SPEED 1      // standard speed - 100kHz
#define CH341_I2C_FAST_SPEED 2  // fast speed - 400kHz
#define CH341_I2C_HIGH_SPEED 3  // high speed - 750kHz

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define TRUE    1
#define FALSE   0

struct EEPROM {
    char *name;
    uint32_t size;
    uint16_t page_size;
    uint8_t addr_size;          // Length of addres in bytes
    uint8_t i2c_addr_mask;
};

const static struct EEPROM eepromlist[] = {
    { "24c01", 128, 8, 1, 0 },  // 16 pages of 8 bytes each = 128 bytes
    { "24c02", 256, 8, 1, 0 },  // 32 pages of 8 bytes each = 256 bytes
    { "24c04", 512, 16, 1, 1 }, // 32 pages of 16 bytes each = 512 bytes
    { "24c08", 1024, 16, 1, 3 },        // 64 pages of 16 bytes each = 1024 bytes
    { "24c16", 2048, 16, 1, 7 },        // 128 pages of 16 bytes each = 2048 bytes
    { "24c32", 4096, 32, 2, 0 },        // 32kbit = 4kbyte
    { "24c64", 8192, 32, 2, 0 },
    { "24c128", 16384, 64, 2, 0 },
    { "24c256", 32768, 64 , 2, 0 },
    { "24c512", 65536, 128, 2, 0 },
    { "24c1024", 131072, 128, 2, 1 },
    { 0, 0, 0, 0 }
};

extern uint8_t *readbuf;

int32_t ch341readEEPROM(struct libusb_device_handle *devHandle, uint8_t * buf, uint32_t bytes, const struct EEPROM *eeprom_info);
int32_t ch341writeEEPROM(struct libusb_device_handle *devHandle, uint8_t * buf, uint32_t bytes, const struct EEPROM *eeprom_info);
struct libusb_device_handle *ch341configure(uint16_t vid, uint16_t pid);
int32_t ch341setstream(struct libusb_device_handle *devHandle, uint32_t speed);
int32_t parseEEPsize(char *eepromname, const struct EEPROM **eeprom);

// callback functions for async USB transfers
void cbBulkIn(struct libusb_transfer *transfer);
void cbBulkOut(struct libusb_transfer *transfer);
