# SmarTo MQTT
Arduino project to check the toilet availability.

It publish a message, over MQTT protocol, when the light is on and also when the motion sensor is activate.
When the light turn off sends a message and go into deep sleep.
It will come out from this state only when the light goes back on.

For more info about the main project visit [SmarTo](https://github.com/MFava91/SmarTo).

## Hardware
- ESP32 - Tested with: [Wemos wifi & bluetooth battery](https://www.banggood.com/it/WeMos-WiFi-Bluetooth-Battery-ESP32-Development-Tool-p-1164436.html)
- Photosensitive resistance sensor module
- HC-SR501 Pir Motion Detector - [Doc](https://www.mpja.com/download/31227sc.pdf)
- Jumper Wires x 6