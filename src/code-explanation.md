# Code Explanation

Two navigation strategies were developed and tested.

## 1. Stop-Before-Turn (stop_before_turn.ino)
- The robot stops briefly before turning
- Reduces drift
- More accurate turns
- Slightly slower

## 2. Continuous Turning (continuous_turn.ino)
- The robot keeps moving while turning
- Faster movement
- May introduce slight drift

## 3. Reverse-and-Turn Navigation Strategy
This version of the code stops the robot when a wall is detected, reverses slightly, and then performs the turn. This strategy was developed to improve turning space and reduce steering errors when the robot approaches walls too closely.

# Final Open Challenge Code Explanation

The final open challenge code uses a faster and more stable control strategy than the earlier timed-turn versions. Instead of only waiting for a front wall and performing a fixed turn, this version continuously follows one side wall using PID control while also using a front-wall override for corners.

## Main Strategy

The robot can run in two directions:

- **Clockwise (CW)**: follows the left wall
- **Counter-clockwise (CCW)**: follows the right wall

Before starting, the direction can be changed using the UP button. The robot starts when the DOWN button is pressed.

## Sensor Usage

The robot uses:

- **Front Laser Sensor (I2C2)**: detects upcoming walls and corners
- **Right Laser Sensor (I2C3)**: used for right-wall following
- **Left Laser Sensor (I2C4)**: used for left-wall following
- **Color Sensor V3 (I2C1)**: detects blue floor markers for lap/turn counting

## PID Wall Following

During normal driving, the robot follows one side wall using PID control.

The PID controller compares the measured wall distance with the target wall distance. The difference becomes the error. The controller then calculates a steering correction to keep the robot at a stable distance from the wall.

This makes the robot smoother and more responsive than the earlier fixed-timing codes.

## Front Wall Avoidance

When the front sensor detects a wall, the robot temporarily overrides normal PID wall following and performs a timed steering action.

The closer the wall is, the stronger the steering correction becomes.

This helps the robot handle corners more reliably without waiting too long or crashing into the wall.

## Servo Smoothing

The servo does not instantly jump to the target angle. Instead, it moves gradually toward the target. This reduces shaking, improves stability, and prevents sudden steering movements.

During front-wall avoidance, the servo response is temporarily increased so the robot can turn faster at corners.

## Speed Adjustment

The robot reduces speed when the steering angle becomes large. This helps reduce drift and keeps the robot stable during sharper corrections.

## Lap Counting

The color sensor detects blue floor markers. The robot counts a blue marker only when it first appears, and a cooldown is used to prevent counting the same marker multiple times.

When the required number of blue detections is reached, the robot stops.

## Why This Code Replaced the Earlier Versions

Earlier versions used fixed turn sequences such as:

- stop-then-turn
- continuous turn
- stop-reverse-then-turn

Those versions worked, but they were less adaptive. This final version performs better because it continuously corrects its path using sensor feedback, making it smoother, faster, and more reliable for the open challenge.


---

## Obstacle Challenge Camera Code Explanation

This code is used for the **Obstacle Challenge**. It is the most advanced version because it combines laser wall-following, camera obstacle detection, front-wall avoidance, and reverse recovery.

### Main Idea

The robot normally follows the arena wall using PID control. When the camera detects a red or green obstacle, the robot changes its steering to pass the obstacle from the correct side:

- **Red obstacle** → pass on the **right**
- **Green obstacle** → pass on the **left**

The code also includes a safety recovery system that reverses the robot if it gets too close to a wall or obstacle.

---

### Sensor Ports

- `I2C1` → Color Sensor V3
- `I2C2` → Front Laser Sensor
- `I2C3` → Right Laser Sensor
- `I2C4` → Left Laser Sensor
- `Serial1` → M-Vision Camera UART data

---

### Actuator Ports

- `M1` → TT Encoder Motor
- `RC3` → Steering Servo

---

### Camera Packet

The camera sends obstacle data in this format:

```text
seen,color,cx,cy,w,h,score,zone
```
---

### Control Priority System

The robot follows a priority-based decision system to ensure safe and reliable navigation.

1. Reverse Recovery  
   Activated when the robot is too close to a wall or obstacle. The robot reverses and repositions itself.

2. Camera Obstacle Guidance  
   When a valid red or green obstacle is detected, the robot adjusts its path to pass on the correct side.

3. Front Wall Assist  
   If the front laser detects a wall, the robot performs a temporary steering correction to avoid collision.

4. Normal PID Wall Following  
   Default behavior when no obstacles or hazards are present.

   ### PID Control System

The robot uses a PID controller to maintain a stable distance from the wall.

- Proportional (P): reacts to the current error between target distance and actual distance  
- Integral (I): corrects accumulated small errors over time  
- Derivative (D): reduces overshooting and stabilizes motion  

This allows smooth and accurate wall-following instead of sharp or unstable steering.

### System Advantages

This implementation improves performance compared to simpler approaches by combining:

- Sensor fusion (camera + laser sensors)
- Adaptive steering using PID control
- Reverse recovery for safety
- Obstacle-aware navigation
- Servo smoothing for stability

These features allow the robot to handle real competition conditions more reliably.
