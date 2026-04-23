#include "MatrixMiniR4.h"

// ======================================================
// WRO Future Engineers Navigation Code
// Strategy:
// 1. Move forward normally
// 2. Detect wall using front laser sensor
// 3. Decide direction on first turn by comparing left and right distances
// 4. Stop, reverse slightly, then perform the turn
// 5. Keep following the same direction for the remaining turns
// ======================================================

// ===================== ROBOT STATE =====================

// Start flag: robot will remain idle until the down button is pressed
bool Start = false;

// Forward driving speed
int MSpeed = 100;

// Reverse driving speed before turning
int ReverseSpeed = -70;

// Servo center angle (straight wheels position)
int ServoCenter = 55;

// Counts how many turns have been completed
int Turns = 0;

// Stores lap direction:
// 'L' = counter-clockwise
// 'R' = clockwise
// '-' = not chosen yet
char Direction = '-';

// ===================== SENSOR READINGS =====================

// Front laser sensor distance
int frontS = 0;

// Right laser sensor distance
int rightS = 0;

// Left laser sensor distance
int leftS  = 0;

// ===================== OBSTACLE STATE =====================

// Current obstacle state
bool obstacleNow = false;

// Previous obstacle state
bool obstacleBefore = false;

// ===================== COOLDOWN =====================

// Stores the last time a steering action happened
unsigned long lastSteerTime = 0;

// Prevents repeated steering triggers from happening too quickly
const unsigned long steerCooldown = 150;

// ===================== TURN TIMING =====================

// Pause before turning left
int LeftStopBeforeTurnTime  = 1000;

// Pause before turning right
int RightStopBeforeTurnTime = 1000;

// Reverse time before turning left
int LeftReverseTime  = 1000;

// Reverse time before turning right
int RightReverseTime = 1000;

// Servo offset for left turn
int LeftTurnOffset  = 28;

// Servo offset for right turn
int RightTurnOffset = 28;

// Turn duration for left turn
int LeftTurnTime  = 1710;

// Turn duration for right turn
int RightTurnTime = 1250;

// ===================== HELPER FUNCTION =====================

// Keeps servo angle inside safe limits
int clampServo(int angle, int minAngle, int maxAngle)
{
  if (angle < minAngle) return minAngle;
  if (angle > maxAngle) return maxAngle;
  return angle;
}

// ===================== TURN FUNCTIONS =====================

// Turn left by changing servo angle, moving forward for a set time,
// then returning the servo to center
void steerLeft()
{
  MiniR4.RC3.setAngle(clampServo(ServoCenter + LeftTurnOffset, 0, 180));
  MiniR4.M1.setPower(MSpeed);
  delay(LeftTurnTime);
  MiniR4.RC3.setAngle(ServoCenter);
}

// Turn right by changing servo angle, moving forward for a set time,
// then returning the servo to center
void steerRight()
{
  MiniR4.RC3.setAngle(clampServo(ServoCenter - RightTurnOffset, 0, 180));
  MiniR4.M1.setPower(MSpeed);
  delay(RightTurnTime);
  MiniR4.RC3.setAngle(ServoCenter);
}

// ===================== COMBINED TURN SEQUENCES =====================

// Full left-turn sequence:
// 1. Stop
// 2. Pause
// 3. Reverse
// 4. Stop again
// 5. Turn left
void stopReverseThenSteerLeft()
{
  MiniR4.M1.setPower(0);
  MiniR4.RC3.setAngle(ServoCenter);
  delay(LeftStopBeforeTurnTime);

  MiniR4.M1.setPower(ReverseSpeed);
  delay(LeftReverseTime);

  MiniR4.M1.setPower(0);
  MiniR4.RC3.setAngle(ServoCenter);
  delay(150);

  steerLeft();
}

// Full right-turn sequence:
// 1. Stop
// 2. Pause
// 3. Reverse
// 4. Stop again
// 5. Turn right
void stopReverseThenSteerRight()
{
  MiniR4.M1.setPower(0);
  MiniR4.RC3.setAngle(ServoCenter);
  delay(RightStopBeforeTurnTime);

  MiniR4.M1.setPower(ReverseSpeed);
  delay(RightReverseTime);

  MiniR4.M1.setPower(0);
  MiniR4.RC3.setAngle(ServoCenter);
  delay(150);

  steerRight();
}

// ===================== SETUP =====================

