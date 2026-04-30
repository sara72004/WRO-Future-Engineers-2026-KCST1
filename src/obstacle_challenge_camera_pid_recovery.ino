#include "MatrixMiniR4.h"

// =====================================================
// WRO FE Obstacle Challenge - Robust + Reverse Recovery
// Matrix Mini R4
//
// Camera packet expected (Serial1):
// seen,color,cx,cy,w,h,score,zone
// color: 1=RED, 2=GREEN
//
// Priority:
// 1) Reverse recovery (safety + reposition)
// 2) Camera obstacle passing (RED pass-right, GREEN pass-left)
// 3) Front wall obstacle-aware assist
// 4) Normal side-wall PID
// =====================================================

// ---------------- Run state ----------------
bool started = false;
bool finished = false;
bool clockwiseMode = true; // true=CW, false=CCW

// ---------------- Motor tuning ----------------
int speedStraight = 52;
int speedCorner   = 44;
int speedMin      = 36;
int speedMax      = 58;

// Reverse power (negative)
int reverseLightPower = -30;
int reverseHardPower  = -42;

// ---------------- Servo tuning ----------------
int servoCenter = 92;
int servoLeftLimit  = 124;   // larger angle => LEFT
int servoRightLimit = 60;    // smaller angle => RIGHT

float servoNow = 92.0f;
float servoTarget = 92.0f;
float servoSmoothFactor = 0.24f;
float maxServoStep = 6.0f;

// ---------------- PID tuning ----------------
float Kp = 0.23f;
float Ki = 0.00025f;
float Kd = 0.40f;

float error = 0.0f;
float prevError = 0.0f;
float integral = 0.0f;
float derivative = 0.0f;
float pid = 0.0f;

// Per-direction wall target
int targetWallDistCW  = 620;
int targetWallDistCCW = 640;

// ---------------- Distance sensors ----------------
int frontS = 0;
int rightS = 0;
int leftS  = 0;

// Filtered side distance
float rightFilt = 0.0f;
float leftFilt  = 0.0f;
bool sideFilterInit = false;
const float sideFilterAlpha = 0.35f;

// Valid ranges
int validMin = 20;
int validMax = 3000;

// Front thresholds
int frontAvoidStart = 3000;
int frontAvoidHard  = 2350;
int frontAvoidPanic = 1800;

// ---------------- Reverse recovery ----------------
bool reverseActive = false;
unsigned long reverseEndMs = 0;
int reversePowerCmd = 0;
float reverseServoTarget = 92.0f;

// Reverse timing
const unsigned long reverseLightMs = 180;
const unsigned long reverseHardMs  = 300;
const unsigned long reverseCooldownMs = 220;
unsigned long reverseCooldownUntilMs = 0;

// ---------------- Front timed assist ----------------
bool frontTurnActive = false;
unsigned long frontTurnEndMs = 0;
float frontTurnServoTarget = 92.0f;
int frontTurnSpeed = 0;
const unsigned long frontTurnDurationMsCW  = 700;
const unsigned long frontTurnDurationMsCCW = 860;

// ---------------- Camera packet ----------------
// seen,color,cx,cy,w,h,score,zone
int CamData[10];
String camLine = "";
unsigned long lastValidCameraPacketMs = 0;
const unsigned long cameraTimeoutMs = 220;

const int CAM_NONE  = 0;
const int CAM_RED   = 1;
const int CAM_GREEN = 2;

const int minCameraScore = 180;
const int minCameraHeight = 20;
const int minCameraWidth  = 6;

// Obstacle lock/guidance
bool obstacleLocked = false;
int lockedColor = CAM_NONE;
unsigned long lastObstacleSeenMs = 0;
const unsigned long obstacleLostTimeoutMs = 260;

bool obstacleGuideActive = false;
unsigned long obstacleGuideUntilMs = 0;
const unsigned long obstacleGuideHoldMs = 160;

float obstacleServoTarget = 92.0f;
int obstacleSpeed = 0;
int lastObstacleSteerCmd = 0; // -2..+2

