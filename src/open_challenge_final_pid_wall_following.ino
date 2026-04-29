#include "MatrixMiniR4.h"

/*
  =====================================================
  WRO Future Engineers 2026 - Open Challenge Final Code
  Robot Name: Blaze
  Board: MATRIX Mini R4

  Strategy:
  - Robot starts only when DOWN button is pressed.
  - Default direction is clockwise (CW).
  - UP button before start toggles between CW and CCW.
  - CW mode follows the LEFT wall.
  - CCW mode follows the RIGHT wall.
  - Laser sensors are used for wall following and front wall avoidance.
  - Color sensor detects BLUE markers for lap/turn counting.
  - OLED is only used before start, then cleared for faster performance.

  Main control methods:
  - Single-wall PID following for smooth driving.
  - Timed front-wall steering override for corners.
  - Smooth servo movement to reduce sudden steering changes.
  =====================================================
*/

// =====================================================
// START / RUN STATE
// =====================================================

// false = waiting for start button
// true  = robot is running
bool started = false;

// false = robot still running
// true  = run completed
bool finished = false;

// true  = clockwise mode
// false = counter-clockwise mode
bool clockwiseMode = true;


// =====================================================
// MOTOR TUNING
// =====================================================

// Normal straight driving speed
int speedStraight = 82;

// Speed used during corner/front-wall avoidance
int speedCorner = 68;

// Minimum allowed motor speed
int speedMin = 75;

// Maximum allowed motor speed
int speedMax = 90;

// Stores the current motor power after clamping
int motorPower = 0;


// =====================================================
// SERVO TUNING
// =====================================================

// Servo center angle: wheels straight
int servoCenter = 92;

// Maximum left steering limit
int servoLeftLimit = 124;

// Maximum right steering limit
int servoRightLimit = 60;

// Current servo position used for smoothing
float servoNow = 92.0f;

// Target servo position calculated by PID or front-wall override
float servoTarget = 92.0f;

// Smooth factor controls how gradually the servo moves toward the target
float servoSmoothFactor = 0.18f;

// Maximum servo movement step per update
float maxServoStep = 5.5f;


// =====================================================
// PID TUNING
// =====================================================

// Proportional gain: reacts to current wall distance error
float Kp = 0.28f;

// Integral gain: corrects small long-term error
float Ki = 0.0004f;

// Derivative gain: reduces overshoot by reacting to error change
float Kd = 0.75f;

// Current wall distance error
float error = 0.0f;

// Previous loop error
float prevError = 0.0f;

// Accumulated error
float integral = 0.0f;

// Rate of change of error
float derivative = 0.0f;

// Final PID output used to adjust servo angle
float pid = 0.0f;

// Wall distance target for clockwise mode
// In CW mode, the robot follows the LEFT wall
int targetWallDistCW = 550;

// Wall distance target for counter-clockwise mode
// In CCW mode, the robot follows the RIGHT wall
int targetWallDistCCW = 700;


// =====================================================
// SENSOR VALUES
// =====================================================

// Front laser sensor reading
int frontS = 0;

// Right laser sensor reading
int rightS = 0;

// Left laser sensor reading
int leftS = 0;


// =====================================================
// FRONT WALL AVOIDANCE THRESHOLDS
// =====================================================

// Front wall detection starts below this value
int frontAvoidStart = 2750;

// Stronger avoidance below this value
int frontAvoidHard = 2000;

// Panic/very-close avoidance below this value
int frontAvoidPanic = 1500;


// =====================================================
// TIMED FRONT TURN STATE
// =====================================================

// true while robot is performing a timed corner turn
bool frontTurnActive = false;

// Time when timed front turn should end
unsigned long frontTurnEndMs = 0;

// Servo target during front-wall avoidance
float frontTurnServoTarget = 92.0f;

// Motor speed during front-wall avoidance
int frontTurnSpeed = 0;

// Timed front turn duration for CW mode
const unsigned long frontTurnDurationMsCW = 600;

// Timed front turn duration for CCW mode
const unsigned long frontTurnDurationMsCCW = 850;


// =====================================================
// VALID SENSOR RANGE
// =====================================================

// Minimum valid laser reading
int validMin = 20;

// Maximum valid laser reading
int validMax = 3000;


// =====================================================
// TIMING
// =====================================================

// Stores last time control loop was executed
unsigned long lastControlMs = 0;

// Control loop period in milliseconds
// 10 ms = fast response
const unsigned long controlPeriodMs = 10;


// =====================================================
// BUTTON DEBOUNCE
// =====================================================

// Stores last button press time
unsigned long lastBtnMs = 0;

