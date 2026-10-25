/*
 * CALIBRATION GUIDE: WHERE TO CHANGE VALUES
 *
 * EDIT = a setting you can adjust. TODO = an unconfigured value to measure
 * or decide before enabling its routine. Both are comments, not commands:
 * change the NUMBER or BOOLEAN in the code, not the comment text.
 *
 * 1. TOP-LEVEL SETTINGS below:
 *    initialReverseDistanceMm: reverse distance AFTER the color toggles, in mm.
 *    toggleDegrees / toggleRetreatDegrees: approach and release wheel rotations
 *      for ONE contact cycle. Calibrate BOTH, with opposite signs.
 *    toggleCalibrated: set true only after checking the actual color cycle.
 *      Red currently repeats twice; Blue once from yellow: verify on the field.
 *    redToggleCount / blueToggleCount: verified contacts for the chosen alliance.
 *    placementCount: 1 by default; raise to 2 or 3 only after timed testing.
 *    autonomousMotionBudgetMs: stop scheduling motion before the 15s period ends.
 *
 * 2. NAMED ROUTE BLOCKS: edit each block's path.* assignments, not the
 *    shared defaults in struct PlacementPath:
 *      blackHolderOne = step #2, first black spot (all routines).
 *      blackHolderTwo = step #3, second black spot (all routines).
 *      redHolder / blueHolder / neutralFinalHolder = optional third placement.
 *    estimatedDurationMs: conservative measured time for this entire placement,
 *      including pickup, scoring, and retreat. Zero blocks an enabled path.
 *    DISTANCES: pickupDistanceMm, holderDistanceMm, retreatDistanceMm.
 *    TURNS: pickupTurnDegrees, holderTurnDegrees (positive right, negative left).
 *    MECHANISMS: pickupLiftDegrees, pickupWristDegrees, clawOpenDegrees,
 *      clawClosedDegrees, carryingLiftDegrees, holderLiftDegrees,
 *      holderWristDegrees (absolute motor encoder degrees).
 *    Pickup/holder travel: positive forward, negative reverse; retreat is
 *    normally positive because the code reverses it. Each route starts at
 *    the previous action's final pose. Zeros are placeholders, not calibration.
 *
 * 3. WHEN CONFIGURATION IS COMPLETE: set path.calibrated = true only for the
 *    required blocks: first black holder for 1 placement, both for 2, and the
 *    selected final holder as well for 3. Verify the goal is in the relevant
 *    toggle quadrant and the pickup contains an object that scores for you. Set
 *    autonomousCalibrated = true below. These flags permit movement; they
 *    do not measure, home, or calibrate the robot automatically.
 *
 * Hardware ports and wheel geometry are in main.cpp. Manual controls are in
 * driver_control.cpp. In main.cpp, practiceModeEnabled selects button practice:
 * A starts 15 seconds of autonomous; after it ends, B starts 1:45 of manual.
 * X stops practice. Set practiceModeEnabled = false for official field control.
 * Keep motion disabled until configuration is complete,
 * then verify the route under supervision; encoder completion cannot detect slip.
 */

#include "robot.h"

using namespace vex;

// CALIBRATION COMPLETE: set true only after hardware and all required paths
// are calibrated below. Each required path must also have calibrated = true.
const bool autonomousCalibrated = false;

// EDIT DISTANCE HERE: reverse travel after toggling, before the first pickup.
const double initialReverseDistanceMm = 50;

// Color toggling uses toggleMotor: all four driveMotor* motors together.
// TODO: signed drive-motor degrees to contact the toggle; positive is forward.
// Retreat must release the toggle before another contact. Verify the color cycle;
// encoder travel alone cannot confirm the field toggle's color.
const double toggleDegrees = 0;
const double toggleRetreatDegrees = 0;
const bool toggleCalibrated = false;
const int redToggleCount = 2; // TODO: verify on the actual toggle from its start color.
const int blueToggleCount = 1;
const int placementCount = 1; // EDIT: enable the first 1, 2, or 3 placements in order.
const int autonomousMotionBudgetMs = 14500; // Reserve 500 ms of the 15s period.
timer autonomousTimer;

struct PlacementPath {
    bool calibrated = false;
    int estimatedDurationMs = 0;
    // Travel is in mm: positive forward, negative reverse.
    // Turns are relative degrees: positive right, negative left.
    // Lift, wrist, and claw positions are absolute motor encoder degrees.
    double pickupTurnDegrees = 0;
    double pickupDistanceMm = 0;
    double pickupLiftDegrees = 0;
    double pickupWristDegrees = 0;
    double clawOpenDegrees = 0;
    double clawClosedDegrees = 0;
    double carryingLiftDegrees = 0;
    double holderTurnDegrees = 0;
    double holderDistanceMm = 0;
    double holderLiftDegrees = 0;
    double holderWristDegrees = 0;
    double retreatDistanceMm = 0;
};

