# Unit tests for the nRF24 transceiver module

The code in this directory is to test the connection of the nRF24 transceiver module.

The `One2One` tests test one-way communication while the `Relay` tests test two-way communication.
To check that the module is wired correctly and working, monitor the serial monitors of both the `RX` and `TX` devices. They should print out packets of digits increasing in order.

## Debugging
To test, at least one working setup is required. If one is not available, test randomly until the test passes - this indicates that both are working. Use one of the working ones to test a setup that is not known to work.

Also ensure that the nRF24 is wired correctly and the soft SPI pins are defined in the `RF24` library as specified in the README of this project's root.
If the soft SPI pins are not required, ignore changing the library and just wire as usual.
