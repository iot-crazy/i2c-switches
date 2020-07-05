# i2c-switches
Read the state of up to 16 switches using just 3 pins on an Arduino using I2C (two wire) serial data and a single interrupt pin to recieve notification of a switch changing. This solution does not require polling of the pins so frees up CPU cycles to do something more useful.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

You will need:
1. An MCP23018 E/SP chip
2. An ESP8266 based MCU
3. Two 4.7kΩ resistors
4. Up to 16 switches
5. Your choice of development or prototyping circuit board
6. Connecting wires

You will already know:
1. How to use the Arduino IDE (or IDE of your choice)
2. How to upload sketches / code to your device
3. How to read circuit diagrams

### Circuit Diagram

The circuit diagram is available here 
[https://www.schematics.com/project/i2c-switch-bank-170038/](https://www.schematics.com/project/i2c-switch-bank-170038/)

### Installing

1. Clone the repo to your local computer, or copy the .ino file.
2. Build the circuit to match the diagram

## Tested On

1. AZDelivery D1 Mini

Other baords coming soon
Please let us know if you test this on other devices so we can add to the list and acknowledge your contribution!

## Trouble Shooting

###Data can't be read from the MCP device
1. Is ADDR pin connected to ground which sets the address to 0x20
2. If using a different MCU, are the pin numbers set correctly in code for the SCL and SDA?

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
