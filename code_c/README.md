# Autonomous Calibration Template

Autonomous motion is disabled by default. This is a route template, not a
field-ready program. No optical sensor or automatic homing is configured.

## Override scoring rules

- Autonomous Bonus: 12 points, awarded by the referee to the Alliance with the
  higher Autonomous-period score.
- Alliance-colored Pin scored in a Goal: 5 points.
- Yellow Pin scored in a Goal: 10 points for the Alliance that owns the Toggle
  in that Goal's Quadrant.
- Robot ending the Match in the contested Midfield: 8 points.

The program reports completed motor commands and a programmed pin-point estimate
after autonomous. This is not proof that a Pin entered a Goal, a Toggle changed
to the intended color, or the Robot physically reached the Midfield. Those
conditions require sensors or visual/referee confirmation. The Autonomous Bonus
and Autonomous Win Point cannot be determined by one Robot program because they
depend on field state, the opposing Alliance, and the official task definition.

## Build

The VEX Robotics extension, SDK, and toolchain are installed separately on each
computer; they are not included in the Git clone. On Windows, open the
`code_c` folder itself in VS Code, then use the Command Palette
(`Ctrl+Shift+P`) to run **VEX | Project: Download Toolchain** and
**VEX | Project: Download SDK For Open Project** if needed. Build with
**VEX | Project: C++ Build**.

On macOS, you can also build from the repository root with:

```sh
make -C code_c
```

The makefile automatically discovers the extension's SDK and ARM toolchain on
macOS. To use a different installation, pass the SDK root with `T` and the
directory containing the ARM tools with `VEX_TOOLCHAIN_BIN`:

```sh
make -C code_c T="/path/to/sdk/root" VEX_TOOLCHAIN_BIN="/path/to/gcc/bin"
```

The compiled binary is written to `code_c/build/code_c.bin`.

## Routines

- `autonomousRed()`: toggle once from yellow to red, reverse 50 mm, place at black
  holder spot 1, place at black holder spot 2, then place at the red holder.
- `autonomousBlue()`: the same sequence, but toggle twice from yellow to blue and finish
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
if configuration is incomplete, no autonomous motion occurs. Instead, calibration
mode lets you press B immediately to begin a 1:45 manual measurement session.
The Brain displays estimated drivetrain travel, left/right and individual drive
motor degrees, both lift positions, wrist position, and claw position. The
Controller displays the most commonly used measurements. Press Up once to zero
only the four drive-motor measurement counters before a distance test. Up does
not reset lift, wrist, or claw positions. Press X to stop the session.

Controller disconnection during a running period stops practice. Practice
refuses field/switch control connections and stops if one is connected; it does
not bypass field disable.

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
8. Set each enabled path's `scoredPin` to `ScoredPin::AllianceColor` or
   `ScoredPin::OwnedYellow`. Leave it as `None` when the route does not score a
   Pin or when Yellow-Pin quadrant ownership has not been verified.

## Current autonomous audit

As checked on October 25, 2026, autonomous is not field-ready and performs no
motion: `autonomousCalibrated` is false, toggle travel is zero, route distances
and mechanism positions are placeholders, route durations are zero, and every
route has `calibrated = false`. Midfield movement is not programmed. These gates
are intentional so placeholder values cannot unexpectedly move the Robot.

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


# How to calibrate the robot
You need to calibrate the robot one movement at a time. Keep `autonomousCalibrated = false` until every enabled movement works reliably.


## 1. Prepare a repeatable starting position

Before taking measurements:

1. Fully charge the battery.
2. Put tape on the field showing the starting wheel positions.
3. Put the lift, wrist and claw in the same physical starting positions.
4. Restart the program so all motor encoder positions begin at `0`.
5. Run every test at the same motor speeds used by autonomous.

