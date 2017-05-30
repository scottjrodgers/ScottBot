"""
This node will control the dynamixel servos in both wheel-mode and position mode

It will subscribe to:
- Twist message from navigation stack
- /dxl/pan (Float - in radians
- /dxl/tilt (Float - in radians)

"""

import dynamixel_functions as dynamixel
import time
from math import pi as PI, floor, fabs, sin, cos

# Control table address
ADDR_MX_TORQUE_ENABLE       = 24                            # Control table address is different in Dynamixel model
ADDR_MX_GOAL_POSITION       = 30
ADDR_MX_PRESENT_POSITION    = 36
ADDR_MX_PRESENT_SPEED       = 38
ADDR_MX_TARGET_SPEED        = 32

# Protocol version
PROTOCOL_VERSION            = 1                             # See which protocol version is used in the Dynamixel

# Default setting
BAUDRATE                    = 1000000
DEVICENAME                  = "/dev/ttyUSB0".encode('utf-8')# Check which port is being used on your controller
                                                            # ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"

TORQUE_ENABLE               = 1                             # Value for enabling the torque
TORQUE_DISABLE              = 0                             # Value for disabling the torque

COMM_SUCCESS                = 0                             # Communication Success result value
COMM_TX_FAIL                = -1001                         # Communication Tx Failed
DXL_SUCCESS                 = 0
DXL_FAIL                    = 0


## Constants / Parameters
wheel_left_ID = 28
wheel_right_ID = 29
pan_servo_ID = 20
tilt_servo_ID = 19

wheel_diameter = 52.0 / 1000   # meters
wheel_separation = 289.6 / 1000   # meters

WHEEL_MAX = 4096
SERVO_MAX = 1024
pan_lower_limit = 256
pan_upper_limit = 768

SERVO_CENTER = 512
MX_MAX_RPM = 117.07  # RPM
MX_MAX_VELOCITY = MX_MAX_RPM / 60.0 * 2 * PI  # Radians / sec

## Global Variables:
x_pos = 0.0
y_pos = 0.0
theta = 0.0
prev_time = time.time()

pan_current_pos = 512
tilt_current_pos = 512
wheel_left_pos = 0
wheel_right_pos = 0
port_num = 0



def get_position(ID):
    """
    Read servo position (in integer ticks) for the given servo ID
    """
    dxl_present_position = dynamixel.read2ByteTxRx(port_num, PROTOCOL_VERSION, ID, ADDR_MX_PRESENT_POSITION)
    if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
        dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))
        dynamixel.printRxPacketError(PROTOCOL_VERSION, dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION))
        return 0
    return dxl_present_position


def get_velocity(ID):
    """
    Read servo velocity(in integer ticks/second) for the given servo ID
    """
    dxl_present_velocity = dynamixel.read2ByteTxRx(port_num, PROTOCOL_VERSION, ID, ADDR_MX_PRESENT_SPEED)
    if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
        dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))
        dynamixel.printRxPacketError(PROTOCOL_VERSION, dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION))
        return 0
    return dxl_present_velocity


def set_target_position(ID, position):
    """
    Sets the target position for a given servo to the given position (Integer)
    """
    dynamixel.write2ByteTxRx(port_num, PROTOCOL_VERSION, ID, ADDR_MX_GOAL_POSITION, position)
    if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
        dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))
        dynamixel.printRxPacketError(PROTOCOL_VERSION, dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION))
        return DXL_FAIL
    return DXL_SUCCESS


def set_target_velocity(ID, velocity):
    """
    Sets the target velocity for a given servo to the given position (Integer)
    """
    dynamixel.write2ByteTxRx(port_num, PROTOCOL_VERSION, ID, ADDR_MX_TARGET_SPEED, velocity)
    if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
        dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))
        dynamixel.printRxPacketError(PROTOCOL_VERSION, dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION))
        return DXL_FAIL
    return DXL_SUCCESS


def enable_torque(ID):
    """
    Enable torque on given servo
    """
    dynamixel.write1ByteTxRx(port_num, PROTOCOL_VERSION, ID, ADDR_MX_TORQUE_ENABLE, TORQUE_ENABLE)
    if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
        dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))
        dynamixel.printRxPacketError(PROTOCOL_VERSION, dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION))
        return DXL_FAIL
    return DXL_SUCCESS


def disable_torque(ID):
    """
    Disable torque on given servo
    """
    dynamixel.write1ByteTxRx(port_num, PROTOCOL_VERSION, ID, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE)
    if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
        dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))
        dynamixel.printRxPacketError(PROTOCOL_VERSION, dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION))
        return DXL_FAIL
    return DXL_SUCCESS