// Pass-side targets in camera image
const int targetCxRed   = 88;   // RED pass-right => keep obstacle left in image
const int targetCxGreen = 232;  // GREEN pass-left => keep obstacle right in image
const int cxDeadband = 12;
const int cxStrongBand = 44;

// ---------------- Timing ----------------
unsigned long lastControlMs = 0;
const unsigned long controlPeriodMs = 12;

unsigned long lastBtnMs = 0;
const unsigned long btnDebounceMs = 180;

// ---------------- Lap count (BLUE only) ----------------
int TURNS = 0;
const int REQUIRED_BLUE_DETECTIONS = 12;
bool lapColorPrev = false;
unsigned long lastLapMs = 0;
const unsigned long lapCooldownMs = 2200;

enum FloorColor {
  FLOOR_UNKNOWN = 0,
  FLOOR_BLUE
};

// =====================================================
// Utilities
// =====================================================
int clampInt(int v, int mn, int mx)
{
  if (v < mn) return mn;
  if (v > mx) return mx;
  return v;
}

float clampFloat(float v, float mn, float mx)
{
  if (v < mn) return mn;
  if (v > mx) return mx;
  return v;
}

bool validDist(int d)
{
  return (d >= validMin && d <= validMax);
}

int readFrontFast() { return MiniR4.I2C2.MXLaserV2.getDistance(); }
int readRightFast() { return MiniR4.I2C3.MXLaserV2.getDistance(); }
int readLeftFast()  { return MiniR4.I2C4.MXLaserV2.getDistance(); }

void setMotor(int pwr)
{
  MiniR4.M1.setPower(clampInt(pwr, -100, 100));
}

void setServoImmediate(int angle)
{
  angle = clampInt(angle, servoRightLimit, servoLeftLimit);
  MiniR4.RC3.setAngle(angle);
}

void updateServoSmooth()
{
  float diff = servoTarget - servoNow;
  if (diff > maxServoStep) diff = maxServoStep;
  if (diff < -maxServoStep) diff = -maxServoStep;

  servoNow += diff;
  servoNow += (servoTarget - servoNow) * servoSmoothFactor;
  servoNow = clampFloat(servoNow, (float)servoRightLimit, (float)servoLeftLimit);

  MiniR4.RC3.setAngle((int)(servoNow + 0.5f));
}

void stopRobot()
{
  setMotor(0);
  servoTarget = (float)servoCenter;
  servoNow = (float)servoCenter;
  setServoImmediate(servoCenter);
}

void resetController()
{
  error = 0.0f;
  prevError = 0.0f;
  integral = 0.0f;
  derivative = 0.0f;
  pid = 0.0f;

  servoTarget = (float)servoCenter;
  servoNow = (float)servoCenter;

  frontTurnActive = false;
  obstacleGuideActive = false;
  obstacleLocked = false;
  lockedColor = CAM_NONE;
  reverseActive = false;
  reverseCooldownUntilMs = 0;
  lastObstacleSteerCmd = 0;

  sideFilterInit = false;
  rightFilt = 0.0f;
  leftFilt = 0.0f;
}

void drawStandby()
{
  MiniR4.OLED.clearDisplay();
  MiniR4.OLED.setCursor(0, 0);
  MiniR4.OLED.print("Obstacle Challenge");
  MiniR4.OLED.setCursor(0, 10);
  MiniR4.OLED.print("UP: Toggle Dir");
  MiniR4.OLED.setCursor(0, 20);
  MiniR4.OLED.print("DOWN: Start ");
  MiniR4.OLED.print(clockwiseMode ? "CW" : "CCW");
  MiniR4.OLED.display();
}

void finishRun()
{
  delay(1000);
  finished = true;
  stopRobot();
  while (true) {}
}

