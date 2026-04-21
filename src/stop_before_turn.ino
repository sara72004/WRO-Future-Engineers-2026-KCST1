#include "MatrixMiniR4.h"   // Main library for the Matrix Mini R4 board

// =====================================================
//                    GLOBAL VARIABLES
// =====================================================

// Start flag:
// false = robot waits in standby
// true  = robot starts running
bool Start = false;

// Motor speed for forward driving
int MSpeed = 100;

// Servo center angle:
// this is the "straight wheels" position
float ServoCenter = 54.2;

// Number of turns completed
int Turns = 0;

// Direction of lap after first decision
// 'L' = counter-clockwise
// 'R' = clockwise
// '-' = not decided yet
char Direction = '-';


// =====================================================
//                  SENSOR READING VARIABLES
// =====================================================

// Distance measured by front laser sensor
int frontS = 0;

// Distance measured by right laser sensor
int rightS = 0;

// Distance measured by left laser sensor
int leftS  = 0;


// =====================================================
//                 OBSTACLE DETECTION STATE
// =====================================================

// Current loop obstacle status
// true  = obstacle/wall detected in front
// false = no obstacle in front
bool obstacleNow = false;

// Previous loop obstacle status
// used to detect when an obstacle appears for the first time
bool obstacleBefore = false;


// =====================================================
//                     STEERING COOLDOWN
// =====================================================

// Stores the time of the last steering action
unsigned long lastSteerTime = 0;

// Minimum time between steering actions in milliseconds
// prevents repeated triggering too quickly
const unsigned long steerCooldown = 150;


// =====================================================
//                PAUSE BEFORE TURN SETTINGS
// =====================================================

// Time to stop before making a turn
// robot pauses 1 second before steering
const int StopBeforeTurnTime = 1000;


// =====================================================
//                    TURNING SETTINGS
// =====================================================

// How far the servo turns away from center
// bigger value = sharper turn
int TurnOffset = 28;

// How long the robot keeps steering while moving forward
// this decides how large the turn becomes
int TurnTime = 1150;


// =====================================================
//          HELPER FUNCTION: LIMIT SERVO ANGLE
// =====================================================

// This function keeps the servo angle inside a safe range
// so it never goes below minAngle or above maxAngle
int clampServo(int angle, int minAngle, int maxAngle)
{
  if (angle < minAngle) return minAngle;   // If angle is too small, use minimum
  if (angle > maxAngle) return maxAngle;   // If angle is too large, use maximum
  return angle;                            // Otherwise return the original angle
}


// =====================================================
//                    TURN LEFT FUNCTION
// =====================================================

void steerLeft()
{
  // Turn front wheels left from the center position
  MiniR4.RC3.setAngle(clampServo(ServoCenter + TurnOffset, 0, 180));

  // Drive forward while wheels are turned left
  MiniR4.M1.setPower(MSpeed);

  // Keep turning for the tuned amount of time
  delay(TurnTime);

  // Re-center the front wheels after finishing the turn
  MiniR4.RC3.setAngle(ServoCenter);
}


// =====================================================
//                   TURN RIGHT FUNCTION
// =====================================================

void steerRight()
{
  // Turn front wheels right from the center position
  MiniR4.RC3.setAngle(clampServo(ServoCenter - TurnOffset, 0, 180));

  // Drive forward while wheels are turned right
  MiniR4.M1.setPower(MSpeed);

  // Keep turning for the tuned amount of time
  delay(TurnTime);

  // Re-center the front wheels after finishing the turn
  MiniR4.RC3.setAngle(ServoCenter);
}


// =====================================================
//             STOP FIRST, THEN TURN LEFT
// =====================================================

void stopThenSteerLeft()
{
  MiniR4.M1.setPower(0);           // Stop the motor completely
  MiniR4.RC3.setAngle(ServoCenter); // Make sure wheels are centered before stopping
  delay(StopBeforeTurnTime);       // Wait 1 second before turning
  steerLeft();                     // Perform the left turn
}


// =====================================================
//            STOP FIRST, THEN TURN RIGHT
// =====================================================

void stopThenSteerRight()
{
  MiniR4.M1.setPower(0);           // Stop the motor completely
  MiniR4.RC3.setAngle(ServoCenter); // Make sure wheels are centered before stopping
  delay(StopBeforeTurnTime);       // Wait 1 second before turning
  steerRight();                    // Perform the right turn
}


// =====================================================
//                        SETUP
// =====================================================

