
# WRO 2026 Future Engineers - Team KCST1 Engineering Journal

## Team Information

| Name | Role |
| :--- | :--- |
| **Sara** | Hardware Design, Mechanical Integration, GitHub Management |
| **Abbas** | Software Development, Testing, System Validation |

### Coach

*   Eng. Zainab

## 1. Introduction: Blaze – Autonomous Self-Driving Robot

This Engineering Journal documents the design, development, and validation of **Blaze**, our autonomous robot developed for the **World Robot Olympiad (WRO ) 2026 – Future Engineers category**. Blaze is engineered to achieve robust, repeatable, and stable navigation within a closed arena, leveraging laser-based environmental sensing, steering-based motion control, and deterministic decision-making algorithms.

The name **Blaze** embodies speed, precision, and reliability—the foundational principles guiding our robot's design. It signifies fast and decisive movement, strong and stable performance under pressure, and consistent execution across multiple runs. Our development process involved multiple iterations, integrating mechanical improvements, sensor optimization, and refined control logic to achieve competition-level performance.

## 2. Engineering Objective

The primary objective for Blaze was to develop a robot capable of:

*   Autonomous wall detection and corner navigation.
*   Stable steering under varying speeds.
*   Consistent lap completion without external input.
*   High reliability under real competition conditions.

## 3. Mobility and Mechanical Design

This section details the design choices and mechanical considerations for the Blaze robot's chassis and drive system. Our iterative design process focused on achieving a balance between stability, maneuverability, and durability, crucial for navigating the WRO competition track.

### 3.1 Chassis Design Choices

Our chassis design prioritizes a low center of gravity and a wide wheelbase to enhance stability during high-speed maneuvers and turns. The frame is constructed from lightweight yet rigid materials to minimize overall robot weight while maximizing structural integrity.

*   **Material Selection:** High-strength aluminum profiles were chosen for the main frame due to their excellent strength-to-weight ratio and ease of assembly. This provides a robust platform that can withstand minor impacts and vibrations without compromising structural integrity.
*   **Layout:** A rectangular base with centrally mounted components (controller, battery) ensures balanced weight distribution, which is critical for consistent traction and predictable handling.
*   **Accessibility:** The design allows for easy access to internal components for maintenance, adjustments, and quick battery swaps during competition.

### 3.2 Steering and Drive Mechanism

Blaze utilizes a **differential drive system** combined with a **steering mechanism** for precise control. This hybrid approach allows for both agile turning and stable straight-line driving.

*   **Drive System:** Two TT Encoder Motors independently drive the rear wheels. The encoders provide precise feedback on wheel rotation, enabling accurate speed control and odometry for navigation.
*   **Steering Mechanism:** A single MG996R Servo Motor controls the front steering. This setup provides responsive steering input, allowing the robot to execute sharp turns and maintain its trajectory with high accuracy.

### 3.3 Torque and Speed Reasoning

The selection of motors and gear ratios was a critical aspect of our mechanical design, directly impacting the robot's performance in terms of speed, acceleration, and ability to handle varying loads.

*   **TT Encoder Motors:** These motors were chosen for their balance of torque and speed, coupled with integrated encoders for precise control. The gear ratio of the motors (e.g., 1:48) was selected to provide sufficient torque for rapid acceleration and climbing small inclines, while also allowing for a maximum speed suitable for the competition track.
    *   **Torque Calculation:** The required torque was estimated based on the robot's weight, wheel diameter, and expected acceleration. This ensured that the motors could overcome static friction and accelerate the robot effectively.
    *   **Speed Considerations:** The maximum rotational speed of the motors, combined with the wheel diameter, determined the robot's top speed. This was balanced against the need for control and stability, as excessively high speeds can lead to loss of traction and reduced accuracy.

### 3.4 Design Tradeoffs

Several design tradeoffs were considered during the development of the chassis and mechanical system:

*   **Weight vs. Durability:** A lighter robot generally achieves higher speeds and better acceleration. However, reducing weight too much can compromise durability. We opted for a balance, using lightweight materials where possible without sacrificing structural integrity.
*   **Speed vs. Precision:** As highlighted in the software architecture, there's an inherent tradeoff between speed and precision in navigation. Our mechanical design supports both strategies (Stop-Then-Turn for precision, Continuous Turn for speed) by providing a stable and responsive platform.
*   **Complexity vs. Reproducibility:** While more complex mechanical designs might offer marginal performance gains, we prioritized a design that is relatively straightforward to build and reproduce, aligning with the WRO competition's emphasis on engineering documentation and reproducibility.

