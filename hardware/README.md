# 🔧 Hardware Documentation

This folder documents the main hardware components used in Blaze, our WRO Future Engineers 2026 robot.

## Main Controller
- **MATRIX Mini R4 Controller**
- Used as the central control unit for reading sensors, processing navigation logic, and controlling the motor and steering servo.

## Sensors
- **Front Laser Sensor V2** → I2C2  
- **Right Laser Sensor V2** → I2C3  
- **Left Laser Sensor V2** → I2C4  
- **MATRIX Color Sensor V3** → I2C1  
- **M-Vision Camera** → UART  

The laser sensors are the main sensors used for the final open challenge strategy because they provide fast and consistent distance readings.

## Actuators
- **TT Encoder Motor** → M1  
  - Provides forward movement.

- **MG996R Servo Motor** → RC3  
  - Controls the steering system.

## Power
- **2 Cell 18650 Battery Box**
- Powers the MATRIX Mini R4 controller and connected components.

## Final Hardware Strategy
The final robot uses laser-based wall following with PID control. The color sensor is used for detecting blue markers for lap counting, while the camera was mainly used during development and testing.
