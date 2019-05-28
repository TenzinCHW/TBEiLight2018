# TBEiLight2018
Control system for The Butterfly Effect. This project uses Arduino Nano with nRF24L01 module.

This used to use Raspberry Pi 3 for the configuration of the lamp and drum locations.
However, since the Raspberry Pi 3 broke inside the drum due to rain or some impact,
We transferred all the functionality to the code in Drums/MasterDrum.

## Libraries required:
- FastLED
- DigitalIO
- TimerOne
- LowPower
- SPI
- EEPROM
- RF24

## Library edits
As there was a mistake in how the PCB (printed circuit board) was designed, we also need to change the `RF24_config.h` file.
1. Uncomment the line `#define SOFTSPI`
2. Find the lines defining the following variables and define them as follows:
```
#define SOFT_SPI_MOSI_PIN 10
#define SOFT_SPI_MISO_PIN 11
#define SOFT_SPI_SCK_PIN 12
```

If the PCB design is rectified, ignore this library edits section.