### 3.5 Mechanical Innovation – Custom Axle

#### Problem:

Initial designs suffered from a loose axle, leading to unstable steering and reduced accuracy during turns. This significantly impacted the robot's ability to maintain a consistent path and achieve optimal lap times.

#### Solution:

A custom 3D printed axle was designed and implemented. This innovative solution provides a more secure and precise fit, eliminating the play in the axle and significantly improving steering stability and overall control. The CAD files for this custom axle are available in the `mechanical/CAD` directory, allowing for easy reproduction and modification.

**Result:** Precise steering and stability, leading to improved navigation and reduced error rates.

### 3.6 Testing and Iterations

Our mechanical design underwent several iterations based on testing feedback:

*   **Initial Prototype:** A basic chassis was assembled to test fundamental concepts of drive and steering. This revealed issues with axle stability, leading to the custom 3D printed axle.
*   **Wheel System Upgrade:** Early tests with mixed wheel types resulted in inconsistent tracking. Switching to a uniform MATRIX TT wheel system significantly improved stability and predictability.
*   **Stress Testing:** The chassis was subjected to various stress tests, including impacts and rapid maneuvers, to identify weak points and ensure durability. Reinforcements were added where necessary.

These iterative improvements, driven by empirical testing, were crucial in refining the mechanical design to achieve optimal performance and reliability for the competition.

## 4. Power and Sensor Architecture

This section details the power system architecture and the selection and placement of sensors, crucial for the robot's autonomous operation.

### 4.1 Component Power Consumption

The following table summarizes the estimated current draw and power consumption for the primary electrical components of the Blaze robot:

| Component | Operating Voltage (V) | Typical Current (A) | Peak Current (A) | Power Consumption (W) |
| :--------------------------------- | :-------------------- | :------------------ | :--------------- | :-------------------- |
| MATRIX Mini R4 (Controller) | 5 | 0.2 | 0.5 | 2.5 |
| MATRIX Laser Sensor V2 (x3) | 5 | 0.05 (each) | 0.1 (each) | 0.5 (total) |
| MATRIX Color Sensor V3 | 5 | 0.02 | 0.05 | 0.25 |
| M-Vision Camera (MS-010) | 5 | 0.1 | 0.2 | 1.0 |
| TT Encoder Motor (x2) | 12 | 0.5 (each) | 1.5 (each) | 36.0 (total peak) |
| MG996R Servo Motor | 5 | 0.1 | 0.5 | 2.5 |
| **Total Estimated Peak Consumption** | | | **~4.85** | **~42.75** |

*Note: Current and power values are approximate and can vary based on specific operating conditions and load. Peak current for motors is estimated during high-load situations (e.g., rapid acceleration or turning).*

### 4.2 Power System Architecture

The robot's power system is designed around a 3-cell LiPo battery, providing a nominal voltage of 11.1V (fully charged at 12.6V). This battery feeds into a main power distribution board, which then supplies power to various components through appropriate voltage regulators:

*   **12V Line:** Directly powers the TT Encoder Motors, ensuring they receive sufficient voltage for optimal torque and speed.
*   **5V Regulator:** A dedicated 5V regulator steps down the battery voltage to supply the MATRIX Mini R4 controller, all sensors (Laser, Color, Camera), and the MG996R Servo Motor. This ensures stable and clean power for sensitive electronic components.

### 4.3 Justification of Design Choices

*   **LiPo Battery Selection:** Chosen for its high energy density and discharge rate, suitable for powering the motors and electronics for extended periods during competition rounds.
*   **Voltage Regulators:** Implemented to provide stable and correct operating voltages for each component, protecting sensitive electronics from voltage fluctuations and ensuring reliable performance.
*   **Current Draw Reasoning:** The total estimated peak current draw of approximately 4.85A (primarily driven by the motors) is well within the safe operating limits of the selected LiPo battery and voltage regulators, which are rated to handle higher continuous and peak currents. This prevents overheating and ensures consistent power delivery.

This detailed power budget analysis confirms the robustness of our power system architecture, minimizing the risk of power-related failures during critical competition moments.

### 4.4 Sensor Selection and Placement

