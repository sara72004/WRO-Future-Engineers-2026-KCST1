# Wiring Notes

## Motors and Actuators
- M1: rear drive motor (controls forward movement)
- RC3: steering servo motor (controls front wheel direction)

## Laser Sensors
- I2C2: front laser sensor (detects walls or obstacles ahead)
- I2C3: right laser sensor (measures distance from the right side)
- I2C4: left laser sensor (measures distance from the left side)

## Color Sensor
- Model: MXColor Sensor V2
- Port: I2C1
- Mounted underneath the robot facing the ground
- Used to detect blue and orange floor colors to determine the initial navigation direction

## Camera
- Connected via UART port
- Mounted above the robot using a custom bracket
- Used to detect red and green obstacle blocks during the obstacle challenge
