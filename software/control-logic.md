# Control Logic

The robot reads three laser sensors: front, left, and right.

Logic:
1. Move forward normally
2. Detect wall using front sensor
3. Compare left and right distances
4. Turn toward the more open side
5. First turn determines the overall lap direction
6. Continue following the same direction for the remaining turns