// Prevents repeated button triggers
const unsigned long btnDebounceMs = 180;


// =====================================================
// LAP COUNTING USING BLUE LINE
// =====================================================

// Number of detected blue markers
int TURNS = 0;

// Number of required blue detections before stopping
const int REQUIRED_BLUE_DETECTIONS = 12;

// Stores previous color detection state
bool lapColorPrev = false;

// Last time a blue marker was counted
unsigned long lastLapMs = 0;

// Minimum time between blue detections
// Prevents counting the same line multiple times
const unsigned long lapCooldownMs = 2000;


// =====================================================
// FLOOR COLOR ENUM
// =====================================================

enum FloorColor
{
  FLOOR_UNKNOWN = 0,
  FLOOR_BLUE
};


// =====================================================
// HELPER FUNCTIONS
// =====================================================

// Clamps an integer between minimum and maximum values
int clampInt(int v, int mn, int mx)
{
  if (v < mn) return mn;
  if (v > mx) return mx;
  return v;
}

// Clamps a float between minimum and maximum values
float clampFloat(float v, float mn, float mx)
{
  if (v < mn) return mn;
  if (v > mx) return mx;
  return v;
}


// =====================================================
// FAST SENSOR READ FUNCTIONS
// =====================================================

// Read front laser sensor
int readFrontFast()
{
  return MiniR4.I2C2.MXLaserV2.getDistance();
}

// Read right laser sensor
int readRightFast()
{
  return MiniR4.I2C3.MXLaserV2.getDistance();
}

// Read left laser sensor
int readLeftFast()
{
  return MiniR4.I2C4.MXLaserV2.getDistance();
}

// Checks whether laser reading is within valid range
bool validDist(int d)
{
  return (d >= validMin && d <= validMax);
}


// =====================================================
// BLUE FLOOR COLOR DETECTION
// =====================================================

// Reads RGB values from color sensor and detects blue only
FloorColor readFloorColorForLap()
{
  int r = MiniR4.I2C1.MXColorV3.getR();
  int g = MiniR4.I2C1.MXColorV3.getG();
  int b = MiniR4.I2C1.MXColorV3.getB();

  // Reject neutral/white floor by checking if RGB values are too similar
  int rgDiff = abs(r - g);
  int rbDiff = abs(r - b);
  int gbDiff = abs(g - b);

  if (rgDiff < 18 && rbDiff < 18 && gbDiff < 18)
  {
    return FLOOR_UNKNOWN;
  }

  // Detect blue when blue value is strong and greater than red and green
  if (b > 100 && b > r && b > g)
  {
    return FLOOR_BLUE;
  }

  return FLOOR_UNKNOWN;
}


// =====================================================
// MOTOR AND SERVO CONTROL
// =====================================================

// Sets motor power after limiting it between -100 and 100
void setMotor(int pwr)
{
  motorPower = clampInt(pwr, -100, 100);
  MiniR4.M1.setPower(motorPower);
}

// Immediately sets servo angle within safe steering limits
void setServoImmediate(int angle)
{
  angle = clampInt(angle, servoRightLimit, servoLeftLimit);
  MiniR4.RC3.setAngle(angle);
}

// Smoothly moves servo toward target angle
// This avoids sudden steering movements and improves stability
void updateServoSmooth()
{
  float diff = servoTarget - servoNow;

  // Limit maximum servo step
  if (diff > maxServoStep) diff = maxServoStep;
  if (diff < -maxServoStep) diff = -maxServoStep;

  // Move servo gradually toward target
  servoNow += diff;
  servoNow += (servoTarget - servoNow) * servoSmoothFactor;

  // Keep servo within safe range
  servoNow = clampFloat(servoNow, (float)servoRightLimit, (float)servoLeftLimit);

  // Send final angle to servo
  MiniR4.RC3.setAngle((int)(servoNow + 0.5f));
}

// Stops motor and centers steering
void stopRobot()
{
  setMotor(0);
  servoTarget = servoCenter;
  servoNow = servoCenter;
  setServoImmediate(servoCenter);
}

// Resets PID values and front turn state before starting run
void resetController()
{
  error = 0.0f;
  prevError = 0.0f;
  integral = 0.0f;
  derivative = 0.0f;
  pid = 0.0f;

  servoNow = (float)servoCenter;
  servoTarget = (float)servoCenter;

  frontTurnActive = false;
  frontTurnEndMs = 0;
}


// =====================================================
// OLED DISPLAY BEFORE START
// =====================================================