*   **3 × MATRIX Laser Sensor V2:** These sensors are strategically placed to provide accurate distance measurements to the track walls, crucial for lane following and obstacle detection. Their reliability and independence from lighting conditions were key factors in their selection.
*   **MATRIX Color Sensor V3:** Used for initial track detection and identifying specific colored markers on the track. Its placement is optimized for detecting track lines directly beneath the robot.
*   **M-Vision Camera (MS-010):** Provides broader environmental awareness, enabling the robot to identify larger obstacles and traffic signs (pillars) for advanced decision-making. Its position is elevated to provide a wider field of view.

## 5. Software Architecture and Obstacle Strategy

This section details the software architecture and the strategies implemented for obstacle avoidance and navigation for the Blaze robot. The software is designed with modularity and flexibility in mind, allowing for easy adaptation to different challenge scenarios and continuous improvement.

### 5.1 Overall Software Architecture

The core navigation logic is implemented using a **state machine approach**, which provides a clear and structured way to manage the robot's behavior based on its current environment and mission objectives. The state machine transitions are triggered by sensor inputs and internal logic, ensuring robust and predictable operation.

### State Machine Diagram

<div align="center">
  <img src="../v-photos/state_machine.png" width="600"/>
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
*   **State Machine Logic:** Manages the overall flow of the robot's behavior, transitioning between different operational states (e.g., direction detection, continuous turn, stop-then-turn, parking) based on sensor inputs and mission progress.
*   **Decision-Making Algorithms:** Simple conditional logic is used for obstacle obedience (green/red pillars) and for selecting between continuous turn and stop-then-turn strategies based on predefined criteria or real-time performance metrics.

### 5.4 Testing and Tuning Process

Our software was developed through an iterative process of testing and tuning, primarily conducted on a custom home arena that replicated competition conditions.

*   **Unit Testing:** Individual modules, such as sensor reading, motor control, and basic navigation functions, were tested independently to ensure their correct operation.
*   **Integration Testing:** Different software components were integrated and tested together to verify their interactions and overall system behavior.
*   **Performance Tuning:** Parameters for the proportional controller (e.g., proportional gain) were fine-tuned through extensive experimentation on the test track. Metrics such as lap consistency, speed, and accuracy of turns were used to evaluate performance.
*   **Edge Case Handling:** The robot was tested under various challenging scenarios, including tight corners, unexpected obstacles, and different lighting conditions, to identify and address potential failure modes.

This rigorous testing and tuning process allowed us to optimize the robot's performance, ensuring its stability, reliability, and ability to consistently complete the challenges.

## 6. Systems Thinking and Engineering Decisions

Our development process for Blaze was guided by a strong emphasis on systems thinking, where the robot is viewed as an integrated system rather than a collection of independent parts. This approach allowed us to make informed engineering decisions, considering the interplay between different subsystems and their impact on overall performance.

### 6.1 Subsystem Interactions

*   **Mobility & Software:** The mechanical design of the chassis and drive system directly influences the effectiveness of the software's navigation algorithms. For instance, a stable chassis and precise steering mechanism enable the proportional controller to make accurate adjustments, leading to smoother lane following.
*   **Sensors & Software:** The choice and placement of sensors dictate the quality and type of data available to the software. Laser sensors provide reliable distance data, which is crucial for the software's ability to accurately detect walls and obstacles.
*   **Power & All Subsystems:** The power system architecture ensures that all components receive stable and sufficient power, preventing unexpected behavior or failures due to power fluctuations. A well-designed power budget is critical for sustained operation.

### 6.2 Engineering Reasoning Behind Decisions

Throughout the project, decisions were made based on a careful evaluation of tradeoffs and constraints.

*   **Sensor Selection:** The decision to transition from color-based navigation to laser-based sensing was driven by the need for greater reliability and consistency under varying lighting conditions. While color sensors might be simpler to implement initially, their performance was found to be too sensitive to environmental factors. Laser sensors, although potentially more complex to integrate, provided the robust distance measurements required for accurate wall detection and lane following.
*   **Steering Mechanism:** The choice of a differential drive system combined with a steering mechanism was a deliberate tradeoff between agility and stability. This hybrid approach allows for both sharp turns and stable straight-line driving, providing a versatile platform for navigating the competition track.
*   **Custom Axle:** The development of a custom 3D printed axle was a direct response to the instability observed in the initial steering mechanism. This decision highlights our commitment to addressing mechanical issues through innovative design solutions, rather than relying solely on software compensation.

### 6.3 Constraints and Tradeoffs

