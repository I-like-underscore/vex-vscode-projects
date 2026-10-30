/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       jerzhan2                                                  */
/*    Created:      10/24/2026, 1:45:38 PM                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "robot.h"

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

int main() {
    driverControl();
    return 0;
    
}
