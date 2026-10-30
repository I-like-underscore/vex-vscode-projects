/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       jerzhan2                                                  */
/*    Created:      10/24/2026, 1:45:38 PM                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "robot.h"
#include <atomic>

using namespace vex;

brain robotBrain;
controller controller1(primary);
motor driveMotorLf(PORT19, ratio6_1, false);
motor driveMotorLb(PORT20, ratio6_1, false);
motor driveMotorRb(PORT11, ratio6_1, false);
motor driveMotorRf(PORT12, ratio6_1, false);
motor liftMotorR(PORT2, ratio36_1, false);
motor liftMotorL(PORT21, ratio36_1, true);
motor_group lift(liftMotorR, liftMotorL);
motor claw(PORT1, ratio18_1, false);
motor toggleWheel(PORT10, ratio18_1, false); // Left-front field-toggle wheel.

competition robotCompetition;
motor_group leftDrive(driveMotorLf, driveMotorLb);
motor_group rightDrive(driveMotorRf, driveMotorRb);

// TODO: replace these drivetrain geometry placeholders with measured values.
// Route travel distances are edited in autonomous.cpp, not here.
const double wheelTravelMm = 259.34; // Nominal circumference of a 3.25-inch wheel, in mm.
const double trackWidthMm = 350.0; // Left-to-right wheel center spacing, in mm.
const double wheelBaseMm = 253.0; // Front-to-back wheel center spacing, in mm.
const double externalGearRatio = 1.0; // Use the actual external drivetrain ratio.
drivetrain drive(leftDrive, rightDrive, wheelTravelMm, trackWidthMm,
                 wheelBaseMm, mm, externalGearRatio);

// EDIT: true = controller-button practice; false = official competition control.
const bool practiceModeEnabled = true;
const int autonomousPeriodMs = 15000;
std::atomic<bool> practiceMatchActive(false);

bool isAutonomousSelectionLocked() {
    return practiceMatchActive.load();
}

bool practiceStopRequested() {
    return controller1.ButtonX.pressing() || robotCompetition.isFieldControl() ||
           robotCompetition.isCompetitionSwitch();
}

bool waitForPracticeStart(bool manualPhase) {
    bool buttonReleased = false;
    while (!practiceStopRequested()) {
        bool buttonPressed = manualPhase ? controller1.ButtonB.pressing() :
                                           controller1.ButtonA.pressing();
        if (!buttonPressed) {
            buttonReleased = true;
        } else if (buttonReleased && controller1.installed()) {
            return true;
        }
        wait(20, msec);
    }
    return false;
}

bool runPracticePeriod(void (*callback)(), int durationMs) {
    timer periodTimer;
    thread modeThread(callback);
    while ((durationMs <= 0 || periodTimer.time(msec) < durationMs) &&
           !practiceStopRequested() && controller1.installed()) {
        wait(10, msec);
    }
    // Stop the command-producing thread BEFORE stopping the motors.
    modeThread.interrupt();
    stopAutonomousMotors();
    return !practiceStopRequested() && controller1.installed();
}

void runPracticeMatch() {
    stopAutonomousMotors();
    if (!isAutonomousCalibrated()) {
        robotBrain.Screen.clearScreen();
        robotBrain.Screen.printAt(20, 40, "CALIBRATION MODE: autonomous disabled");
        robotBrain.Screen.printAt(20, 75, "Press B for manual calibration");
        robotBrain.Screen.printAt(20, 110, "Press X to stop");
        controller1.Screen.clearScreen();
        controller1.Screen.setCursor(1, 1);
        controller1.Screen.print("CALIBRATE B:start");
        if (!waitForPracticeStart(true)) {
            return;
        }

        practiceMatchActive.store(true);
        runPracticePeriod(driverControl, 0);
        return;
    }

    controller1.Screen.clearScreen();
    controller1.Screen.setCursor(1, 1);
    controller1.Screen.print("A: auto 15s  X: stop");
    if (!waitForPracticeStart(false)) {
        return;
    }

    practiceMatchActive.store(true);
    robotBrain.Screen.clearScreen();
    robotBrain.Screen.printAt(20, 40, "Autonomous: 15 seconds. X: stop");
    controller1.Screen.clearScreen();
    controller1.Screen.setCursor(1, 1);
    controller1.Screen.print("AUTO 15s  X: stop");
    if (!runPracticePeriod(autonomous, autonomousPeriodMs)) {
        return;
    }

    robotBrain.Screen.clearScreen();
    robotBrain.Screen.printAt(20, 40, "Autonomous ended. B: manual control");
    controller1.Screen.clearScreen();
    controller1.Screen.setCursor(1, 1);
    controller1.Screen.print("B: manual control");
    if (!waitForPracticeStart(true)) {
        return;
    }

    robotBrain.Screen.clearScreen();
    robotBrain.Screen.printAt(20, 40, "Manual control. X: stop");
    controller1.Screen.clearScreen();
    controller1.Screen.setCursor(1, 1);
    controller1.Screen.print("MANUAL X: stop");
    runPracticePeriod(driverControl, 0);
}

int main() {
    robotBrain.Screen.pressed(selectAutonomous);
    showAutonomousSelection();
    if (practiceModeEnabled) {
        runPracticeMatch();
        practiceMatchActive.store(true);
        stopAutonomousMotors();
        robotBrain.Screen.clearScreen();
        robotBrain.Screen.printAt(20, 40, "Practice stopped. Restart program to retry.");
        controller1.Screen.clearScreen();
        controller1.Screen.setCursor(1, 1);
        controller1.Screen.print("STOPPED: restart");
    } else {
        robotCompetition.autonomous(autonomous);
        robotCompetition.drivercontrol(driverControl);
    }
    while (true) {
        wait(100, msec);
    }
}