// EDIT ROUTES BELOW: zeros are placeholders, not measured positions.
// Each route starts where the previous action ends, not at the match start.
// Pickup travel is automatically reversed before turning toward the holder.

// STEP #2: wall pickup -> black holder, spot 1 (shared by all teams).
PlacementPath blackHolderOne = [] {
    PlacementPath path;
    path.estimatedDurationMs = 0; // TODO: measured worst-case placement time, in ms.
    path.pickupTurnDegrees = 0;
    path.pickupDistanceMm = 0; // TODO: distance to the wall pickup.
    path.pickupLiftDegrees = 0;
    path.pickupWristDegrees = 0;
    path.clawOpenDegrees = 0;
    path.clawClosedDegrees = 0;
    path.carryingLiftDegrees = 0;
    path.holderTurnDegrees = 0;
    path.holderDistanceMm = 0; // TODO: distance to black spot 1.
    path.holderLiftDegrees = 0;
    path.holderWristDegrees = 0;
    path.retreatDistanceMm = 0; // TODO: positive distance to back away after release.
    path.calibrated = false; // Set true AFTER this entire path is calibrated.
    return path;
}();

// STEP #3: another wall pickup -> black holder, spot 2.
PlacementPath blackHolderTwo = [] {
    PlacementPath path;
    path.estimatedDurationMs = 0; // TODO: measured worst-case placement time, in ms.
    path.pickupTurnDegrees = 0;
    path.pickupDistanceMm = 0; // TODO: distance to the next wall pickup.
    path.pickupLiftDegrees = 0;
    path.pickupWristDegrees = 0;
    path.clawOpenDegrees = 0;
    path.clawClosedDegrees = 0;
    path.carryingLiftDegrees = 0;
    path.holderTurnDegrees = 0;
    path.holderDistanceMm = 0; // TODO: distance to black spot 2, not spot 1.
    path.holderLiftDegrees = 0;
    path.holderWristDegrees = 0;
    path.retreatDistanceMm = 0;
    path.calibrated = false; // Set true AFTER this entire path is calibrated.
    return path;
}();

// STEP #4, RED TEAM: wall pickup -> red holder.
PlacementPath redHolder = [] {
    PlacementPath path;
    path.estimatedDurationMs = 0; // TODO: measured worst-case placement time, in ms.
    path.pickupTurnDegrees = 0;
    path.pickupDistanceMm = 0; // TODO: distance to the final wall pickup.
    path.pickupLiftDegrees = 0;
    path.pickupWristDegrees = 0;
    path.clawOpenDegrees = 0;
    path.clawClosedDegrees = 0;
    path.carryingLiftDegrees = 0;
    path.holderTurnDegrees = 0;
    path.holderDistanceMm = 0; // TODO: distance to the red holder.
    path.holderLiftDegrees = 0;
    path.holderWristDegrees = 0;
    path.retreatDistanceMm = 0;
    path.calibrated = false; // Set true AFTER this entire path is calibrated.
    return path;
}();

// STEP #4, BLUE TEAM: wall pickup -> blue holder.
PlacementPath blueHolder = [] {
    PlacementPath path;
    path.estimatedDurationMs = 0; // TODO: measured worst-case placement time, in ms.
    path.pickupTurnDegrees = 0;
    path.pickupDistanceMm = 0; // TODO: distance to the final wall pickup.
    path.pickupLiftDegrees = 0;
    path.pickupWristDegrees = 0;
    path.clawOpenDegrees = 0;
    path.clawClosedDegrees = 0;
    path.carryingLiftDegrees = 0;
    path.holderTurnDegrees = 0;
    path.holderDistanceMm = 0; // TODO: distance to the blue holder.
    path.holderLiftDegrees = 0;
    path.holderWristDegrees = 0;
    path.retreatDistanceMm = 0;
    path.calibrated = false; // Set true AFTER this entire path is calibrated.
    return path;
}();

// OPTIONAL STEP #4, NEUTRAL ROUTE: this is a goal strategy, not a third alliance.
PlacementPath neutralFinalHolder = [] {
    PlacementPath path;
    path.estimatedDurationMs = 0; // TODO: measured worst-case placement time, in ms.
    path.pickupTurnDegrees = 0;
    path.pickupDistanceMm = 0; // TODO: distance to the final wall pickup.
    path.pickupLiftDegrees = 0;
    path.pickupWristDegrees = 0;
    path.clawOpenDegrees = 0;
    path.clawClosedDegrees = 0;
    path.carryingLiftDegrees = 0;
    path.holderTurnDegrees = 0;
    path.holderDistanceMm = 0; // TODO: distance to the chosen neutral goal.
    path.holderLiftDegrees = 0;
    path.holderWristDegrees = 0;
    path.retreatDistanceMm = 0;
    path.calibrated = false; // Set true AFTER this entire path is calibrated.
    return path;
}();

