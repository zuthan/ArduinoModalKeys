ArduinoModalKeys
================

This is a project to implement programmable keyboard behavior in hardware using an Arduino with USB Host Shield. 
The default logic turns the Alt keys into special modifiers that make ASDF and ;LKJ and QWER and POIU 
into sets of (Shift, Alt, Ctrl, Gui/Super/Command/Win) modifiers, each of which put the keyboard in different modes.

## Hardware Prerequisites

* [Arduino Leonardo](http://arduino.cc/en/Main/arduinoBoardLeonardo)
* [USB Host Shield](http://arduino.cc/en/Main/ArduinoUSBHostShield)
* [Micro USB to USB cable](http://www.amazon.com/AmazonBasics-USB-2-0-Micro-Cable/dp/B00C28L5UW)
* USB Keyboard with at least [6KRO](https://en.wikipedia.org/wiki/Rollover_%28key%29)

## Software Prerequisites

* [Arduino IDE](http://arduino.cc/en/main/software)
* [USB Host Shield library](https://github.com/felis/USB_Host_Shield_2.0)
* The contents of this repository (yes, really :P)

## Setup Instructions
* Connect your Arduino Leonardo to your computer via the micro USB cable
* Open modal_keys/modal_keys.ino in the Arduino IDE
* Download the USB Host Shield library into \<Arduino Sketchbook location\>/library. Arduino Sketchbook location is specified in File -> Preferences.
* Restart Arduino IDE
* In the Arduino IDE, select the Board type to be Leonardo: Tools -> Board
* Check that the Arduino Leonardo is connected. If it is, there should be an entry under the Tools -> Serial Port menu. If it isn't, Google is your friend :-)
* Upload the sketch to your device: File -> Upload
* Unplug your Arduino device
* Connect the USB Host Shield to the Arduino Leonardo
* Plug your USB keyboard into the USB port on the USB Host Shield
* Plug the Arduino back into the computer
* Keystrokes typed into this keyboard should now be sent to your computer through the Arduino Leonardo
