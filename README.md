# 🤖 WRO Future Engineers 2026 – KCST1  

## 🚀 Autonomous Self-Driving Robot

This repository presents the full design, development, and validation of our autonomous robot for the **World Robot Olympiad (WRO) 2026 – Future Engineers category**.

The system is designed to achieve **robust, repeatable, and stable navigation** within a closed arena using a combination of:
- Laser-based environmental sensing  
- Steering-based motion control  
- Deterministic decision-making algorithms  

---

## 👥 Team Members

### Sara
![Sara](t-photos/Sara.jpeg)

*Responsible for hardware design, mechanical integration, and repository (GitHub) management.*

---

### Abbas
![Abbas](t-photos/Abbas.jpeg)

*Responsible for software development, testing, and system validation.*

---

### 👩‍🏫 Coach
- Eng. Zainab


We would like to express our sincere appreciation to our coach for her guidance, support, and continuous feedback throughout the development of this project.
---


### Team
![Team](t-photos/Team%20WRO2025.jpeg)



## 🎥 Demonstration Video
👉 https://youtu.be/j1B9A54QlEQ  

---

## 🧠 Engineering Objective

The primary objective was to develop a robot capable of:

- Autonomous wall detection and corner navigation  
- Stable steering under varying speeds  
- Consistent lap completion without external input  
- High reliability under real competition conditions  

To achieve this, we followed an **iterative engineering process**, continuously refining both **hardware and software** based on real testing results.

---

## 🔧 Hardware Architecture

### 🧠 Controller
- MATRIX Mini R4 (MAFI900 kit)  
Acts as the central processing unit, handling:
- Sensor fusion  
- Control logic execution  
- Actuator control  

---

### 📡 Sensor System

- **3 × MATRIX Laser Sensor V2**
  - Front → forward obstacle detection  
  - Left & Right → lateral distance comparison  

✔ Selected as the **primary sensing system** due to:
- High reliability compared to color detection  
- Independence from lighting conditions  
- Consistent distance measurement  

---

- **MATRIX Color Sensor V3**
  - Used for floor color detection (blue/orange)  
  - Supports directional decision logic  

---

- **M-Vision Camera (MS-010)**
  - Used during early development stages  
  - Evaluated for object detection capability  
  - Later deprioritized due to complexity and inconsistency  

---

### ⚙️ Actuation System

- **TT Encoder Motor**
  - Provides forward motion  
  - Enables controlled speed output  

- **MG996R Servo Motor (Metal Gearbox)**
  - Controls steering angle  
  - Chosen for:
    - High torque  
    - Mechanical durability  
    - Precision control  

---

### 🛞 Wheel System

- MATRIX TT Wheels (Blue Rim)

#### 🔍 Design Decision

Initial configuration used mixed wheels (yellow + blue), which resulted in:
- Uneven traction  
- Vibrations during movement  
- Reduced steering accuracy  

#### ✅ Improvement
All wheels were replaced with identical blue wheels, resulting in:
- Uniform friction  
- Improved stability  
- Smoother turning behavior  

---

### 🔧 Mechanical Innovation – Custom Axle

#### Problem:
After upgrading the wheels, the axle did not fit tightly inside the wheel hub, causing:
- Mechanical play  
- Inconsistent steering response  

#### Solution:
A **custom 3D-printed axle** was designed and implemented to:
- Achieve a perfect fit  
- Eliminate unwanted movement  
- Improve steering precision  

✔ This modification significantly increased control accuracy and repeatability.

---

## 🧭 Navigation System Design

### ❌ Initial Approach – Color-Based Navigation

The robot initially relied on:
- Floor color detection (blue/orange)  
- Camera-assisted obstacle handling  

#### Observed Issues:
- Missed color lines at higher speeds  
- Sensitivity to lighting conditions  
- Inconsistent behavior across runs  

---

### ✅ Final Approach – Laser-Based Navigation

A fully sensor-driven navigation system was developed using three laser sensors.

### Core Algorithm:
1. Robot moves forward continuously  
2. When front sensor detects a wall:
   - Compare left and right distances  
   - Turn toward the side with greater available space  

---

### 🔁 Direction Locking Strategy

To ensure consistent pathing:

- The **first detected turn determines direction**:
  - Right side closer → turn left → Counter-clockwise  
  - Left side closer → turn right → Clockwise  

- All subsequent turns follow the same direction  

✔ This eliminates oscillation and improves path stability.

---

## 💻 Software Architecture

Two control strategies were implemented to optimize performance under different conditions:

---

### 🅰️ Strategy 1 – Stop-Then-Turn

- Robot stops before executing a turn  
- Reduces inertia and drift  
- Improves turning accuracy  

✔ Advantage:
- High precision  

❌ Trade-off:
- Slower lap time  

---

### 🅱️ Strategy 2 – Continuous Turn

- Robot maintains forward motion while turning  
- Reduces time loss  
- Increases speed  

✔ Advantage:
- Faster performance  

❌ Trade-off:
- Slight turning drift  

---

### 🎯 Design Decision

Both strategies were retained to allow:
- Adaptability during competition  
- Selection based on arena performance  

---

## 🧪 Testing & Validation

### Challenge:
Limited access to the official competition arena  

### Solution:
A **custom home testing environment** was constructed using:
- Boxes to simulate walls  
- Replicated track layout  

### Outcome:
- Enabled continuous testing  
- Allowed parameter tuning  
- Improved reliability before competition  

---

## 📁 Repository Structure
engineering_journal/ → detailed development process and decisions
hardware/ → components and wiring documentation
mechanical/ → design iterations and improvements
software/ → control logic explanation
src/ → source code implementations
testing/ → experimental results and validation
t-photos/ → team documentation
v-photos/ → robot images
video/ → demonstration video


---

## 🏁 Engineering Summary

This project demonstrates a complete engineering workflow:

- Problem identification through testing  
- Iterative improvement of hardware and software  
- Data-driven decision making  
- Practical solutions (e.g., custom axle design)  

### Key Outcomes:
- Transition to a more reliable sensing system  
- Improved mechanical stability  
- Flexible software strategies  
- Consistent autonomous performance  

---

## 🧾 Final Note

The final robot achieves **stable, repeatable, and reliable navigation**, meeting the requirements of the WRO Future Engineers challenge while demonstrating strong engineering design principles.
