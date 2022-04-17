# Wake Word Detection

The wake word detection sample for ESP32 boards. Present TensorFlow model is trained for "Marvin" word and provide quite high accuracy. The `model` subdir contains instructions how to train model for any other word.

## Prepare

Clone TensorFlowLite micro components:
```sh
$ git clone --recurse-submodules https://github.com/espressif/tflite-micro-esp-examples.git $HOME/tflite-micro
$ cd <project-path>
$ mkdir -p components
$ ln -s $HOME/tflite-micro/components/tflite-lib tflite-lib
$ ln -s $HOME/tflite-micro/components/esp-nn esp-nn
```

Clone KissFFT component:
```sh
$ git clone https://github.com/karz0n/esp32-kissfft.git $HOME/kissfft
$ cd <project-path>
$ mkdir -p components
$ ln -s $HOME/kissfft kissfft
```

## Configure

The pinout for INMP441 device is next:

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
