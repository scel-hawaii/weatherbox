# A makefile

FILENAME='GoBlink'
MCU='atmega328p'
program: build upload

build:
	avr-gcc -mmcu=$(MCU) $(FILENAME).c -o $(FILENAME).out
	avr-objcopy -j .text -j .data -O ihex $(FILENAME).out $(FILENAME).hex
upload:
	avrdude -c avrispmkii -p $(MCU) -P usb -U flash:w:$(FILENAME).hex
