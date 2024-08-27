# Water Boiler Heater

## Overview

The **Water Boiler Heater** is an Arduino project designed to utilize excess photovoltaic (PV) power to heat domestic water with a heating rod. This project leverages a TRIAC dimming module from RobotDyn to control the power delivered to the heating rod. The system integrates with Home Assistant for remote control and monitoring.

## Components Used

- **ESP8266**: Microcontroller for WiFi connectivity and control.
- **RobotDyn TRIAC Dimmer Module**: Controls the power to the heating rod using phase-cut dimming.
- **Home Assistant**: Used to remotely control and monitor the heating power level.

## Features

- **Remote Control**: Adjust the heating power level using Home Assistant.
- **Phase-Cut Dimming**: Efficiently controls the heating power with a TRIAC dimmer.
- **Web Integration**: Connects to Home Assistant for easy remote management.

## Library Note

The project uses the `RBDDimmer` library to interface with the TRIAC dimmer module. Note that there is a known bug with the `RBDDimmer` library when used with the ESP8266. The fix for this bug can be found in the [pull request #57](https://github.com/RobotDynOfficial/RBDDimmer/pull/57/commits/6c1d73f15ab00fc4977046bd4bc4cb42dd1d2147) of the library's GitHub repository.

## Installation

### Required Libraries

- **RBDDimmer**: Ensure you have the `RBDDimmer` library installed. You may need to apply the patch from the linked pull request to address the ESP8266 compatibility issue.

### Code Setup

1. **Include Required Libraries**: Ensure you include the necessary libraries in your Arduino sketch.

2. **Configure WiFi and MQTT Settings**: Replace the placeholder WiFi credentials and MQTT server information with your own.

3. **Pin Configuration**: Connect the TRIAC dimmer module to the appropriate GPIO pins on the ESP8266. Adjust the `dimmerPin` and `zerocrossPin` definitions in the code as needed.
