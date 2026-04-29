# Power Budget

This document outlines the power budget for the Blaze robot, detailing the current draw and power consumption of key components. This analysis ensures that the selected power supply (LiPo battery) and voltage regulators are adequate to meet the robot's operational demands, especially during peak performance scenarios.

## Component Power Consumption

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

## Power System Architecture

The robot's power system is designed around a 3-cell LiPo battery, providing a nominal voltage of 11.1V (fully charged at 12.6V). This battery feeds into a main power distribution board, which then supplies power to various components through appropriate voltage regulators:

*   **12V Line:** Directly powers the TT Encoder Motors, ensuring they receive sufficient voltage for optimal torque and speed.
*   **5V Regulator:** A dedicated 5V regulator steps down the battery voltage to supply the MATRIX Mini R4 controller, all sensors (Laser, Color, Camera), and the MG996R Servo Motor. This ensures stable and clean power for sensitive electronic components.

## Justification of Design Choices

*   **LiPo Battery Selection:** Chosen for its high energy density and discharge rate, suitable for powering the motors and electronics for extended periods during competition rounds.
*   **Voltage Regulators:** Implemented to provide stable and correct operating voltages for each component, protecting sensitive electronics from voltage fluctuations and ensuring reliable performance.
*   **Current Draw Reasoning:** The total estimated peak current draw of approximately 4.85A (primarily driven by the motors) is well within the safe operating limits of the selected LiPo battery and voltage regulators, which are rated to handle higher continuous and peak currents. This prevents overheating and ensures consistent power delivery.

This detailed power budget analysis confirms the robustness of our power system architecture, minimizing the risk of power-related failures during critical competition moments.
