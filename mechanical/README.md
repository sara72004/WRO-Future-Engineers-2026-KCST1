# 🔧 Mechanical Design

This section documents the mechanical structure, design decisions, and improvements made to the robot.

---

## 🧱 Chassis Design

The robot chassis was built using the MATRIX structural system. The design focuses on:

- Stability during movement  
- Proper weight distribution  
- Secure mounting of all components  

The controller, sensors, and battery are positioned to maintain balance and reduce unwanted vibrations.

---

## 🛞 Wheel System Upgrade

Initially, the robot used mixed wheel types, which caused:

- Uneven traction  
- Vibrations  
- Poor steering accuracy  

### ✅ Improvement

All wheels were replaced with identical **MATRIX TT Wheels (Blue Rim)**.

### 🎯 Result

- Smoother motion  
- Improved traction  
- More stable steering  

---

## 🔧 Custom 3D Printed Axle

### 📁 File
- `cad/wheel_shaft_83mm.stl`

### ⚠️ Problem

After upgrading the wheels, the axle did not fit tightly inside the wheel hub, leading to:

- Loose connection  
- Wheel wobbling  
- Inconsistent steering
    <div align="center">
  <img src="../v-photos/24.%20axle%20before%20(loose).jpeg" width="220"/>

### ✅ Solution

A custom 3D printed axle was designed to:

- Match the exact diameter of the wheel hub  
- Provide a tight and secure fit  
- Eliminate unwanted movement

  <div align="center">
  <img src="../v-photos/25.%203D%20printed%20axle%20pieces.jpeg" width="220"/>
  <img src="../v-photos/27.%20installed%20axle%20on%20robot.jpeg" width="220"/>
</div>

### 🎯 Result

- Increased steering precision  
- Improved mechanical stability  
- More consistent robot behavior  

---

## 🧱 CAD File

The custom 3D printed axle design is included in this repository.

| File | Description |
|---|---|
| [wheel_shaft_83mm.stl](wheel_shaft_83mm.stl) | Custom 3D printed axle used to improve wheel fit and steering stability |

This part was designed after the original axle was found to be loose inside the upgraded wheel hub.


## 🧪 Design Approach

The mechanical system was developed using an **iterative engineering process**:

1. Build initial design  
2. Test performance  
3. Identify mechanical issues  
4. Apply targeted improvements  

This approach ensured continuous improvement and reliable final performance.

