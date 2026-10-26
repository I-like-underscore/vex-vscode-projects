#pragma once

#include "vex.h"

extern vex::brain robotBrain;
extern vex::controller controller1;
extern vex::motor driveMotorLf;
extern vex::motor driveMotorLb;
extern vex::motor driveMotorRb;
extern vex::motor driveMotorRf;
extern vex::motor_group toggleMotor;
extern vex::motor liftMotorR;
extern vex::motor liftMotorL;
extern vex::motor_group lift;
extern vex::motor claw;
extern vex::motor clawPosition;
extern vex::competition robotCompetition;
extern vex::drivetrain drive;
extern const double wheelTravelMm;
extern const double externalGearRatio;

void autonomousRed();
void autonomousBlue();
void autonomousBlack();
void autonomous();
void driverControl();
void selectAutonomous();
void showAutonomousSelection();
void stopAutonomousMotors();
bool isAutonomousSelectionLocked();
bool isAutonomousCalibrated();