*   **Weight vs. Durability:** A lighter robot generally achieves higher speeds and better acceleration. However, reducing weight too much can compromise durability. We opted for a balance, using lightweight materials where possible without sacrificing structural integrity.
*   **Speed vs. Precision:** As highlighted in the software architecture, there's an inherent tradeoff between speed and precision in navigation. Our mechanical design supports both strategies (Stop-Then-Turn for precision, Continuous Turn for speed) by providing a stable and responsive platform.
*   **Complexity vs. Reproducibility:** While more complex mechanical designs might offer marginal performance gains, we prioritized a design that is relatively straightforward to build and reproduce, aligning with the WRO competition's emphasis on engineering documentation and reproducibility.

### 6.4 Iteration and Testing Cycles

Our development process was characterized by continuous iteration and testing.

*   **Initial Prototype:** A basic chassis was assembled to test fundamental concepts of drive and steering. This revealed issues with axle stability, leading to the custom 3D printed axle.
*   **Wheel System Upgrade:** Early tests with mixed wheel types resulted in inconsistent tracking. Switching to a uniform MATRIX TT wheel system significantly improved stability and predictability.
*   **Stress Testing:** The chassis was subjected to various stress tests, including impacts and rapid maneuvers, to identify weak points and ensure durability. Reinforcements were added where necessary.

### 6.5 Risk Identification and Mitigation Actions

*   **Power Failure:** The risk of power failure during a competition round was mitigated by implementing a robust power system architecture with a dedicated 5V regulator for sensitive electronics and a 12V line for the motors. A detailed power budget analysis confirmed the adequacy of the selected LiPo battery and voltage regulators.
*   **Sensor Interference:** The risk of sensor interference or misreadings was addressed by carefully selecting and placing the sensors. Laser sensors were chosen for their reliability and independence from lighting conditions, while the camera was positioned to provide a clear view of the track and obstacles.
*   **Mechanical Instability:** The risk of mechanical instability, particularly in the steering mechanism, was mitigated by designing and implementing a custom 3D printed axle. This innovative solution provided a more secure and precise fit, eliminating play and significantly improving steering stability.

## 7. Reproducibility and GitHub Quality

This section details the structure and quality of our GitHub repository, ensuring that our engineering process and final robot design are fully reproducible by other teams.

### 7.1 GitHub Structure and Clarity

Our GitHub repository is organized logically, with clear directories for different aspects of the project:

*   `engineering_journal/`: Detailed documentation of the engineering process, design decisions, and iterative improvements.
*   `hardware/`: Specifications and documentation for all electronic components, including the crucial power budget.
*   `mechanical/`: Mechanical design files, including CAD models for 3D printed parts and assembly instructions.
*   `schemes/`: Wiring diagrams and system block diagrams.
*   `software/`: Documentation of software architecture and algorithms, including a state machine diagram.
*   `src/`: All source code files for the robot.
*   `testing/`: Documentation of testing procedures, results, and validation reports.
*   `t-photos/`: Team photos.
*   `v-photos/`: Photos of the vehicle from various angles.
*   `video/`: Links to demonstration videos.

### 7.2 Commit History

Our repository features a comprehensive commit history, reflecting the iterative nature of our development process. The commits are meaningful and provide a clear record of the changes made over time.

### 7.3 README Content and Structure

The main `README.md` file provides a comprehensive overview of the project, including team information, engineering objectives, hardware and software architecture, and instructions for building and compiling the code. It serves as a central hub for understanding the project and navigating the repository.

### 7.4 File Organization

Files are organized logically within their respective directories, making it easy to locate specific information or components. The use of clear and descriptive filenames further enhances the repository's usability.

### 7.5 CAD, Code, Wiring, and Related Technical Files

All necessary technical files, including CAD models, source code, and wiring diagrams, are included in the repository. This ensures that other teams have access to all the information required to reproduce our robot design.

### 7.6 Reproducibility of the Robot

Our comprehensive documentation, clear repository structure, and inclusion of all necessary technical files ensure that our robot design is fully reproducible. Other teams can follow our engineering process, understand our design decisions, and build their own version of Blaze using the provided resources.

## 8. Conclusion

The development of Blaze has been a challenging yet rewarding engineering journey. Through a systematic approach, rigorous testing, and continuous iteration, we have created a robust and reliable autonomous robot capable of navigating the WRO 2026 Future Engineers competition track. This Engineering Journal serves as a comprehensive record of our work, demonstrating our commitment to engineering excellence and our willingness to share our knowledge with the broader robotics community.
