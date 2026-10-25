# Autonomous Calibration Template

Autonomous motion is disabled by default. This is a route template, not a
field-ready program. No optical sensor or automatic homing is configured.

## Routines

- `autonomousRed()`: toggle twice from yellow to red, reverse 50 mm, place at black
  holder spot 1, place at black holder spot 2, then place at the red holder.
- `autonomousBlue()`: the same sequence, but toggle once from yellow to blue and finish
  at the blue holder.
- `autonomousBlack()`: a separate full run with configurable toggles before reversing and
  two black-holder placements. Its toggle count and final destination must be
  configured; neither has been assumed.

## Practice Controls

Button-controlled practice is enabled by `practiceModeEnabled = true` in
[src/main.cpp](src/main.cpp). Select RED, BLUE, or BLACK on the Brain screen,
then use the controller:

1. Press A to start a 15-second autonomous period. The selected routine runs
   once; if it finishes early, motors remain stopped for the rest of the period.
2. After the 15 seconds, release and press B to start manual control for
   1 minute 45 seconds (105 seconds). Holding B during autonomous will not start it.
3. Motors stop when manual time expires. X stops practice during either period
   or while waiting to start. Restart the program and restore starting positions
   before another run. There is no automatic homing.

Team selection is locked after A. The autonomous calibration gate still applies:
if configuration is incomplete, no autonomous motion occurs, but the 15-second
period still elapses before manual control can start. Controller disconnection
during a running period stops practice. Practice refuses field/switch control
connections and stops if one is connected; it does not bypass field disable.

For official competition, set `practiceModeEnabled = false` and rebuild. Field
control then starts autonomous and manual callbacks; A/B do not switch modes.
Official period timing is controlled by the field, not these practice timers.

## Source Layout

- [src/main.cpp](src/main.cpp): hardware definitions, drivetrain dimensions,
  and competition setup.
- [src/driver_control.cpp](src/driver_control.cpp): manual controller input.
- [src/autonomous.cpp](src/autonomous.cpp): autonomous routes, calibration,
  and team selection.
- [include/robot.h](include/robot.h): shared hardware and function declarations.

## Before Enabling Motion

Configure motor ports and drivetrain dimensions in [src/main.cpp](src/main.cpp).
Configure toggles, placement paths, and the calibration gate in
[src/autonomous.cpp](src/autonomous.cpp):

1. Verify all motor ports and reversal flags with the robot raised safely.
   Existing reversal flags are unchanged. Confirm both drive sides move in
   the same physical direction when commanded forward.
2. Measure `wheelTravelMm` (wheel circumference), `trackWidthMm`, `wheelBaseMm`,
   and `externalGearRatio`. The supplied dimensions are placeholders.
3. Color toggling uses `toggleMotor`, the group containing `driveMotorLf`,
   `driveMotorLb`, `driveMotorRb`, and `driveMotorRf`.
   It shares the same motors as the drivetrain; do not command both
   at once. Set the signed drive-motor angle per toggle in `toggleDegrees`.
   Positive moves forward, negative reverses, assuming motor directions are
   verified. Each toggle is one incremental rotation of all four motors,
   not a return stroke or a button pulse. If the field toggle needs backing
   away and approaching again, adapt `toggleColor()` before enabling.
   Include the robot's movement during toggling in the first pickup's start pose.
4. Configure `blackHolderOne` and `blackHolderTwo` separately, then each required
   final holder: `redHolder`, `blueHolder`, or `blackFinalHolder`. These objects
   currently contain only zeros. Set their fields in source or initialization
   code before competition starts. Mark each completed path `calibrated = true`.
5. For Black, set `blackToggleCount` explicitly. Use zero only if no toggle is
   required. The default of -1 blocks the routine.
6. Establish and consistently reproduce the initial lift, wrist, claw, robot,
   and yellow toggle positions. Mechanism angles are absolute motor encoder
   degrees relative to their startup origins, not measured physical joint
   angles. There is no automatic reset or homing between runs.
7. Check clearance, object retention, speeds, and motion timeouts under
   supervised testing. Enable `autonomousCalibrated` only after configuration.

## Placement Coordinates

Each path begins at the preceding action's final robot pose. The first path
begins after the initial toggles and 50 mm reverse. The second begins after the
first placement's retreat, and the final path begins after the second retreat.
No automatic return to a common starting pose is assumed. The shared black
paths must be verified for every enabled routine's starting pose.

- `pickupTurnDegrees`: relative turn toward the wall pickup, positive right.
- `pickupDistanceMm`: signed travel to the object, positive forward.
- `pickupLiftDegrees`, `pickupWristDegrees`: absolute pickup mechanism positions.
- `clawOpenDegrees`, `clawClosedDegrees`: absolute release and grip positions.
- `carryingLiftDegrees`: absolute transport height after gripping.
- The robot backs out by the opposite of `pickupDistanceMm` before turning.
- `holderTurnDegrees`: relative turn from that pickup heading, positive right.
- `holderDistanceMm`: signed travel to this holder's distinct drop spot.
- `holderWristDegrees`, `holderLiftDegrees`: absolute placement positions.
- After releasing, the robot reverses by `retreatDistanceMm` (normally positive).

Each placement performs one grip and release. If pins and cups need separate
pickups or a different grasp sequence, adapt `placeAtHolder()` before enabling.
Encoder completion cannot confirm a successful grip or detect wheel slip.
Failed/timed-out movement stops the remaining sequence; lift and claw motors
are held at the end rather than homed or lowered automatically.