default:
	clang -o ch341eeprom ch341eeprom.c ch341funcs.c -lusb-1.0
	clang -o mktestimg mktestimg.c

clean:
	rm -f ch341eeprom mktestimg

test256: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=256
	./ch341eeprom -v -s 256 -w tmp_random.bin
	./ch341eeprom -v -s 256 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	echo "Test 256Kbit EEPROM done"

test1024: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=1024
	./ch341eeprom -v -s 1024 -w tmp_random.bin
	./ch341eeprom -v -s 1024 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	echo "Test 1024Kbit EEPROM done"