def initialize():
    """
    1) Connect to dynamixel USB port, abort if it fails to connect
    2) Read YAML config file and apply settings for
        - Pan and Tilt motor IDs
        - Pan and Tile motor limits
        - Left and Right Wheel IDs
        - Wheel separation
        - Wheel diameter
    3) Read in current wheel position for each wheel
    4) Reset pan / tilt to zeros
    """

    global port_num, wheel_left_pos, wheel_right_pos

    ## 1) Connect to Dynamixel USB
    port_num = dynamixel.portHandler(DEVICENAME)

    # Initialize PacketHandler Structs
    dynamixel.packetHandler()

    ## 2) Parse YAML config
    ## TODO: add this later
    pass

    ## 3) Read curren wheel positions
    wheel_left_pos = get_position(wheel_left_ID)
    wheel_right_pos = get_position(wheel_right_ID)

    ## 4) Reset Pan / Tilt to center
    set_target_position(pan_servo_ID, SERVO_CENTER)
    set_target_position(tilt_servo_ID, SERVO_CENTER)



## Set the target velocities in radians/sec
def set_wheel_velocities(v_left, v_right):
    max_linear_velocity = MX_MAX_VELOCITY * wheel_diameter * 2 * PI
    scale_factor = max(1.0, fabs(v_left / max_linear_velocity), fabs(v_right / max_linear_velocity))

    l_neg = 0
    r_pos = 0       # assuming that right motor is reversed
    if v_left < 0.0:
        l_neg = 1
    if v_right >= 0.0:
        r_pos = 1

    abs_v_left = fabs(v_left)
    abs_v_right = fabs(v_right)
    int_v_left = int(floor(abs_v_left / scale_factor))
    int_v_right = int(floor(abs_v_right / scale_factor))

    set_target_velocity(wheel_left_ID, int_v_left + l_neg * 1024, int_v_right + r_pos * 1024)
    return 0


# Get the delta
def get_wheel_movement_deltas():
    global wheel_left_pos, wheel_right_pos

    prev_left_pos = wheel_left_pos
    prev_right_pos = wheel_right_pos


    wheel_left_pos = get_position(wheel_left_ID)
    wheel_right_pos = -get_position(wheel_right_ID)   # Assume right motor is reversed

    delta_left =  wheel_left_pos - prev_left_pos
    if delta_left > WHEEL_MAX / 2:
        delta_left -= WHEEL_MAX
    elif delta_left < -WHEEL_MAX / 2:
        delta_left += WHEEL_MAX

    delta_right = wheel_right_pos - prev_right_pos
    if delta_right > WHEEL_MAX / 2:
        delta_right -= WHEEL_MAX
    elif delta_right < -WHEEL_MAX / 2:
        delta_right += WHEEL_MAX

    linear_left = delta_left * 2 * PI * wheel_diameter
    linear_right = delta_right * 2 * PI * wheel_diameter

    return linear_left, linear_right


# Stop Wheels
def stop_wheels():
    set_target_velocity(wheel_left_ID, 0)
    set_target_velocity(wheel_right_ID, 0)
    return


# Stop everything
def stop():
    stop_wheels()
    set_target_velocity(pan_servo_ID, 0)
    set_target_position(pan_servo_ID, get_position(pan_servo_ID))
    set_target_velocity(tilt_servo_ID, 0)
    set_target_position(tilt_servo_ID, get_position(tilt_servo_ID))
    return 0


#=====================================================================
#  Given a target forward velocity and target angular velocity,
#  Compute the linear velocity of left and right motors
#=====================================================================
def apply_twist(fwd_vel, omega):
    left_vel = fwd_vel - omega * wheel_separation
    right_vel = fwd_vel + omega * wheel_separation

    set_wheel_velocities(left_vel, right_vel)
    return 0


#=====================================================================
#  Poll the positions, integrate the forward and angular velocities to
#  Estimate the new position and orientation
#=====================================================================
def compute_odometry():
    # globals
    global x_pos, y_pos, theta, prev_time

    # get timestamp compute delta time and store new timestamp
    curr_time = time.time()
    delta_time = curr_time - prev_time
    prev_time = curr_time()

    # poll linear motion of wheels
    delta_left, delta_right = get_wheel_movement_deltas()

    # compute forward motion and angular motion
    fwd_delta = (delta_left + delta_right) / 2.0
    angular_delta = (delta_right - delta_left) / 2.0

    # integrate over 10 steps
    N = 10
    for i in range(N):
        x_pos += (fwd_delta/N) * cos(theta)
        y_pos += (fwd_delta/N) * sin(theta)
        theta += angular_delta / N

    # Compute velocities
    fwd_vel = fwd_delta / delta_time
    ang_vel = angular_delta / delta_time

    # Return all odometry values
    return x_pos, y_pos, theta, fwd_vel, ang_vel