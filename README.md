# MUX_3p_TEST
I2C multiplexer system 
TCA9548A multiplexer with Arduino
This is the first time I have setup a MUX system. And I need your help to identify where I am going wrong! Bear with me.
A non-responsive system, I get as far as a serial output up to the first sensor initiation – which fails.
I have individually and successfully tested all sensory systems are working - separate from the MUX setup.
Attached is a GitHub package of the project scope including electrical schematics, Arduino code and pictures.
I have located the addresses of all the devices using the scanner code successfully. As you can see I have connected 3 peripheral devices – clock, compass and pressure sensor. Along with a SD card reader using SPI communications.
I am using 10k pull up resistors to 5v bus line for both the SDA and SCL channels, with Arduino uno.
