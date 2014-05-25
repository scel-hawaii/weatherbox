BOARD_TAG    = uno
MONITOR_PORT  = /dev/ttyACM2
ARDUINO_LIBS = SCEL_MillisOverflow XBee Adafruit_MCP4725 Adafruit_INA219 Adafruit_BMP085 \
				DallasTemperature OneWire SHT1x Wire SoftwareSerial EEPROM
FILENAME=$(notdir $(shell pwd))
MCU='atmega328p'
MONITOR_PORT = /dev/ttyUSB1


include Arduino.mk

isp-upload:
	ln -fs build-uno/$(FILENAME).hex . 
	avrdude -c avrispmkii -p $(MCU) -P usb -U flash:w:$(FILENAME).hex

isp-build:
	avr-gcc -mmcu=$(MCU) $(FILENAME).c -o $(FILENAME).out
	avr-objcopy -j .text -j .data -O ihex $(FILENAME).out $(FILENAME).hex

isp-reset:
	avrdude -c avrispmkii -p $(MCU) -P usb 