enum class AllianceColor { Red, Blue };
AllianceColor selectedAlliance = AllianceColor::Red;
bool neutralGoalRoute = false;

int remainingAutonomousMs() {
    int remainingMs = autonomousMotionBudgetMs - static_cast<int>(autonomousTimer.time(msec));
    return remainingMs > 0 ? remainingMs : 0;
}

int motionTimeoutMs(int maximumMs) {
    bool fieldConnected = robotCompetition.isFieldControl() ||
                          robotCompetition.isCompetitionSwitch();
    if (fieldConnected && (!robotCompetition.isEnabled() || !robotCompetition.isAutonomous())) {
        return 0;
    }
    int remainingMs = remainingAutonomousMs();
    return remainingMs < maximumMs ? remainingMs : maximumMs;
}

void stopAutonomousMotors() {
    drive.stop(brake);
    lift.stop(hold);
    claw.stop(hold);
    clawPosition.stop(hold);
}

bool rotateToggleDrive(double rotationDegrees) {
        int timeoutMs = motionTimeoutMs(2000);
        if (timeoutMs <= 0) {
            return false;
        }
        toggleMotor.setTimeout(timeoutMs, msec);
        double lfStartDegrees = driveMotorLf.position(degrees);
        double lbStartDegrees = driveMotorLb.position(degrees);
        double rbStartDegrees = driveMotorRb.position(degrees);
        double rfStartDegrees = driveMotorRf.position(degrees);
        return toggleMotor.spinFor(rotationDegrees, degrees, true) &&
            fabs(driveMotorLf.position(degrees) - lfStartDegrees - rotationDegrees) <= 5 &&
            fabs(driveMotorLb.position(degrees) - lbStartDegrees - rotationDegrees) <= 5 &&
            fabs(driveMotorRb.position(degrees) - rbStartDegrees - rotationDegrees) <= 5 &&
            fabs(driveMotorRf.position(degrees) - rfStartDegrees - rotationDegrees) <= 5;
}

bool toggleColor() {
    return rotateToggleDrive(toggleDegrees) && rotateToggleDrive(toggleRetreatDegrees);
}

bool moveMechanism(motor &mechanism, double targetDegrees) {
    int timeoutMs = motionTimeoutMs(2000);
    if (timeoutMs <= 0) {
        return false;
    }
    mechanism.setTimeout(timeoutMs, msec);
    return mechanism.spinToPosition(targetDegrees, degrees, true) &&
           fabs(mechanism.position(degrees) - targetDegrees) <= 5;
}

bool moveLift(double targetDegrees) {
        int timeoutMs = motionTimeoutMs(3000);
        if (timeoutMs <= 0) {
            return false;
        }
        lift.setTimeout(timeoutMs, msec);
        return lift.spinToPosition(targetDegrees, degrees, true) &&
            fabs(liftMotorL.position(degrees) - targetDegrees) <= 5 &&
            fabs(liftMotorR.position(degrees) - targetDegrees) <= 5;
}

bool driveDistance(double distanceMm) {
    int timeoutMs = motionTimeoutMs(4000);
    if (timeoutMs <= 0) {
        return false;
    }
    if (distanceMm == 0) {
        return true;
    }
    drive.setTimeout(timeoutMs, msec);
    return drive.driveFor(distanceMm > 0 ? forward : reverse, fabs(distanceMm), mm);
}

bool turnDegrees(double angleDegrees) {
    int timeoutMs = motionTimeoutMs(4000);
    if (timeoutMs <= 0) {
        return false;
    }
    if (angleDegrees == 0) {
        return true;
    }
    drive.setTimeout(timeoutMs, msec);
    return drive.turnFor(angleDegrees > 0 ? right : left, fabs(angleDegrees), degrees);
}

bool placeAtHolder(const PlacementPath &path) {
    return turnDegrees(path.pickupTurnDegrees) &&
           moveLift(path.pickupLiftDegrees) &&
           moveMechanism(clawPosition, path.pickupWristDegrees) &&
           moveMechanism(claw, path.clawOpenDegrees) &&
           driveDistance(path.pickupDistanceMm) &&
           moveMechanism(claw, path.clawClosedDegrees) &&
           moveLift(path.carryingLiftDegrees) &&
           driveDistance(-path.pickupDistanceMm) &&
           turnDegrees(path.holderTurnDegrees) &&
           driveDistance(path.holderDistanceMm) &&
           moveMechanism(clawPosition, path.holderWristDegrees) &&
           moveLift(path.holderLiftDegrees) &&
           moveMechanism(claw, path.clawOpenDegrees) &&
           driveDistance(-path.retreatDistanceMm);
}

