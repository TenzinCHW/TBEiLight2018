# Source directory for The Butterfly Effect

This directory contains the code for the currently-working setup.

MasterOnly contains the code for an Arduino Nano (Master) equipped with an nRF24 module. This device should only be on when the installation is ready to go live. Its responsibility is to assign the correct lamp locations, drum locations, wave properties such as period, wavelength and lifespan of a wave. To generate new coordinates for lamps, update `generate_config_h.py` with the polar coordinates of each lamp from a drum (labeled `D1`, `D2`, `D3` and `D4`). The format is `(radius in cm, angle in degrees from a fixed reference line going through one of the drums)`. Then run `python3 generate_config_h` and move `config.h` to `MasterOnly`.

`NonMasterDrum` contains the code for generating signals that keep the lamps awake when no one is hitting the drums, as well as to detect the signal from the piezoelectric sensors attached to all drums. This sensor is connected to pin `A5` on the Arduino Nano. This code is to be uploaded to an Arduino Nano in each drum. To reset the whole system, turn off all drums for at least 30 seconds and then turn them and the Master Arduino on.

Finally, to configure the lamps, use the code in `LampControl/WriteLampID` to set the id (starting from 0) of each Arduino to be put into the lamps. The lamp IDs must correspond to this range from 0 to the number of lamps being deployed (in our case 80). This address is written to the first 2 bytes of EEPROM memory. Then, upload the code in `LampControl` to every lamp and the installation is ready for deployment.
