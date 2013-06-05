default:
	clang -o ch341eeprom ch341eeprom.c ch341funcs.c -lusb-1.0
	clang -o mktestimg mktestimg.c

clean:
	rm -f ch341eeprom mktestimg