void setup()
{
  // Initialize the Matrix board and all built-in systems
  MiniR4.begin();

  // Tell the board that we are using a 2-cell battery pack
  MiniR4.PWR.setBattCell(2);

  // ---------------- Motor setup ----------------

  // Set motor direction
  // false = normal direction
  MiniR4.M1.setReverse(false);

  // Start with motor stopped
  MiniR4.M1.setPower(0);

  // Disable brake mode
  // false = motor coasts when power is removed
  MiniR4.M1.setBrake(false);

  // Reset encoder/counter of the motor
  MiniR4.M1.resetCounter();

  // ---------------- Servo setup ----------------

  // Put the steering servo in the center position at startup
  MiniR4.RC3.setAngle(ServoCenter);

  // ---------------- Laser sensor setup ----------------

  // Initialize front laser sensor on I2C2
  MiniR4.I2C2.MXLaserV2.begin();

  // Initialize right laser sensor on I2C3
  MiniR4.I2C3.MXLaserV2.begin();

  // Initialize left laser sensor on I2C4
  MiniR4.I2C4.MXLaserV2.begin();

  // ---------------- OLED startup message ----------------

  // Clear any old text on the OLED
  MiniR4.OLED.clearDisplay();

  // Print first line
  MiniR4.OLED.setCursor(0, 0);
  MiniR4.OLED.print("Robot On Standby");

  // Print second line
  MiniR4.OLED.setCursor(0, 10);
  MiniR4.OLED.print("R U Flippin' Ready?");

  // Show everything on screen
  MiniR4.OLED.display();

  // Small startup delay so message can be seen
  delay(1000);
}


// =====================================================
//                         LOOP
// =====================================================

void loop()
{
  // If the DOWN button is pressed, start the robot
  if (MiniR4.BTN_DOWN.getState())
  {
    Start = true;     // Set robot to running mode
    delay(300);       // Small debounce delay to avoid repeated press detection
  }

  // If start button was not pressed yet, do nothing
  if (!Start)
  {
    return;
  }

  // ---------------- Read all laser sensors ----------------

  // Read front distance
  frontS = MiniR4.I2C2.MXLaserV2.getDistance();

  // Read right distance
  rightS = MiniR4.I2C3.MXLaserV2.getDistance();

  // Read left distance
  leftS  = MiniR4.I2C4.MXLaserV2.getDistance();

  // ---------------- Obstacle detection ----------------

  // If front sensor sees something between 1 and 1200,
  // count it as an obstacle/wall
  obstacleNow = (frontS >= 1 && frontS <= 1200);

  // ---------------- Cooldown check ----------------

  // Only allow a new steering action if enough time passed
  bool canSteer = (millis() - lastSteerTime >= steerCooldown);

  // ---------------- Normal forward driving ----------------

  // If there is no obstacle in front:
  if (!obstacleNow)
  {
    MiniR4.M1.setPower(MSpeed);      // Keep moving forward
    MiniR4.RC3.setAngle(ServoCenter); // Keep wheels centered
  }

  // ---------------- Steering trigger logic ----------------

  // Only steer when:
  // 1. obstacle exists now
  // 2. obstacle did NOT exist in previous loop
  //    => means obstacle just appeared
  // 3. cooldown time passed
  if (obstacleNow && !obstacleBefore && canSteer)
  {
    // ---------------- First turn logic ----------------
    if (Turns == 0)
    {
      // On the very first obstacle, decide lap direction

      // If right side is closer than left side,
      // it means left side is more open
      if (rightS < leftS)
      {
        Direction = 'L';             // Set direction as counter-clockwise
        Turns = 1;                   // Count first turn
        lastSteerTime = millis();    // Save time of this turn
        stopThenSteerLeft();         // Stop, wait, then turn left
      }

      // If right side is farther than left side,
      // it means right side is more open
      else if (rightS > leftS)
      {
        Direction = 'R';             // Set direction as clockwise
        Turns = 1;                   // Count first turn
        lastSteerTime = millis();    // Save time of this turn
        stopThenSteerRight();        // Stop, wait, then turn right
      }
    }

    // ---------------- Remaining turns ----------------
    else
    {
      // Continue turning until 12 total turns are done
      if (Turns < 12)
      {
        Turns++;                     // Increase turn count
        lastSteerTime = millis();    // Save time of this steering action

        // Follow the already chosen direction
        if (Direction == 'L')
        {
          stopThenSteerLeft();
        }
        else if (Direction == 'R')
        {
          stopThenSteerRight();
        }
      }

      // When exactly 12 turns are reached, stop forever
      else if (Turns == 12)
      {
        MiniR4.M1.setPower(0);        // Stop motor
        MiniR4.RC3.setAngle(ServoCenter); // Center steering

        // Infinite loop so robot stays stopped
        while (true) {}
      }
    }
  }

  // Save current obstacle state for next loop
  // this is important to know whether obstacle is new or old
  obstacleBefore = obstacleNow;

  // =====================================================
  //                    OLED STATUS DISPLAY
  // =====================================================

  // Clear previous OLED text
  MiniR4.OLED.clearDisplay();

  // First line: show chosen direction and number of turns
  MiniR4.OLED.setCursor(0, 0);
  MiniR4.OLED.print("D:");
  MiniR4.OLED.print(Direction);
  MiniR4.OLED.print(" T:");
  MiniR4.OLED.print(Turns);

  // Second line: show front and right sensor values
  MiniR4.OLED.setCursor(0, 10);
  MiniR4.OLED.print("F:");
  MiniR4.OLED.print(frontS);
  MiniR4.OLED.print(" R:");
  MiniR4.OLED.print(rightS);

  // Third line: show left sensor value
  MiniR4.OLED.setCursor(0, 20);
  MiniR4.OLED.print("L:");
  MiniR4.OLED.print(leftS);

  // Update OLED with the new text
  MiniR4.OLED.display();

  // Small delay to reduce screen flicker and sensor spam
  delay(50);
}
