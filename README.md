# ESP32 TFLITE WWD

The wake word detection sample for ESP32 boards.
Presented model is trained for "Marvin" wake word and provide quite high accuracy.
The `model` subdir contains instructions how to train model for custom wake word.

The project involves using ESP32 kind devices (e.g. Espressif ESP32 Dev Module) and INMP441 mems microphone with quite high quality.
Particular pin-out between ESP32 and mic is presented at `Pins` section.

## Dependencies

* Platform.io toolkit
* ESP-IDF (ver. 5.1)
* TensorFlow Lite Micro (git submodule)
* ESP-NN (git submodule)
* KissFFT (git submodule)

Note: The TensorFlow Lite component is responsible for recognizing wake word using trained model and histogram of recorded sound.

Note: The KissFFT component is responsible for making histogram of recorded sound.

## Pins

The pin-out for INMP441 sensor is next:

| INMP441 | ESP32  | Info                          |
|---------|--------|-------------------------------|
| L/R     | GND    | Left channel or right channel |
| WS      | GPIO22 | Left right clock              |
| SCK     | GPIO26 | Serial clock                  |
| SD      | GPIO21 | Serial data                   |
| VDD     | 3v3    | Power - DO NOT USE 5V!        |
| GND     | GND    | GND                           |

Configuration locates at `lib/config/Config.hpp` file.

## Build

* Clone
```shell
$ git clone <url>
$ cd esp32-tflite-wwd
$ git submodule update --init --recursive
```
* Build
```shell
$ pio run -t menuconfig
$ pio run
```
* Upload
```shell
$ pio run -t upload
```
* Connect
```shell
$ pio run -t monitor
```
