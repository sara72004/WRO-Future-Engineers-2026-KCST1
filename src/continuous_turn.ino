#include "MatrixMiniR4.h"   // Main library for Matrix Mini R4 board

// =====================================================
//                    GLOBAL VARIABLES
// =====================================================

// Start flag:
// false = robot waits in standby
// true  = robot begins running
bool Start = false;

// Forward motor speed
int MSpeed = 100;

// Servo center angle:
// this is the straight steering position
float ServoCenter = 54.2;

// Number of turns completed so far
int Turns = 0;

// Lap direction chosen after first turn
// 'L' = counter-clockwise
// 'R' = clockwise
// '-' = not decided yet
char Direction = '-';


// =====================================================
//                  SENSOR READING VARIABLES
// =====================================================

// Distance from front laser sensor
int frontS = 0;

// Distance from right laser sensor
int rightS = 0;

// Distance from left laser sensor
int leftS = 0;


// =====================================================
//                 OBSTACLE DETECTION STATE
// =====================================================

// Current loop obstacle state
bool obstacleNow = false;

// Previous loop obstacle state
// used to detect when an obstacle first appears
bool obstacleBefore = false;


// =====================================================
//                    STEERING COOLDOWN
// =====================================================

// Stores the time when the last steering action happened
unsigned long lastSteerTime = 0;

// Minimum time between two steering actions
// prevents rapid repeated turns from one wall detection
const unsigned long steerCooldown = 150;


// =====================================================
//                     TURN SETTINGS
// =====================================================

// Steering amount away from center
// bigger value = sharper turn
int TurnOffset = 28;

// Time in milliseconds the wheels stay turned
// robot continues moving during this time
int TurnTime = 950;


// =====================================================
//         HELPER FUNCTION: LIMIT SERVO ANGLE
// =====================================================

// Keeps the servo angle inside a safe range
int clampServo(int angle, int minAngle, int maxAngle)
{
  if (angle < minAngle) return minAngle;   // Use min if angle is too small
  if (angle > maxAngle) return maxAngle;   // Use max if angle is too large
  return angle;                            // Otherwise return original value
}


// =====================================================
//                    LEFT TURN FUNCTION
// =====================================================

void steerLeft()
{
  // Turn front wheels to the left
  MiniR4.RC3.setAngle(ServoCenter + TurnOffset);

  // Keep the wheels turned left for TurnTime milliseconds
  // Note: motor is not stopped here, so robot keeps moving
  delay(TurnTime);

  // Recenter the wheels after finishing the turn
  MiniR4.RC3.setAngle(ServoCenter);
}


// =====================================================
//                   RIGHT TURN FUNCTION
// =====================================================

void steerRight()
{
  // Turn front wheels to the right
  MiniR4.RC3.setAngle(ServoCenter - TurnOffset);

  // Keep the wheels turned right for TurnTime milliseconds
  // Note: motor is not stopped here, so robot keeps moving
  delay(TurnTime);

  // Recenter the wheels after finishing the turn
  MiniR4.RC3.setAngle(ServoCenter);
}


// =====================================================
//                         SETUP
// =====================================================

void setup()
{
  // Initialize Matrix board
  MiniR4.begin();

  // Set battery type as 2-cell battery
  MiniR4.PWR.setBattCell(2);

  // ---------------- Motor setup ----------------

  // Set motor direction
  MiniR4.M1.setReverse(false);

  // Start with motor off
  MiniR4.M1.setPower(0);

  // Disable brake mode
  MiniR4.M1.setBrake(false);

  // Reset motor encoder/counter
  MiniR4.M1.resetCounter();

  // ---------------- Servo setup ----------------

  // Put steering in center position at startup
  MiniR4.RC3.setAngle(ServoCenter);

  // ---------------- Laser sensor setup ----------------

  // Front sensor
  MiniR4.I2C2.MXLaserV2.begin();

  // Right sensor
  MiniR4.I2C3.MXLaserV2.begin();

  // Left sensor
  MiniR4.I2C4.MXLaserV2.begin();

  // ---------------- OLED startup display ----------------

  // Clear screen
  MiniR4.OLED.clearDisplay();

  // First startup line
  MiniR4.OLED.setCursor(0, 0);
  MiniR4.OLED.print("Robot On Standby");

  // Second startup line
  MiniR4.OLED.setCursor(0, 10);
  MiniR4.OLED.print("R U Flippin' Ready?");

  // Show text on OLED
  MiniR4.OLED.display();

  // Small pause so the message is visible
  delay(1000);
}


