# 2D-Wasserwaage
A simple school project: a digital 2D spirit level built with an Arduino Nano (ATmega328P) in Microchip Studio.

## Features
Displays tilt angle (°) on OLED
LED “bubble” shows direction like a real level
Uses I2Clib (sensor + display)

## Hardware
Arduino Nano (ATmega328P)
OLED display (SH1106 / I2C)
Accelerometer (FXLS8964 sensor)
LEDs + resistors

## How it works
Reads x, y, z values from the accelerometer
Calculates angle using atan2()
Converts to degrees and shows it on the OLED
LEDs indicate tilt direction (like a bubble)

## Usage
Open project in [Microchip Studio](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio#Downloads)
Compile & upload to Arduino Nano (e.g. [xLoader](https://github.com/binaryupdates/xLoader))
Power the device