#include "robot.h"

using namespace vex;

const int driverSpeedPercent = 30;
const int liftSpeedPercent = 40;

void driverControl() {
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

        this_thread::sleep_for(20);
    }
}
