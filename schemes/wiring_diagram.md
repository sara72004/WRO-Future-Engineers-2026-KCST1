===========================
MATRIX MINI R4 CONNECTIONS
===========================

[POWER]
Battery Pack (2x18650) → Mini R4 Power Input

---------------------------
[SENSORS - I2C]
---------------------------
Front Laser Sensor → I2C2
Right Laser Sensor → I2C3
Left Laser Sensor  → I2C4

Color Sensor V3 → I2C1

(All use: VCC, GND, SDA, SCL)

---------------------------
[CAMERA - UART]
---------------------------
M-Vision Camera → UART Port
- VCC → 5V
- GND → GND
- TX → RX
- RX → TX

---------------------------
[ACTUATORS]
---------------------------
TT Encoder Motor → M1 Port
MG996R Servo → RC3 Port

===========================