// =====================================================
// Floor color (blue lap marker)
// =====================================================
FloorColor readFloorColorForLap()
{
  int r = MiniR4.I2C1.MXColorV3.getR();
  int g = MiniR4.I2C1.MXColorV3.getG();
  int b = MiniR4.I2C1.MXColorV3.getB();

  int rgDiff = abs(r - g);
  int rbDiff = abs(r - b);
  int gbDiff = abs(g - b);

  if (rgDiff < 18 && rbDiff < 18 && gbDiff < 18) return FLOOR_UNKNOWN;
  if (b > 100 && b > r && b > g) return FLOOR_BLUE;
  return FLOOR_UNKNOWN;
}

// =====================================================
// Camera parsing
// =====================================================
void clearCamData()
{
  for (int i = 0; i < 10; i++) CamData[i] = 0;
}

bool parseCamPacket(String line)
{
  line.trim();

  int values[8];
  int idx = 0;
  int start = 0;

  while (idx < 8) {
    int comma = line.indexOf(',', start);
    if (comma == -1) {
      values[idx++] = line.substring(start).toInt();
      break;
    }
    values[idx++] = line.substring(start, comma).toInt();
    start = comma + 1;
  }

  if (idx != 8) return false;

  for (int i = 0; i < 8; i++) CamData[i] = values[i];
  lastValidCameraPacketMs = millis();
  return true;
}

void readCameraPacket()
{
  while (Serial1.available()) {
    char c = (char)Serial1.read();

    if (c == '\n') {
      if (!parseCamPacket(camLine)) clearCamData();
      camLine = "";
    } else if (c != '\r') {
      camLine += c;
      if (camLine.length() > 64) {
        camLine = "";
        clearCamData();
      }
    }
  }
}

bool cameraPacketFresh()
{
  return (millis() - lastValidCameraPacketMs) <= cameraTimeoutMs;
}

// =====================================================
// Obstacle guidance
// =====================================================

// return -2,-1,0,+1,+2
int obstacleSteerCmdFromCx(int colorId, int cx)
{
  int targetCx = 160;
  if (colorId == CAM_RED) targetCx = targetCxRed;
  else if (colorId == CAM_GREEN) targetCx = targetCxGreen;
  else return 0;

  int e = cx - targetCx;

  if (abs(e) <= cxDeadband) return 0;
  if (e > cxStrongBand) return +2;
  if (e > cxDeadband)   return +1;
  if (e < -cxStrongBand) return -2;
  return -1;
}

// +cmd => RIGHT, -cmd => LEFT
float servoTargetFromSteerCmd(int cmd)
{
  float kLight = 170.0f;
  float kStrong = 320.0f;

  float k = 0.0f;
  if (abs(cmd) == 1) k = kLight;
  if (abs(cmd) >= 2) k = kStrong;

  float t = (float)servoCenter - (k * (float)cmd);
  return clampFloat(t, (float)servoRightLimit, (float)servoLeftLimit);
}

int obstacleSpeedFromCmd(int cmd, bool frontValid, int fDist)
{
  int a = abs(cmd);
  int s = speedStraight;
  if (a == 1) s = 40;
  if (a >= 2) s = 32;

  if (frontValid && fDist < frontAvoidHard)  s -= 4;
  if (frontValid && fDist < frontAvoidPanic) s -= 6;

  return clampInt(s, 26, speedMax);
}

// =====================================================
// Wall PID
// =====================================================
void updateFilteredSides(bool rightValid, bool leftValid)
{
  if (!sideFilterInit) {
    rightFilt = rightValid ? (float)rightS : 0.0f;
    leftFilt  = leftValid  ? (float)leftS  : 0.0f;
    sideFilterInit = true;
    return;
  }

  if (rightValid) rightFilt = rightFilt + sideFilterAlpha * ((float)rightS - rightFilt);
  if (leftValid)  leftFilt  = leftFilt  + sideFilterAlpha * ((float)leftS  - leftFilt);
}

