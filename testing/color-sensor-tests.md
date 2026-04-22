# Color Sensor Tests

The color sensor was initially used to detect blue and orange floor lines.

Problems found:
- blue was detected better than orange
- orange detection was weaker
- at higher speed, the robot could miss the line

To improve performance, HSV values were displayed on the OLED and used for calibration.
