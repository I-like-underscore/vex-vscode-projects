#include "robot.h"

using namespace vex;

const int driverSpeedPercent = 30;
const int liftSpeedPercent = 40;

void resetDriveMeasurement() {
    driveMotorLf.setPosition(0, degrees);
    driveMotorLb.setPosition(0, degrees);
    driveMotorRf.setPosition(0, degrees);
    driveMotorRb.setPosition(0, degrees);
}

void showCalibrationDebug() {
    double lfDegrees = driveMotorLf.position(degrees);
    double lbDegrees = driveMotorLb.position(degrees);
    double rfDegrees = driveMotorRf.position(degrees);
    double rbDegrees = driveMotorRb.position(degrees);
    double leftDegrees = (lfDegrees + lbDegrees) / 2.0;
    double rightDegrees = (rfDegrees + rbDegrees) / 2.0;
    double averageDegrees = (leftDegrees + rightDegrees) / 2.0;
    double estimatedDistanceMm = averageDegrees / 360.0 *
                                 wheelTravelMm * externalGearRatio;

    robotBrain.Screen.clearScreen();
    robotBrain.Screen.printAt(10, 25, "CALIBRATION  Up = reset drive distance");
    robotBrain.Screen.printAt(10, 55, false, "Drive estimate: %7.1f mm",
                              estimatedDistanceMm);
    robotBrain.Screen.printAt(10, 85, false, "Drive deg L:%7.1f R:%7.1f",
                              leftDegrees, rightDegrees);
    robotBrain.Screen.printAt(10, 115, false, "LF:%6.0f LB:%6.0f RF:%6.0f RB:%6.0f",
                              lfDegrees, lbDegrees, rfDegrees, rbDegrees);
    robotBrain.Screen.printAt(10, 150, false, "Lift R:%7.1f  L:%7.1f",
                              liftMotorR.position(degrees),
                              liftMotorL.position(degrees));
    robotBrain.Screen.printAt(10, 180, false, "Toggle:%7.1f  Claw:%7.1f",
                              toggleWheel.position(degrees),
                              claw.position(degrees));
    robotBrain.Screen.printAt(10, 215, "X = stop measurement session");

    controller1.Screen.clearScreen();
    controller1.Screen.setCursor(1, 1);
    controller1.Screen.print("D:%6.0fmm", estimatedDistanceMm);
    controller1.Screen.setCursor(2, 1);
    controller1.Screen.print("Lift:%5.0f", liftMotorR.position(degrees));
    controller1.Screen.setCursor(3, 1);
    controller1.Screen.print("T:%4.0f C:%4.0f",
                             toggleWheel.position(degrees),
                             claw.position(degrees));
}

void driverControl() {
    const bool calibrationDebugEnabled = !isAutonomousCalibrated() &&
        !robotCompetition.isFieldControl() &&
        !robotCompetition.isCompetitionSwitch();
    bool resetButtonWasPressed = false;
    int debugRefreshCount = 0;
    if (calibrationDebugEnabled) {
        resetDriveMeasurement();
        showCalibrationDebug();
    }

    while (true) {
        int driveSpeed = -controller1.Axis1.position() * driverSpeedPercent / 100;
        int turn = -controller1.Axis3.position() * driverSpeedPercent / 100;

        if (driveSpeed > -10 && driveSpeed < 10) {
            driveSpeed = 0;
        }
        if (turn > -10 && turn < 10) {
            turn = 0;
        }

        int leftSpeed = driveSpeed + turn;
        int rightSpeed = driveSpeed - turn;

        if (leftSpeed > 100) {
            leftSpeed = 100;
        } else if (leftSpeed < -100) {
            leftSpeed = -100;
        }
        if (rightSpeed > 100) {
            rightSpeed = 100;
        } else if (rightSpeed < -100) {
            rightSpeed = -100;
        }

        driveMotorLf.setVelocity(leftSpeed, percent);
        driveMotorLb.setVelocity(leftSpeed, percent);
        driveMotorRf.setVelocity(rightSpeed, percent);
        driveMotorRb.setVelocity(rightSpeed, percent);
        driveMotorLf.spin(forward);
        driveMotorLb.spin(forward);
        driveMotorRf.spin(forward);
        driveMotorRb.spin(forward);

        if (controller1.ButtonL2.pressing()) {
            lift.spin(forward, liftSpeedPercent, percent);
        } else if (controller1.ButtonL1.pressing()) {
            lift.spin(reverse, liftSpeedPercent, percent);
        } else {
            lift.stop(hold);
        }

        if (controller1.ButtonR1.pressing()) {
            claw.spin(forward, 80, percent);
        } else if (controller1.ButtonR2.pressing()) {
            claw.spin(reverse, 80, percent);
        } else {
            claw.stop(hold);
        }

        if (controller1.ButtonLeft.pressing()) {
            toggleWheel.spin(forward, 30, percent);
        } else if (controller1.ButtonRight.pressing()) {
            toggleWheel.spin(reverse, 30, percent);
        } else {
            toggleWheel.stop(hold);
        }

        if (calibrationDebugEnabled) {
            bool resetButtonPressed = controller1.ButtonUp.pressing();
            if (resetButtonPressed && !resetButtonWasPressed) {
                resetDriveMeasurement();
            }
            resetButtonWasPressed = resetButtonPressed;

            if (++debugRefreshCount >= 5) {
                showCalibrationDebug();
                debugRefreshCount = 0;
            }
        }

        this_thread::sleep_for(20);
    }
}
