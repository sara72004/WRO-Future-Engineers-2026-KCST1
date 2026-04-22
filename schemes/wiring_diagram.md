# 🔌 Wiring Diagram

This diagram shows the **physical connections** between all electronic components and the MATRIX Mini R4 controller.

---

## 📊 Diagram

![Wiring Diagram](../v-photos/mermaid-diagram%20wiring.png)

---

## 🔍 Description

The wiring diagram provides a detailed view of how each component is connected to specific ports on the controller.

---

## ⚡ Power Connections

- **2x18650 Battery Pack → Controller Power Input**
  - Supplies power to the entire system
  - The controller distributes power to sensors and actuators

---

## 📡 Sensor Connections

### Laser Sensors (I2C Communication)

- **Front Laser Sensor → I2C2**
- **Right Laser Sensor → I2C3**
- **Left Laser Sensor → I2C4**

✔ These sensors provide real-time distance measurements for navigation.

---

### Color Sensor

- **Color Sensor V3 → I2C1**

✔ Used for detecting floor colors and determining movement direction.

---

### Camera Module

- **M-Vision Camera → UART Port**

✔ Used during development for object detection and testing.

---

## ⚙️ Actuator Connections

### Motor

- **TT Encoder Motor → M1 Port**

✔ Controls forward movement and speed.

---

### Steering

- **MG996R Servo Motor → RC3 Port**

✔ Controls the steering angle of the robot.

---

## 🎯 Purpose

This diagram helps to:

- Ensure correct hardware assembly  
- Avoid incorrect wiring during setup  
- Provide a reference for troubleshooting  

---

## ✅ Summary

The wiring design ensures:

- Clean and organized connections  
- Proper communication between components  
- Reliable power distribution  

All connections are made directly to the **MATRIX Mini R4**, making the system simple and efficient.