float computePidServoTarget(bool followRightWall, int activeTargetWallDist, float dt, bool rightValid, bool leftValid)
{
  updateFilteredSides(rightValid, leftValid);

  if (followRightWall) {
    if (rightValid)      error = (float)activeTargetWallDist - rightFilt;
    else if (leftValid)  error = leftFilt - (float)activeTargetWallDist;
    else                 error = prevError * 0.7f;
  } else {
    if (leftValid)       error = leftFilt - (float)activeTargetWallDist;
    else if (rightValid) error = (float)activeTargetWallDist - rightFilt;
    else                 error = prevError * 0.7f;
  }

  if (error > -260.0f && error < 260.0f) integral += error * dt;
  integral = clampFloat(integral, -180.0f, 180.0f);

  derivative = (error - prevError) / dt;
  derivative = clampFloat(derivative, -900.0f, 900.0f);

  pid = Kp * error + Ki * integral + Kd * derivative;
  prevError = error;

  float pidTarget = (float)servoCenter + pid;
  return clampFloat(pidTarget, (float)servoRightLimit, (float)servoLeftLimit);
}

int pidSpeedFromTarget(float target)
{
  int desiredSpeed = speedStraight;
  int steerAmount = abs((int)(target - (float)servoCenter));

  if (steerAmount > 12) desiredSpeed -= 4;
  if (steerAmount > 20) desiredSpeed -= 4;
  if (steerAmount > 28) desiredSpeed -= 5;

  return clampInt(desiredSpeed, speedMin, speedMax);
}

// =====================================================
// Reverse decision
// =====================================================
bool reverseAvailable(unsigned long nowMs)
{
  return (!reverseActive && nowMs >= reverseCooldownUntilMs);
}

void startReverse(unsigned long nowMs, int powerCmd, unsigned long durationMs, float steerTarget)
{
  reverseActive = true;
  reversePowerCmd = powerCmd;
  reverseEndMs = nowMs + durationMs;
  reverseServoTarget = clampFloat(steerTarget, (float)servoRightLimit, (float)servoLeftLimit);

  // cancel other transient modes so recovery is decisive
  frontTurnActive = false;
  obstacleGuideActive = false;
}

// =====================================================
// Setup / Loop
// =====================================================
void setup()
{
  MiniR4.begin();
  MiniR4.PWR.setBattCell(2);

  MiniR4.M1.setReverse(false);
  MiniR4.M1.setPower(0);
  MiniR4.M1.setBrake(false);
  MiniR4.M1.resetCounter();

  setServoImmediate(servoCenter);

  MiniR4.I2C1.MXColorV3.begin(); // floor
  MiniR4.I2C2.MXLaserV2.begin(); // front
  MiniR4.I2C3.MXLaserV2.begin(); // right
  MiniR4.I2C4.MXLaserV2.begin(); // left

  Serial1.begin(115200);
  clearCamData();

  drawStandby();
}

