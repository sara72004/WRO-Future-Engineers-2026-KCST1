# Code Explanation

Two navigation strategies were developed and tested.

## 1. Stop-Before-Turn (stop_before_turn.ino)
- The robot stops briefly before turning
- Reduces drift
- More accurate turns
- Slightly slower

## 2. Continuous Turning (continuous_turn.ino)
- The robot keeps moving while turning
- Faster movement
- May introduce slight drift

## 3. Reverse-and-Turn Navigation Strategy
This version of the code stops the robot when a wall is detected, reverses slightly, and then performs the turn. This strategy was developed to improve turning space and reduce steering errors when the robot approaches walls too closely.

## Competition Strategy
all versions are kept and tested. The final selection will be made based on performance during competition conditions.
