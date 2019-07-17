# movuino esp32 firmware

this is a port of the esp8266 version of the movuino firmware (cri-motionlab's movuino-firmware repository) for use with the movuina software.

everything is supposed to work the same, except the status led which is not present anymore on the esp32 version of movuino.

### how to flash this firmware

* install the latest version of arduino
* copy the contents of the `libraries` directory into your `Documents/Arduino/libraries` directory
* install esp32 board definitions as explained [here](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md)
* open the `firmware/firmware.ino` file with Arduino
* select the `ESP32 Dev Module` board definition from the `Tools` menu
* select the port that should appear when you plug the movuino via USB from the `Tools` menu.
* hit the `upload` button to upload the firmware to the board