void loop()
{
  if (finished) return;

  readCameraPacket();

  // -------- pre-start --------
  if (!started) {
    unsigned long now = millis();

    if (MiniR4.BTN_UP.getState() && (now - lastBtnMs > btnDebounceMs)) {
      clockwiseMode = !clockwiseMode;
      lastBtnMs = now;
      drawStandby();
    }

    if (MiniR4.BTN_DOWN.getState() && (now - lastBtnMs > btnDebounceMs)) {
      started = true;
      resetController();
      lastControlMs = millis();
      lastLapMs = millis();
      lastBtnMs = now;

      MiniR4.OLED.clearDisplay();
      MiniR4.OLED.display();
      delay(120);
    } else {
      return;
    }
  }

  bool followRightWall = !clockwiseMode;
  int activeTargetWallDist = clockwiseMode ? targetWallDistCW : targetWallDistCCW;
  unsigned long activeFrontTurnDurationMs = clockwiseMode ? frontTurnDurationMsCW : frontTurnDurationMsCCW;

  unsigned long nowMs = millis();
  if (nowMs - lastControlMs < controlPeriodMs) {
    updateServoSmooth();
    return;
  }

  float dt = (nowMs - lastControlMs) / 1000.0f;
  lastControlMs = nowMs;
  if (dt <= 0.0f) dt = 0.012f;

  // read sensors
  frontS = readFrontFast();
  rightS = readRightFast();
  leftS  = readLeftFast();

  bool frontValid = validDist(frontS);
  bool rightValid = validDist(rightS);
  bool leftValid  = validDist(leftS);

  if (frontTurnActive && nowMs >= frontTurnEndMs) frontTurnActive = false;
  if (obstacleGuideActive && nowMs >= obstacleGuideUntilMs) obstacleGuideActive = false;
  if (reverseActive && nowMs >= reverseEndMs) {
    reverseActive = false;
    reverseCooldownUntilMs = nowMs + reverseCooldownMs;
  }

  // Precompute PID target
  float pidServoTarget = computePidServoTarget(followRightWall, activeTargetWallDist, dt, rightValid, leftValid);

  // Camera validity
  bool camFresh = cameraPacketFresh();
  bool camSeen = camFresh && (CamData[0] == 1);
  int camColor = camSeen ? CamData[1] : CAM_NONE;
  int camCx    = camSeen ? CamData[2] : 160;
  int camW     = camSeen ? CamData[4] : 0;
  int camH     = camSeen ? CamData[5] : 0;
  int camScore = camSeen ? CamData[6] : 0;

  bool camValid =
      camSeen &&
      (camColor == CAM_RED || camColor == CAM_GREEN) &&
      (camScore >= minCameraScore) &&
      (camW >= minCameraWidth) &&
      (camH >= minCameraHeight);

  // maintain obstacle lock
  if (camValid) {
    obstacleLocked = true;
    lockedColor = camColor;
    lastObstacleSeenMs = nowMs;

    int cmd = obstacleSteerCmdFromCx(lockedColor, camCx);
    lastObstacleSteerCmd = cmd;

    if (cmd != 0) {
      obstacleServoTarget = servoTargetFromSteerCmd(cmd);
      obstacleSpeed = obstacleSpeedFromCmd(cmd, frontValid, frontS);
      obstacleGuideActive = true;
      obstacleGuideUntilMs = nowMs + obstacleGuideHoldMs;
    } else {
      obstacleGuideActive = false;
    }
  } else {
    if (obstacleLocked && (nowMs - lastObstacleSeenMs > obstacleLostTimeoutMs)) {
      obstacleLocked = false;
      lockedColor = CAM_NONE;
      obstacleGuideActive = false;
      lastObstacleSteerCmd = 0;
    }
  }

  bool frontHard  = frontValid && (frontS < frontAvoidHard);
  bool frontPanic = frontValid && (frontS < frontAvoidPanic);

  // =====================================================
  // 1) REVERSE RECOVERY (highest)
  // =====================================================
  // Trigger if panic close OR very strong obstacle correction while front is close
  if (!reverseActive && reverseAvailable(nowMs)) {
    bool needHardReverse = frontPanic;
    bool needAssistReverse = (obstacleGuideActive && abs(lastObstacleSteerCmd) >= 2 && frontValid && frontS < frontAvoidHard);

    if (needHardReverse || needAssistReverse) {
      float steerDuringReverse = (float)servoCenter;

      // If obstacle is known, reverse while biasing toward correct pass side
      if (obstacleLocked && (lockedColor == CAM_RED || lockedColor == CAM_GREEN)) {
        if (lockedColor == CAM_RED) {
          // RED pass-right => bias RIGHT in reverse too
          steerDuringReverse = (float)servoCenter - 220.0f;
        } else {
          // GREEN pass-left => bias LEFT
          steerDuringReverse = (float)servoCenter + 220.0f;
        }
      } else {
        // fallback: steer away from followed side
        steerDuringReverse = followRightWall ? ((float)servoCenter + 180.0f) : ((float)servoCenter - 180.0f);
      }

      if (needHardReverse) {
        startReverse(nowMs, reverseHardPower, reverseHardMs, steerDuringReverse);
      } else {
        startReverse(nowMs, reverseLightPower, reverseLightMs, steerDuringReverse);
      }
    }
  }

  if (reverseActive) {
    servoTarget = reverseServoTarget;
    setMotor(reversePowerCmd);

    float savedSmooth = servoSmoothFactor;
    float savedStep = maxServoStep;
    servoSmoothFactor = 0.30f;
    maxServoStep = 8.0f;
    updateServoSmooth();
    servoSmoothFactor = savedSmooth;
    maxServoStep = savedStep;
  }

  // =====================================================
  // 2) CAMERA GUIDANCE
  // =====================================================
  else if (obstacleGuideActive && !frontPanic) {
    // blend with PID so wall discipline stays
    servoTarget = 0.75f * obstacleServoTarget + 0.25f * pidServoTarget;
    servoTarget = clampFloat(servoTarget, (float)servoRightLimit, (float)servoLeftLimit);
    setMotor(obstacleSpeed);

    float savedSmooth = servoSmoothFactor;
    float savedStep = maxServoStep;
    servoSmoothFactor = 0.30f;
    maxServoStep = 7.5f;
    updateServoSmooth();
    servoSmoothFactor = savedSmooth;
    maxServoStep = savedStep;
  }

  // =====================================================
  // 3) FRONT ASSIST (obstacle-aware)
  // =====================================================
  else if (frontTurnActive) {
    servoTarget = clampFloat(frontTurnServoTarget, (float)servoRightLimit, (float)servoLeftLimit);
    setMotor(clampInt(frontTurnSpeed, 26, speedMax));

    float savedSmooth = servoSmoothFactor;
    float savedStep = maxServoStep;
    servoSmoothFactor = 0.32f;
    maxServoStep = 8.0f;
    updateServoSmooth();
    servoSmoothFactor = savedSmooth;
    maxServoStep = savedStep;
  }
  else if (frontValid && frontS < frontAvoidStart) {
    float k;
    if (frontS < frontAvoidPanic)      k = 340.0f;
    else if (frontS < frontAvoidHard)  k = 250.0f;
    else                               k = 150.0f;

    if (obstacleLocked && (lockedColor == CAM_RED || lockedColor == CAM_GREEN)) {
      // keep obstacle passing intent
      if (lockedColor == CAM_RED) frontTurnServoTarget = (float)servoCenter - k; // right
      else                        frontTurnServoTarget = (float)servoCenter + k; // left
    } else {
      if (followRightWall) frontTurnServoTarget = (float)servoCenter + k;
      else                 frontTurnServoTarget = (float)servoCenter - k;
    }

    frontTurnServoTarget = clampFloat(frontTurnServoTarget, (float)servoRightLimit, (float)servoLeftLimit);

    int s = speedCorner;
    if (frontS < frontAvoidHard)  s = 34;
    if (frontS < frontAvoidPanic) s = 28;
    frontTurnSpeed = clampInt(s, 24, speedMax);

    frontTurnActive = true;
    frontTurnEndMs = nowMs + activeFrontTurnDurationMs;

    servoTarget = frontTurnServoTarget;
    setMotor(frontTurnSpeed);

    float savedSmooth = servoSmoothFactor;
    float savedStep = maxServoStep;
    servoSmoothFactor = 0.32f;
    maxServoStep = 8.0f;
    updateServoSmooth();
    servoSmoothFactor = savedSmooth;
    maxServoStep = savedStep;
  }

  // =====================================================
  // 4) NORMAL WALL PID
  // =====================================================
  else {
    servoTarget = pidServoTarget;
    setMotor(pidSpeedFromTarget(servoTarget));
    updateServoSmooth();
  }

  // =====================================================
  // Blue lap counting
  // =====================================================
  FloorColor floorNow = readFloorColorForLap();
  bool blueNow = (floorNow == FLOOR_BLUE);

  if (blueNow && !lapColorPrev) {
    if (millis() - lastLapMs > lapCooldownMs) {
      TURNS++;
      lastLapMs = millis();
    }
  }
  lapColorPrev = blueNow;

  if (TURNS >= REQUIRED_BLUE_DETECTIONS) finishRun();
}
