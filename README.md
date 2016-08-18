# HC-05 Arduino Bluetooth
This library is forked from Adafruit's bluetooth LE library, and subclasses the AT Parser class. Unlike
other HC-05 libraries, this one allows you to to issue commands to the module that you would normally have
to handle on your own with AT-Commands. This library abstracts all these AT-Commands from you, and hides them
behind simple methods, such as `bind()`.
