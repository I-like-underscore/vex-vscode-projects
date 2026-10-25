#region VEXcode Generated Robot Configuration
from vex import *
import urandom
import math

# Brain should be defined by default
brain=Brain()

# Robot configuration code
controller_1 = Controller(PRIMARY)
LF = Motor(Ports.PORT19, GearSetting.RATIO_6_1, False)
LB = Motor(Ports.PORT20, GearSetting.RATIO_6_1, False)
RB = Motor(Ports.PORT11, GearSetting.RATIO_6_1, False)
RF = Motor(Ports.PORT12, GearSetting.RATIO_6_1, False)
Lift_motor_R = Motor(Ports.PORT2, GearSetting.RATIO_36_1, False)
Lift_motor_L = Motor(Ports.PORT21, GearSetting.RATIO_36_1, True)
Lift = MotorGroup(Lift_motor_R, Lift_motor_L)
Claw = Motor(Ports.PORT1, GearSetting.RATIO_18_1, False)
Claw_position = Motor(Ports.PORT10, GearSetting.RATIO_18_1, False)


# wait for rotation sensor to fully initialize
wait(30, MSEC)


# Make random actually random
def initializeRandomSeed():
    wait(100, MSEC)
    random = brain.battery.voltage(MV) + brain.battery.current(CurrentUnits.AMP) * 100 + brain.timer.system_high_res()
    urandom.seed(int(random))
      
# Set random seed 
initializeRandomSeed()


def play_vexcode_sound(sound_name):
    # Helper to make playing sounds from the V5 in VEXcode easier and
    # keeps the code cleaner by making it clear what is happening.
    print("VEXPlaySound:" + sound_name)
    wait(5, MSEC)

# add a small delay to make sure we don't print in the middle of the REPL header
wait(200, MSEC)
# clear the console to make sure we don't have the REPL in the console
print("\033[2J")

#endregion VEXcode Generated Robot Configuration

# ------------------------------------------
# 
# 	Project:      VEXcode Project
#	Author:       VEX
#	Created:
#	Description:  VEXcode V5 Python Project
# 
# ------------------------------------------

# Library imports
from vex import *

# Begin project code
def autonomous():
    pass
def driver_control():
    while True:
        #drive
        forward = -controller_1.axis1.position()
        turn = -controller_1.axis3.position()
        if abs(forward) < 10:
            forward = 0
        if abs(turn) < 10:
            turn = 0
        left_speed = forward + turn
        right_speed = forward - turn
        if left_speed > 100:
            left_speed = 100
        elif left_speed < -100:
            left_speed = -100

        if right_speed > 100:
            right_speed = 100
        elif right_speed < -100:
            right_speed = -100
        LF.set_velocity(left_speed, PERCENT)
        LB.set_velocity(left_speed, PERCENT)
        RF.set_velocity(right_speed, PERCENT)
        RB.set_velocity(right_speed, PERCENT)
        LF.spin(FORWARD)
        LB.spin(FORWARD)
        RF.spin(FORWARD)
        RB.spin(FORWARD)
        #Lift
        if controller_1.buttonL2.pressing():
            Lift.spin(FORWARD, 100, PERCENT)
        elif controller_1.buttonL1.pressing():
            Lift.spin(REVERSE, 70, PERCENT)
        else:
            Lift.stop(HOLD)
        #Claw
        if controller_1.buttonR1.pressing():
            Claw.spin(FORWARD, 80, PERCENT)
        elif controller_1.buttonR2.pressing():
            Claw.spin(REVERSE, 80, PERCENT)
        else:
            Claw.stop(HOLD)
        if controller_1.buttonLeft.pressing():
            Claw_position.spin(FORWARD, 30, PERCENT)
        elif controller_1.buttonRight.pressing():
            Claw_position.spin(REVERSE, 30, PERCENT)
        else:
            Claw_position.stop(HOLD)
        # Allow other tasks to run
        wait(20, MSEC)
driver_control()