// Displays direction selection screen before run starts
void drawStandby()
{
  MiniR4.OLED.clearDisplay();

  MiniR4.OLED.setCursor(0, 0);
  MiniR4.OLED.print("Open Challenge");

  MiniR4.OLED.setCursor(0, 10);
  MiniR4.OLED.print("UP: Toggle Dir");

  MiniR4.OLED.setCursor(0, 20);
  MiniR4.OLED.print("DOWN: Start ");
  MiniR4.OLED.print(clockwiseMode ? "CW" : "CCW");

  MiniR4.OLED.display();
}


// =====================================================
// FINISH RUN
// =====================================================

// Stops the robot after required blue detections
void finishRun()
{
  delay(1250);
  finished = true;
  stopRobot();

  // Keep robot stopped forever
  while (true) {}
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
  // Initialize MATRIX Mini R4
  MiniR4.begin();

  // Set battery type to 2-cell
  MiniR4.PWR.setBattCell(2);

  // Motor setup
  MiniR4.M1.setReverse(false);
  MiniR4.M1.setPower(0);
  MiniR4.M1.setBrake(false);
  MiniR4.M1.resetCounter();

  // Center servo at startup
  setServoImmediate(servoCenter);

  // Initialize sensors
  MiniR4.I2C1.MXColorV3.begin();   // Color sensor
  MiniR4.I2C2.MXLaserV2.begin();   // Front laser sensor
  MiniR4.I2C3.MXLaserV2.begin();   // Right laser sensor
  MiniR4.I2C4.MXLaserV2.begin();   // Left laser sensor

  // Show standby screen
  drawStandby();
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  // Stop updating if run is already finished
  if (finished)
  {
    return;
  }

  // =====================================================
  // PRE-START MODE SELECTION
  // =====================================================

  if (!started)
  {
    unsigned long now = millis();

    // UP button toggles between CW and CCW before starting
    if (MiniR4.BTN_UP.getState() && (now - lastBtnMs > btnDebounceMs))
    {
      clockwiseMode = !clockwiseMode;
      lastBtnMs = now;
      drawStandby();
    }

    // DOWN button starts the run
    if (MiniR4.BTN_DOWN.getState() && (now - lastBtnMs > btnDebounceMs))
    {
      started = true;

      resetController();

      lastControlMs = millis();
      lastLapMs = millis();
      lastBtnMs = now;

      // Clear OLED after start to reduce display update load
      MiniR4.OLED.clearDisplay();
      MiniR4.OLED.display();

      delay(80);
    }
    else
    {
      return;
    }
  }

  // CW mode follows left wall
  // CCW mode follows right wall
  bool followRightWall = !clockwiseMode;

  // Select wall target based on direction
  int activeTargetWallDist = clockwiseMode ? targetWallDistCW : targetWallDistCCW;

  // Select front turn duration based on direction
  unsigned long activeFrontTurnDurationMs =
    clockwiseMode ? frontTurnDurationMsCW : frontTurnDurationMsCCW;

  // =====================================================
  // CONTROL LOOP TIMING
  // =====================================================

  unsigned long nowMs = millis();

  // Run main control logic every 10 ms
  // If not time yet, only update servo smoothing
  if (nowMs - lastControlMs < controlPeriodMs)
  {
    updateServoSmooth();
    return;
  }

  float dt = (nowMs - lastControlMs) / 1000.0f;
  lastControlMs = nowMs;

  if (dt <= 0.0f)
  {
    dt = 0.01f;
  }

  // =====================================================
  // READ SENSORS
  // =====================================================

  frontS = readFrontFast();
  rightS = readRightFast();
  leftS  = readLeftFast();

  bool frontValid = validDist(frontS);
  bool rightValid = validDist(rightS);
  bool leftValid  = validDist(leftS);

  // =====================================================
  // TIMED FRONT TURN STATE
  // =====================================================

  // End timed front turn after its duration expires
  if (frontTurnActive && nowMs >= frontTurnEndMs)
  {
    frontTurnActive = false;
  }

  // =====================================================
  // FRONT WALL AVOIDANCE OVERRIDE
  // =====================================================

  if (frontTurnActive)
  {
    // Continue the timed corner steering action
    servoTarget = frontTurnServoTarget;
    servoTarget = clampFloat(servoTarget, (float)servoRightLimit, (float)servoLeftLimit);

    int desiredSpeed = clampInt(frontTurnSpeed, speedMin, speedMax);
    setMotor(desiredSpeed);

    // Temporarily make servo respond faster during cornering
    float savedSmooth = servoSmoothFactor;
    float savedStep = maxServoStep;

    servoSmoothFactor = 0.35f;
    maxServoStep = 10.0f;

    updateServoSmooth();

    servoSmoothFactor = savedSmooth;
    maxServoStep = savedStep;
  }
  else if (frontValid && frontS < frontAvoidStart)
  {
    // Front wall detected, start corner avoidance

    float k;

    // Closer front wall = stronger steering correction
    if (frontS < frontAvoidPanic)
    {
      k = 360.0f;
    }
    else if (frontS < frontAvoidHard)
    {
      k = 260.0f;
    }
    else
    {
      k = 150.0f;
    }

    // CCW follows right wall, so front avoidance turns left
    // CW follows left wall, so front avoidance turns right
    if (followRightWall)
    {
      frontTurnServoTarget = (float)servoCenter + k;  // turn left
    }
    else
    {
      frontTurnServoTarget = (float)servoCenter - k;  // turn right
    }

    frontTurnServoTarget =
      clampFloat(frontTurnServoTarget, (float)servoRightLimit, (float)servoLeftLimit);

    // Reduce speed when approaching front wall
    int desiredSpeed = speedCorner;

    if (frontS < frontAvoidHard)
    {
      desiredSpeed = speedMin;
    }

    if (frontS < frontAvoidPanic)
    {
      desiredSpeed = speedMin - 5;
    }

    frontTurnSpeed = clampInt(desiredSpeed, speedMin, speedMax);

    // Activate timed front turn
    frontTurnActive = true;
    frontTurnEndMs = nowMs + activeFrontTurnDurationMs;

    servoTarget = frontTurnServoTarget;
    setMotor(frontTurnSpeed);

    // Temporarily increase servo response during corner entry
    float savedSmooth = servoSmoothFactor;
    float savedStep = maxServoStep;

    servoSmoothFactor = 0.35f;
    maxServoStep = 10.0f;

    updateServoSmooth();

    servoSmoothFactor = savedSmooth;
    maxServoStep = savedStep;
  }
  else
  {
    // =====================================================
    // NORMAL SINGLE-WALL PID FOLLOWING
    // =====================================================

    if (followRightWall)
    {
      // CCW mode: follow right wall
      if (rightValid)
      {
        error = (float)(activeTargetWallDist - rightS);
      }
      else if (leftValid)
      {
        // fallback if right sensor reading is invalid
        error = (float)(leftS - activeTargetWallDist);
      }
      else
      {
        // if both side readings are invalid, slowly decay previous error
        error = prevError * 0.7f;
      }
    }
    else
    {
      // CW mode: follow left wall
      if (leftValid)
      {
        error = (float)(leftS - activeTargetWallDist);
      }
      else if (rightValid)
      {
        // fallback if left sensor reading is invalid
        error = (float)(activeTargetWallDist - rightS);
      }
      else
      {
        // if both side readings are invalid, slowly decay previous error
        error = prevError * 0.7f;
      }
    }

    // Only accumulate integral for reasonable error values
    // This prevents integral windup from large bad readings
    if (error > -300.0f && error < 300.0f)
    {
      integral += error * dt;
    }

    integral = clampFloat(integral, -250.0f, 250.0f);

    // Calculate derivative and PID output
    derivative = (error - prevError) / dt;
    pid = Kp * error + Ki * integral + Kd * derivative;
    prevError = error;

    // Convert PID output into servo target
    servoTarget = (float)servoCenter + pid;
    servoTarget = clampFloat(servoTarget, (float)servoRightLimit, (float)servoLeftLimit);

    // Reduce speed if steering angle is large
    int desiredSpeed = speedStraight;
    int steerAmount = abs((int)(servoTarget - servoCenter));

    if (steerAmount > 18) desiredSpeed -= 5;
    if (steerAmount > 28) desiredSpeed -= 6;
    if (steerAmount > 36) desiredSpeed -= 6;

    desiredSpeed = clampInt(desiredSpeed, speedMin, speedMax);

    setMotor(desiredSpeed);
    updateServoSmooth();
  }

  // =====================================================
  // LAP COUNTING USING BLUE COLOR
  // =====================================================

  FloorColor floorNow = readFloorColorForLap();
  bool blueNow = (floorNow == FLOOR_BLUE);

  // Count only the rising edge of blue detection
  if (blueNow && !lapColorPrev)
  {
    if (millis() - lastLapMs > lapCooldownMs)
    {
      TURNS++;
      lastLapMs = millis();
    }
  }

  lapColorPrev = blueNow;

  // Stop robot after required number of blue detections
  if (TURNS >= REQUIRED_BLUE_DETECTIONS)
  {
    finishRun();
  }
}
