# ESP32 TFLITE WWD

The wake word detection sample for ESP32 boards.
Present TensorFlow model is trained for "Marvin" word and provide quite high accuracy.
The `model` subdir contains instructions how to train model for any other word.

The project involde using ESP32 kind devices (e.g. Espressif ESP32 Dev Module) and INMP441 mems microphone with quite high quality. Particular pinout between ESP32 and mic is presented at `Configure` section.

## Dependencies

* ESP-IDF (ver. 4.4.3)
* TensorFlow Lite Micro ([repository](https://github.com/espressif/tflite-micro-esp-examples.git))
* KissFFT ([repository](https://github.com/karz0n/esp32-kissfft.git))

Note: The TensorFlow Lite component is responsible for recognizing wake word using trained model and gistogram of recorded sound.

Note: The KissFFT component is responsible for making gistogram of recorded sound.

## Prepare

* Clone dependencies using helpfull script:
```shell
$ cd <project-path>
$ bash scripts/setup.sh
```

## Configure

The pinout for INMP441 sensor is next:

| INMP441 | ESP32  | Info                          |
| ------- | ------ | ----------------------------- |
| L/R     | GND    | Left channel or right channel |
| WS      | GPIO22 | Left right clock              |
| SCK     | GPIO26 | Serial clock                  |
| SD      | GPIO21 | Serial data                   |
| VDD     | 3v3    | Power - DO NOT USE 5V!        |
| GND     | GND    | GND                           |

Configuration locates at `lib/config/Config.hpp` file.

## Using

Open this project using Visual Studio Code with previously configured PlatformIO environment, build and upload to device as usual.
