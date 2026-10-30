#pragma once

#include "vex.h"

extern vex::brain robotBrain;
extern vex::controller controller1;
extern vex::motor driveMotorLf;
extern vex::motor driveMotorLb;
extern vex::motor driveMotorRb;
extern vex::motor driveMotorRf;
extern vex::motor toggleWheel;
extern vex::motor liftMotorR;
extern vex::motor liftMotorL;
extern vex::motor_group lift;
extern vex::motor claw;

void driverControl();