Motor positions are absolute from the position recorded when the program starts. [VEX motor-position documentation](https://api.vex.com/v5/home/cpp/Motors_and_MotorControllers/motor_and_motor_group.html)

## 2. Drivetrain measurements

These values are in [main.cpp](/Users/jerzhan2/repos1/vex-vscode-projects/code_c/src/main.cpp).

| Parameter | How to measure |
|---|---|
| `wheelTravelMm` | Mark one drive wheel and the floor. Roll the loaded robot forward exactly five wheel rotations. Measure the distance and divide by five. |
| `trackWidthMm` | Measure from the center of the left drive wheel to the center of the corresponding right drive wheel. |
| `wheelBaseMm` | Measure from the center of the front drive wheel to the center of the rear drive wheel. |
| `externalGearRatio` | If the motor drives the wheel directly, use `1.0`. If gears are used, start with `driving gear teeth ÷ driven gear teeth`; for a 36-tooth motor gear driving a 60-tooth wheel gear, start with `0.6`, then validate with the distance test below. |

VEX defines wheel travel as wheel circumference, track width as left-to-right wheel distance, and wheelbase as front-to-back wheel distance. [VEX drivetrain documentation](https://api.vex.com/v5/home/cpp/Drivetrain/index.html)

### Distance validation

After entering the values:

1. Command the robot to drive `1000 mm`.
2. Measure its actual travel.
3. Repeat three times and average the results.
4. If it travels the wrong distance, correct the effective wheel travel:

```text
new wheelTravelMm =
    old wheelTravelMm × actual distance ÷ commanded distance
```

Example: it was commanded `1000 mm` but travelled `920 mm`:

```text
new value = old value × 920 ÷ 1000
```

### Turning validation

1. Draw a straight line beside the robot.
2. Command a `360°` turn.
3. Measure the actual rotation with a large protractor, angle application, or marked field tiles.
4. Repeat three times.

A useful first correction is:

```text
new trackWidthMm =
    old trackWidthMm × commanded angle ÷ actual angle
```

Example: commanded `360°`, actual turn `330°`:

```text
new track width = old track width × 360 ÷ 330
```

Then test `90°`, `180°`, and `360°` turns again.

## 3. Reading lift, wrist and claw positions

The mechanism values are motor encoder degrees. Add this temporary display inside the `driverControl()` loop:

```cpp
robotBrain.Screen.clearScreen();
robotBrain.Screen.printAt(
    10, 40, false,
    "Lift R: %.0f  Lift L: %.0f",
    liftMotorR.position(degrees),
    liftMotorL.position(degrees)
);
robotBrain.Screen.printAt(
    10, 80, false,
    "Wrist: %.0f",
    clawPosition.position(degrees)
);
robotBrain.Screen.printAt(
    10, 120, false,
    "Claw: %.0f",
    claw.position(degrees)
);
```

Move each mechanism slowly with the controller. When it reaches the desired physical position, record the displayed number.

Do not force a mechanism hard against a physical stop. If the two lift motors report significantly different positions, check their reversal settings and linkage before continuing.

## 4. Toggle measurements

These values are near the top of [autonomous.cpp](/Users/jerzhan2/repos1/vex-vscode-projects/code_c/src/autonomous.cpp:74).

### `toggleDegrees`

1. Place the robot at its exact autonomous starting position.
2. Approach the Toggle slowly.
3. Record the drive-motor position before contact.
4. Move until the Toggle completely changes state.
5. Record the new drive-motor position.
6. Subtract the starting position.

```text
toggleDegrees = ending motor degrees − starting motor degrees
```

Use a positive value if the robot approaches forward and a negative value if it approaches in reverse.

Test at least five times. Use enough travel to activate the Toggle reliably without pushing excessively.

### `toggleRetreatDegrees`

1. After activating the Toggle, reverse until the robot completely releases it.
2. Record the motor-position change.
3. This value must normally have the opposite sign from `toggleDegrees`.

Example:

```cpp
const double toggleDegrees = 145;
const double toggleRetreatDegrees = -90;
```

### `redToggleCount` and `blueToggleCount`

Do not calculate these from distance. Test the actual field Toggle:

1. Put it in the expected starting state.
2. Perform one complete contact-and-retreat cycle.
3. Record the resulting color.
4. Repeat until you know how many cycles produce red and blue.

Your current values are:

```cpp
const int redToggleCount = 1;
const int blueToggleCount = 2;
```

Verify these physically; the code has no color eensor.

### `initialReverseDistanceMm`

This is the distance travelled after finishing the Toggle and before starting the first pickup:

1. Complete the Toggle sequence manually.
2. Back up until the robot has enough room to begin the first pickup turn.
3. Measure the movement of the wheel axle with a tape measure.
4. Enter the positive magnitude—the code automatically reverses it.

```cpp
const double initialReverseDistanceMm = 150;
```

### `toggleCalibrated`

Set this to `true` only after the Toggle succeeds at least five consecutive times:

```cpp
const bool toggleCalibrated = true;
```

## 5. Measurements for every placement path

Repeat the following procedure separately for:

- `blackHolderOne`
- `blackHolderTwo`
- `redHolder`
- `blueHolder`
- `neutralFinalHolder`

Each path starts from where the previous path finished—not from the original match starting position.

### `pickupTurnDegrees`

Measure the turn from the path’s starting heading toward the Pin:

- Right turn: positive.
- Left turn: negative.

Example:

```cpp
path.pickupTurnDegrees = -45;
```

Test by commanding only the turn and checking whether the claw points directly at the Pin.

### `pickupDistanceMm`

After completing `pickupTurnDegrees`:

1. Measure from the drive-wheel axle’s starting position.
2. Move until the open claw is correctly positioned around the Pin.
3. Measure the axle movement.

Use positive for forward and negative for reverse.

The program automatically travels the opposite distance after gripping, so there is no separate pickup-return parameter.

### `pickupLiftDegrees`

Manually move the lift to the best pickup height and record the lift encoder value.

```cpp
path.pickupLiftDegrees = measuredLiftPosition;
```

### `pickupWristDegrees`

Move the wrist until the claw is aligned with the Pin and record `clawPosition.position(degrees)`.

### `clawOpenDegrees`

Open the claw enough to enter around the Pin without hitting it. Record `claw.position(degrees)`.

Leave a small safety clearance; do not use the mechanical maximum unless necessary.

### `clawClosedDegrees`

Close the claw until it holds the Pin securely without stalling the motor. Record the position.

Test by lifting and gently driving backward. The Pin should remain held.

### `carryingLiftDegrees`

After gripping the Pin:

1. Raise the lift enough to clear the floor and obstacles.
2. Keep the robot’s center of gravity reasonably low.
3. Record the lift position.

### `holderTurnDegrees`

After the robot automatically backs away from the pickup, measure the relative turn needed to face the destination Goal:

- Right is positive.
- Left is negative.

This is relative to the robot’s heading after backing away—not the field’s global angle.

### `holderDistanceMm`

After `holderTurnDegrees`, measure the signed distance from the wheel axle to the correct scoring position.

Start conservatively. Increase it gradually so the robot does not collide aggressively with the Goal.

### `holderLiftDegrees`

Move the lift to the height needed to place the Pin into the Goal and record its encoder position.

### `holderWristDegrees`

Move the wrist to the release angle and record its encoder position.

Test whether gravity releases the Pin cleanly when the claw opens.

### `retreatDistanceMm`

After releasing the Pin:

1. Back away until the robot clears the Goal.
2. Measure the axle movement.
3. Store this as a positive number.

The code automatically applies the reverse direction:

```cpp
driveDistance(-path.retreatDistanceMm);
```

### `estimatedDurationMs`

After the entire path works:

1. Time it from the beginning of `pickupTurnDegrees` until retreat finishes.
2. Run it at least five times.
3. Take the slowest result.
4. Add approximately 20% safety margin.

Example:

```text
Slowest test: 3.4 seconds
3.4 × 1.20 = 4.08 seconds
```

Use:

```cpp
path.estimatedDurationMs = 4100;
```

### `scoredPin`

This is selected, not measured:

```cpp
path.scoredPin = ScoredPin::AllianceColor;
```

or:

```cpp
path.scoredPin = ScoredPin::OwnedYellow;
```

Only use `OwnedYellow` when the destination Goal is in a Quadrant whose Toggle your Alliance owns.

### `path.calibrated`

Set it to `true` only after the complete route succeeds at least five consecutive times:

```cpp
path.calibrated = true;
```

## 6. Other configuration values

### `placementCount`

This determines how many paths run:

```cpp
const int placementCount = 1;
```

- `1`: `blackHolderOne`
- `2`: `blackHolderOne`, then `blackHolderTwo`
- `3`: both black-holder paths, then the selected final route

Calibrate with `1` first. Increase it only when the preceding path is reliable and the total routine fits in 15 seconds.

### `autonomousMotionBudgetMs`

The existing value is appropriate:

```cpp
const int autonomousMotionBudgetMs = 14500;
```

It reserves 500 milliseconds before the 15-second limit. If the field robot stops slowly or communication introduces delays, consider reducing it to `14000`.

### `autonomousCalibrated`

This is the final master switch:

```cpp
const bool autonomousCalibrated = true;
```

Set it to `true` only after:

- Drivetrain distance and turning are calibrated.
- Toggle operation is reliable.
- Every enabled path is calibrated.
- Total autonomous time is below the budget.
- The routine succeeds repeatedly with Pins placed in their normal starting positions.

## 7. Recommended calibration order

1. Verify motor ports and directions with the robot raised.
2. Measure wheel travel, track width, wheelbase and gear ratio.
3. Test 1000 mm driving and 360° turning.
4. Establish repeatable mechanism starting positions.
5. Record lift, wrist and claw positions.
6. Calibrate only `blackHolderOne`.
7. Set `placementCount = 1` and test it.
8. Calibrate the Toggle.
9. Calibrate additional placement paths individually.
10. Run at least ten complete 15-second tests.
11. Set `autonomousCalibrated = true`.

The autonomous report can confirm that motor commands finished, but you must visually confirm that the Pin actually entered the Goal, the Toggle reached the correct color, and the robot reached the intended position.



Yes—you can now measure safely while autonomousCalibrated = false.
I added calibration mode and successfully rebuilt the project.
How to use it:
1. Download the new program to the VEX Brain.
2. Place the mechanisms in their repeatable starting positions.
3. Start the program.
4. The Brain displays CALIBRATION MODE.
5. Press B to start the 1:45 measurement session.
6. Drive and operate the mechanisms manually.
7. Press Up once before a drive-distance test to reset the drive measurement to zero.
8. Press X to stop.
The Brain displays:
- Estimated drive distance in millimetres
- Average left and right motor degrees
- All four individual drive-motor positions
- Left and right lift positions
- Wrist position
- Claw position
The Controller displays the drive distance, lift, wrist, and claw values.
Code locations:
- Calibration startup: [main.cpp (line 85)](/Users/jerzhan2/repos1/vex-vscode-projects/code_c/src/main.cpp:85)
- Debug display: [driver_control.cpp (line 14)](/Users/jerzhan2/repos1/vex-vscode-projects/code_c/src/driver_control.cpp:14)
- Calibration flag check: [autonomous.cpp (line 76)](/Users/jerzhan2/repos1/vex-vscode-projects/code_c/src/autonomous.cpp:76)
The displayed drive distance is an estimate based on the 3.25-inch wheel and gear ratio. Compare it with a tape measure before trusting it. Lift, wrist, and claw are not reset by Up, because their absolute positions must remain referenced to their startup positions.


