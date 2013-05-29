default:
	clang -o ch341eeprom ch341eeprom.c ch341funcs.c -lusb-1.0

clean:
	rm -f ch341eeprom