void setup()
{
  // Initialize MATRIX Mini R4 board
  MiniR4.begin();

  // Tell the board that a 2-cell battery is being used
  MiniR4.PWR.setBattCell(2);

  // ---------- Motor setup ----------
  MiniR4.M1.setReverse(false);   // keep motor normal direction
  MiniR4.M1.setPower(0);         // start with motor stopped
  MiniR4.M1.setBrake(false);     // allow free rolling when motor is off
  MiniR4.M1.resetCounter();      // reset motor encoder count

  // ---------- Servo setup ----------
  MiniR4.RC3.setAngle(ServoCenter);  // center steering at startup

  // ---------- Laser sensor setup ----------
  MiniR4.I2C2.MXLaserV2.begin();   // Front laser sensor
  MiniR4.I2C3.MXLaserV2.begin();   // Right laser sensor
  MiniR4.I2C4.MXLaserV2.begin();   // Left laser sensor

  // ---------- OLED startup message ----------
  MiniR4.OLED.clearDisplay();
  MiniR4.OLED.setCursor(0, 0);
  MiniR4.OLED.print("Robot On Standby");
  MiniR4.OLED.setCursor(0, 10);
  MiniR4.OLED.print("R U Flippin' Ready?");
  MiniR4.OLED.display();

  delay(1000);
}

// ===================== MAIN LOOP =====================

void loop()
{
  // Start the robot only after the DOWN button is pressed
  if (MiniR4.BTN_DOWN.getState())
  {
    Start = true;
    delay(300); // debounce delay
  }

  // Do nothing until robot is started
  if (!Start)
  {
    return;
  }

  // ---------- Read laser sensors ----------
  frontS = MiniR4.I2C2.MXLaserV2.getDistance();
  rightS = MiniR4.I2C3.MXLaserV2.getDistance();
  leftS  = MiniR4.I2C4.MXLaserV2.getDistance();

  // ---------- Detect if a wall/obstacle is in front ----------
  // If front distance is between 1 and 650, treat it as obstacle detected
  obstacleNow = (frontS >= 1 && frontS <= 650);

  // ---------- Check steering cooldown ----------
  bool canSteer = (millis() - lastSteerTime >= steerCooldown);

  // ---------- Normal forward movement ----------
  // If there is no obstacle, keep driving forward with wheels centered
  if (!obstacleNow)
  {
    MiniR4.M1.setPower(MSpeed);
    MiniR4.RC3.setAngle(ServoCenter);
  }

  // ---------- Trigger turn only when obstacle first appears ----------
  // This prevents repeated steering while the same wall is still detected
  if (obstacleNow && !obstacleBefore && canSteer)
  {
    // ---------- First turn decides lap direction ----------
    if (Turns == 0)
    {
      if (rightS < leftS)
      {
        // Right side is closer, so left side is more open
        // Choose left turn and lock direction to counter-clockwise
        Direction = 'L';
        Turns = 1;
        lastSteerTime = millis();
        stopReverseThenSteerLeft();
      }
      else if (rightS > leftS)
      {
        // Left side is closer, so right side is more open
        // Choose right turn and lock direction to clockwise
        Direction = 'R';
        Turns = 1;
        lastSteerTime = millis();
        stopReverseThenSteerRight();
      }
    }
    else
    {
      // ---------- Continue following chosen lap direction ----------
      if (Turns < 12)
      {
        Turns++;
        lastSteerTime = millis();

        if (Direction == 'L')
        {
          stopReverseThenSteerLeft();

          // Stop robot after the final turn
          if (Turns == 12)
          {
            delay(1000);
            MiniR4.M1.setPower(0);
            MiniR4.RC3.setAngle(ServoCenter);
            while (true) {}
          }
        }
        else if (Direction == 'R')
        {
          stopReverseThenSteerRight();

          // Stop robot after the final turn
          if (Turns == 12)
          {
            delay(1000);
            MiniR4.M1.setPower(0);
            MiniR4.RC3.setAngle(ServoCenter);
            while (true) {}
          }
        }
      }
    }
  }

  // Save current obstacle state for next loop cycle
  obstacleBefore = obstacleNow;

  // ===================== OLED STATUS DISPLAY =====================

  MiniR4.OLED.clearDisplay();

  // Show chosen direction and turn count
  MiniR4.OLED.setCursor(0, 0);
  MiniR4.OLED.print("D:");
  MiniR4.OLED.print(Direction);
  MiniR4.OLED.print(" T:");
  MiniR4.OLED.print(Turns);

  // Show front and right laser readings
  MiniR4.OLED.setCursor(0, 10);
  MiniR4.OLED.print("F:");
  MiniR4.OLED.print(frontS);
  MiniR4.OLED.print(" R:");
  MiniR4.OLED.print(rightS);

  // Show left laser reading
  MiniR4.OLED.setCursor(0, 20);
  MiniR4.OLED.print("L:");
  MiniR4.OLED.print(leftS);

  MiniR4.OLED.display();

  // Small delay to stabilize display updates
  delay(50);
}
