CC = gcc
CFLAGS = -Wall -O2

default:
	$(CC) $(CFLAGS) -o ch341eeprom ch341eeprom.c ch341funcs.c -lusb-1.0
	$(CC) $(CFLAGS) -o mktestimg mktestimg.c

clean:
	rm -f ch341eeprom mktestimg

test01: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=1
	./ch341eeprom -v -s 24c01 -w tmp_random.bin
	./ch341eeprom -v -s 24c01 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 1Kbit/128bytes EEPROM done"

test02: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=2
	./ch341eeprom -v -s 24c02 -w tmp_random.bin
	./ch341eeprom -v -s 24c02 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 2Kbit/256bytes EEPROM done"

test04: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=4
	./ch341eeprom -v -s 24c04 -w tmp_random.bin
	./ch341eeprom -v -s 24c04 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 4Kbit/512bytes EEPROM done"

test08: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=8
	./ch341eeprom -v -s 24c08 -w tmp_random.bin
	./ch341eeprom -v -s 24c08 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 8Kbit/1Kbyte EEPROM done"

test16: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=16
	./ch341eeprom -v -s 24c16 -w tmp_random.bin
	./ch341eeprom -v -s 24c16 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 16Kbit/2Kbyte EEPROM done"

test32: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=32
	./ch341eeprom -v -s 24c32 -w tmp_random.bin
	./ch341eeprom -v -s 24c32 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 32Kbit/4Kbyte EEPROM done"

test64: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=64
	./ch341eeprom -v -s 24c64 -w tmp_random.bin
	./ch341eeprom -v -s 24c64 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 64Kbit/8Kbyte EEPROM done"

test128: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=128
	./ch341eeprom -v -s 24c128 -w tmp_random.bin
	./ch341eeprom -v -s 24c128 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 128Kbit/16Kbyte EEPROM done"

test256: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=256
	./ch341eeprom -v -s 24c256 -w tmp_random.bin
	./ch341eeprom -v -s 24c256 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 256Kbit/32Kbyte EEPROM done"

test512: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=512
	./ch341eeprom -v -s 24c512 -w tmp_random.bin
	./ch341eeprom -v -s 24c512 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 512Kbit/64Kbyte EEPROM done"

test1024: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=1024
	./ch341eeprom -v -s 24c1024 -w tmp_random.bin
	./ch341eeprom -v -s 24c1024 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	@echo "Test 1024Kbit/128Kbyte EEPROM done"