// =====================================================
//                          LOOP
// =====================================================

void loop()
{
  // If DOWN button is pressed, start the robot
  if (MiniR4.BTN_DOWN.getState())
  {
    Start = true;     // Enable running mode
    delay(300);       // Debounce delay
  }

  // If robot has not been started yet, do nothing
  if (!Start)
  {
    return;
  }

  // ---------------- Read distance sensors ----------------

  // Read front laser distance
  frontS = MiniR4.I2C2.MXLaserV2.getDistance();

  // Read right laser distance
  rightS = MiniR4.I2C3.MXLaserV2.getDistance();

  // Read left laser distance
  leftS = MiniR4.I2C4.MXLaserV2.getDistance();

  // ---------------- Obstacle detection ----------------

  // If front distance is between 1 and 900,
  // treat it as a wall/obstacle in front
  obstacleNow = (frontS >= 1 && frontS <= 900);

  // ---------------- Cooldown logic ----------------

  // Only allow a turn if enough time has passed since last turn
  bool canSteer = (millis() - lastSteerTime >= steerCooldown);

  // ---------------- Normal forward driving ----------------

  // If there is no obstacle in front,
  // keep moving forward with wheels centered
  if (!obstacleNow)
  {
    MiniR4.M1.setPower(MSpeed);
    MiniR4.RC3.setAngle(ServoCenter);
  }

  // =====================================================
  //         TURN ONLY WHEN OBSTACLE FIRST APPEARS
  // =====================================================

  // Turn only when:
  // 1. obstacle exists now
  // 2. obstacle did not exist in previous loop
  // 3. enough cooldown time has passed
  if (obstacleNow && !obstacleBefore && canSteer)
  {
    // ---------------- First turn decides lap direction ----------------
    if (Turns == 0)
    {
      // Compare side distances:
      // if right is smaller than left,
      // left side is more open, so turn left
      if (rightS < leftS)
      {
        Direction = 'L';          // Set lap direction to counter-clockwise
        Turns = 1;                // First turn completed
        lastSteerTime = millis(); // Save time of this turn
        steerLeft();              // Turn left immediately
      }

      // If right side is more open, turn right
      else if (rightS > leftS)
      {
        Direction = 'R';          // Set lap direction to clockwise
        Turns = 1;                // First turn completed
        lastSteerTime = millis(); // Save time of this turn
        steerRight();             // Turn right immediately
      }
    }

    // ---------------- All later turns ----------------
    else
    {
      // Keep turning until 12 turns are completed
      if (Turns < 12)
      {
        Turns++;                  // Increase turn count
        lastSteerTime = millis(); // Save time of this turn

        // Follow the already chosen lap direction
        if (Direction == 'L')
        {
          steerLeft();
        }
        else if (Direction == 'R')
        {
          steerRight();
        }
      }

      // Stop robot after 12 turns
      else if (Turns == 12)
      {
        MiniR4.M1.setPower(0);         // Stop motor
        MiniR4.RC3.setAngle(ServoCenter); // Center steering

        // Infinite loop to keep robot stopped
        while (true) {}
      }
    }
  }

  // Save current obstacle state for next loop
  obstacleBefore = obstacleNow;

  // =====================================================
  //                   OLED STATUS DISPLAY
  // =====================================================

  // Clear old OLED text
  MiniR4.OLED.clearDisplay();

  // First line: show direction and number of turns
  MiniR4.OLED.setCursor(0, 0);
  MiniR4.OLED.print("D:");
  MiniR4.OLED.print(Direction);
  MiniR4.OLED.print(" T:");
  MiniR4.OLED.print(Turns);

  // Second line: show front and right distance readings
  MiniR4.OLED.setCursor(0, 10);
  MiniR4.OLED.print("F:");
  MiniR4.OLED.print(frontS);
  MiniR4.OLED.print(" R:");
  MiniR4.OLED.print(rightS);

  // Third line: show left distance reading
  MiniR4.OLED.setCursor(0, 20);
  MiniR4.OLED.print("L:");
  MiniR4.OLED.print(leftS);

  // Update OLED
  MiniR4.OLED.display();

  // Small delay to slow down loop slightly
  delay(50);
}
