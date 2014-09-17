Weatherbox
============================

## Introduction

This is the primary directory for the reis weatherbox project. The goal of the reis weatherbox
project is to create a cheap, easy to use and maintainable weather data collection device 
which can then be massively deployable across a university campus. 

The current firmware release is skynet-b03.

## Packet Schema Documentation
| Sensor                | Description                                           | 
| -------               | -----------                                           |
| bmp085_press_pa       | bmp085 pressure sensor in pascals                     |
| bmp085_temp_decic     | bmp085 temperature sensor in degress C times 10       |
| humidity_centi_pct    | sht1x humidity in percent times 100                   |
| apogee_w_m2           | apogee solar irradiance sensor voltage in millivolts  |
| batt_mv               | battery voltage in millivolts                         |
| panel_mv              | solar panel voltage in millivolts                     |
| uptime_ms             | the current uptime in milliseconds                    |
| overflow_num          | how many times the curent uptime timer has overflowed |
| n                     | number of datapoints in the packet                    |
| schema                | the packet schema number                              |
| address               | the weatherbox node address                           | 

## Current collaborators:

* Kenny Luong
* Sean Hara
* Zachary Dorman
* Christie Obatake 
* Ryan Walser


## Arduino IDE Library Setup

Although efforts to move away from the arduino IDE are underway, it is still the best platform to 
develop the weatherbox. A quick note about the library -- you'll neeed to either add the 
library contents to your own arduino library folder, or just make repository your
sketchbook folder. I'd recommend the fromer. If you're on linux, symlinking all of the folders
in the weatherbox/libraries directory should work. 

