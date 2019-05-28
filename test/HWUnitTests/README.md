# Hardware unit testing
The files in these two folders are for testing that all components are working and that the circuit is wired correctly.

Upload the code in `Transmit` to a single Arduino Nano equipped with an nRF24 (testing device) and the code in `Receive` to the lamp module being tested. Turn the lamp module on and open the serial monitor for the testing device.

If the transceiver device on the lamp module is working, the serial monitor will display a bunch of lines of numbers, the last of which should be increasing by 1 for each line. If the LEDs are working, they should flash white.
This code cannot test the charging circuit nor the solar panel.

