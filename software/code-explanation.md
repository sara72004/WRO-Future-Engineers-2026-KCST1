# Software Architecture and Obstacle Strategy

This document details the software architecture and the strategies implemented for obstacle avoidance and navigation for the Blaze robot. The software is designed with modularity and flexibility in mind, allowing for easy adaptation to different challenge scenarios and continuous improvement.

## 1. Overall Software Architecture

The core navigation logic is implemented using a **state machine approach**, which provides a clear and structured way to manage the robot's behavior based on its current environment and mission objectives. The state machine transitions are triggered by sensor inputs and internal logic, ensuring robust and predictable operation.

### State Machine Diagram

<div align="center">
  <img src="v-photos/state_machine.png" width="600"/>
</div>

**Explanation of States:**

*   **Initial:** The starting state of the robot, awaiting activation.
*   **Direction Detection:** The robot determines the initial direction of travel (clockwise or counter-clockwise) based on predefined criteria or sensor readings. This decision is crucial for consistent lap completion.
*   **Continuous Turn Strategy:** If speed is prioritized, the robot enters this state, executing turns while maintaining continuous forward motion. This strategy is optimized for faster lap times.
*   **Stop-Then-Turn Strategy:** If precision is critical, the robot enters this state, coming to a complete stop before executing a turn. This ensures high accuracy and minimal overshoot.
*   **Lap Completed:** After completing the required number of laps, the robot transitions to this state.
*   **Start Parking:** The robot initiates the parking maneuver, aiming to precisely position itself in the designated parking lot.
*   **End:** The robot successfully completes its mission and enters a standby state.

### 5.2 Lane Following and Obstacle Obedience Strategy

Our primary navigation method relies on **laser-based environmental sensing** to detect walls and maintain a consistent distance from them. This approach offers superior reliability compared to color-based methods, especially under varying lighting conditions.

### Lane Following Algorithm

Our lane following algorithm utilizes a **proportional controller** based on the lateral offset of the detected lane center. The laser sensors provide continuous feedback on the robot's distance to the left and right walls. This information is used to calculate the robot's deviation from the center of the lane. A proportional control loop then adjusts the steering angle to minimize this deviation, keeping the robot centered within the lane.

*   **Input:** Distance readings from left and right laser sensors.
*   **Calculation:** `Error = Distance_Right - Distance_Left`
*   **Output:** Steering adjustment proportional to `Error`.

### Obstacle Obedience Strategy

For the Obstacle Challenge, the robot must interpret traffic signs (green and red pillars) and adjust its lane-following behavior accordingly. Our strategy incorporates the following logic:

*   **Green Pillar (Keep Left):** Upon detecting a green pillar, the robot temporarily adjusts its proportional control to favor the left side of the lane, maintaining a closer distance to the left wall while passing the obstacle.
*   **Red Pillar (Keep Right):** Similarly, upon detecting a red pillar, the robot adjusts its control to favor the right side of the lane, maintaining a closer distance to the right wall.
*   **Avoidance:** The laser sensors also provide data for obstacle avoidance. If an obstacle is detected directly in the robot's path, a pre-programmed avoidance maneuver is initiated, which may involve temporary deviation from the lane center, followed by a return to the lane once the obstacle is cleared.

### 5.3 Algorithms Used

*   **Proportional Control (P-Controller):** Used for lane following to maintain the robot's position relative to the lane center. The steering output is directly proportional to the error (deviation from center).
*   **State Machine Logic:** Manages the overall flow of the robot's behavior, transitioning between different operational states (e.g., direction detection, continuous turn, stop-then-turn, parking) based on sensor inputs and mission progress).
*   **Decision-Making Algorithms:** Simple conditional logic is used for obstacle obedience (green/red pillars) and for selecting between continuous turn and stop-then-turn strategies based on predefined criteria or real-time performance metrics.

### 5.4 Testing and Tuning Process

Our software was developed through an iterative process of testing and tuning, primarily conducted on a custom home arena that replicated competition conditions.

*   **Unit Testing:** Individual modules, such as sensor reading, motor control, and basic navigation functions, were tested independently to ensure their correct operation.
*   **Integration Testing:** Different software components were integrated and tested together to verify their interactions and overall system behavior.
*   **Performance Tuning:** Parameters for the proportional controller (e.g., proportional gain) were fine-tuned through extensive experimentation on the test track. Metrics such as lap consistency, speed, and accuracy of turns were used to evaluate performance.
*   **Edge Case Handling:** The robot was tested under various challenging scenarios, including tight corners, unexpected obstacles, and different lighting conditions, to identify and address potential failure modes.

This rigorous testing and tuning process allowed us to optimize the robot's performance, ensuring its stability, reliability, and ability to consistently complete the challenges.
