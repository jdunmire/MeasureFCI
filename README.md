MeasureFCI
==========

MeasureFCI is an Arudino sketch to measure frequency, capacitance, and
inductance using the [SCIM-shield](https://github.com/jdunmire/SCIM-shield).

This sketch was developed and tested on an [Arduino
UNO](http://arduino.cc/en/Main/arduinoBoardUno). The Arduino IDE was run
on an Ubuntu system.

## Required Hardware
The [lcmeter_nui shield](https://github.com/jdunmire/lcmeter_nui) a combination of an oscillator, switches,
and connectors that make it possible to measure capacitance, inductance,
and frequencies.

## Sketch Features
  - Measures frequency and capacitance or inductance once per second.
  - Detects capacitance and inductance setting of the
    [lcmeter_nui](https://github.com/jdunmire/lcmeter_nui)
  - Measured values are in scalled units: milli(m), micro(u), nano(n),
    pico(p)
  - Output is CSV formated for easy import into other programs or
    spreadsheets.
  - Interactive Calibration mode.
  - Calibration is saved in EEPROM.
  - Serial output at 115200 baud.

## Installation
The
[EEPROMex](https://github.com/thijse/Arduino-Libraries/tree/master/EEPROMEx) library must be installed in the `sketchbook/library/`
directory.

### Installation from [GitHub](https://github.com/jdunmire/MeasureFCI)
Clone [git](http://git-scm.com/)
archive into the `sketchbook/` directory:

    $ cd sketchbook/
    $ git clone https://github.com/jdunmire/MeasureFCI.git

### Installation from zip
Download the ZIP to a temporary location and unzip it into the
`sketchbook/` directory and adjust the directory name:

    $ wget https://github.com/jdunmire/MeasureFCI/archive/master.zip /tmp
    $ cd sketchbook/
    $ unzip /tmp/MeasureFCI-master.zip
    $ mv MeasureFCI-master/ MeasureFCI/
    $ rm /tmp/MearsureFCI-master.zip


## Operation
When the sketch starts it prints instructions on how to enter
Calibration mode, comma separated column labels, and then begins to
print the measured values:

    Hold push-button to enter calibration mode.
    Freq, Units, Value, Units
    238880, Hz, 221.9, uH
    238879, Hz, 221.9, uH
    ...

Monitor the output using the Arduino Serial Monitor. Use 115200 baud.