void runAutonomous(int toggleCount, const PlacementPath &finalHolder) {
    stopAutonomousMotors();
    const PlacementPath *paths[] = {&blackHolderOne, &blackHolderTwo, &finalHolder};
    bool pathsReady = placementCount >= 1 && placementCount <= 3;
    for (int pathIndex = 0; pathsReady && pathIndex < placementCount; ++pathIndex) {
        pathsReady = paths[pathIndex]->calibrated &&
                     paths[pathIndex]->estimatedDurationMs > 0 &&
                     paths[pathIndex]->estimatedDurationMs <= autonomousMotionBudgetMs;
    }
    if (!autonomousCalibrated || !pathsReady || toggleCount < 0 ||
        initialReverseDistanceMm < 0 || autonomousMotionBudgetMs <= 0 ||
        autonomousMotionBudgetMs >= 15000 ||
        (toggleCount > 0 && (!toggleCalibrated || toggleDegrees == 0 ||
                    toggleRetreatDegrees == 0 ||
                    (toggleDegrees > 0) == (toggleRetreatDegrees > 0)))) {
        robotBrain.Screen.clearScreen();
        robotBrain.Screen.printAt(20, 40, "Autonomous disabled: calibrate first");
        return;
    }

    autonomousTimer.reset();
    drive.setStopping(brake);
    drive.setDriveVelocity(20, percent);
    drive.setTurnVelocity(15, percent);
    lift.setStopping(hold);
    lift.setVelocity(25, percent);
    claw.setStopping(hold);
    claw.setVelocity(20, percent);
    clawPosition.setStopping(hold);
    clawPosition.setVelocity(20, percent);

    bool succeeded = true;
    if (toggleCount > 0) {
        toggleMotor.setVelocity(20, percent);
        toggleMotor.setStopping(brake);
        for (int toggleIndex = 0; toggleIndex < toggleCount; ++toggleIndex) {
            if (!toggleColor()) {
                succeeded = false;
                break;
            }
        }
    }
    succeeded = succeeded && driveDistance(-initialReverseDistanceMm);
    const char *outcome = "Autonomous complete";
    for (int pathIndex = 0; succeeded && pathIndex < placementCount; ++pathIndex) {
        if (remainingAutonomousMs() < paths[pathIndex]->estimatedDurationMs) {
            outcome = "Autonomous: placement skipped (time)";
            break;
        }
        succeeded = placeAtHolder(*paths[pathIndex]);
    }
    stopAutonomousMotors();
    robotBrain.Screen.clearScreen();
    robotBrain.Screen.printAt(20, 40, succeeded ? outcome :
                                              "Autonomous stopped: motion or timeout");
}

void autonomousRed() {
    runAutonomous(redToggleCount, redHolder);
}

void autonomousBlue() {
    runAutonomous(blueToggleCount, blueHolder);
}

void autonomousBlack() {
    // Retained entry point for neutral-goal scoring, using the selected alliance.
    int toggleCount = selectedAlliance == AllianceColor::Red ? redToggleCount : blueToggleCount;
    runAutonomous(toggleCount, neutralFinalHolder);
}

void showAutonomousSelection() {
    robotBrain.Screen.clearScreen();
    robotBrain.Screen.printAt(20, 30, "Alliance:");
    robotBrain.Screen.printAt(20, 90, "RED                    BLUE");
    robotBrain.Screen.printAt(20, 120, "Final goal route (if 3 placements enabled):");
    robotBrain.Screen.printAt(20, 160, "ALLIANCE GOAL          NEUTRAL GOAL");
    const char *allianceName = selectedAlliance == AllianceColor::Red ? "Red" : "Blue";
    robotBrain.Screen.printAt(20, 200, false, "Selected: %s / %s", allianceName,
                             neutralGoalRoute ? "Neutral" : "Alliance goal");
    robotBrain.Screen.printAt(20, 225, autonomousCalibrated ? "Calibration enabled" :
                                                           "Motion disabled: calibration required");
}

void selectAutonomous() {
    bool fieldConnected = robotCompetition.isFieldControl() ||
                          robotCompetition.isCompetitionSwitch();
    if (isAutonomousSelectionLocked() ||
        (fieldConnected && robotCompetition.isEnabled())) {
        return;
    }
    int touchX = robotBrain.Screen.xPosition();
    int touchY = robotBrain.Screen.yPosition();
    if (touchY >= 65 && touchY <= 110) {
        selectedAlliance = touchX < 240 ? AllianceColor::Red : AllianceColor::Blue;
    } else if (touchY >= 135 && touchY <= 175) {
        neutralGoalRoute = touchX >= 240;
    } else {
        return;
    }
    showAutonomousSelection();
}

void autonomous() {
    if (neutralGoalRoute) {
        autonomousBlack();
    } else if (selectedAlliance == AllianceColor::Red) {
        autonomousRed();
    } else {
        autonomousBlue();
    }
}