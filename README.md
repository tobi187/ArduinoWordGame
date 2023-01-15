# ArduinoWordGame

A little German Word Game on the Arduino where get different words. The Charracters of these words are not in the right order and your task is to fix this.

## Arduino Setup (In my Case):
- Arduino (Arduino Mega 2560)
- Lcd Display (LCD 2004 I²C)
- Passive Buzzer
- Joystick

## PinSetup

| Arduino | LCD       |
|---------|-----      |
|   5V    | VCC       |
|   GND   | GND       |
|   SDA   | SDA/20    |
|   SCL   | SCL/21    |
| Arduino | Joystick  |
|---------|-----------|
|   GND   |   GND     |
|   5V    |   5V      |
|   A1    |   VRx     |
|   A0    |   VRy     |
| Arduino |  Buzzer   |
|   GND   |     -     |
| d8(PWM) |     +     |
