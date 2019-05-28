ARDUINO_QUIET = 1

# base configuration
BOARD_TAG    = nano#328
BOARD_SUB    = atmega328
ARDUINO_LIBS = FastLED LowPower SPI TimerOne EEPROM RF24 DigitalIO

# defaults
PORT   = 0

MONITOR_PORT = /dev/ttyUSB$(PORT)
OBJDIR       = build-$(BOARD_TAG)

CXXFLAGS_STD = -fno-threadsafe-statics
include /usr/share/arduino/Arduino.mk
