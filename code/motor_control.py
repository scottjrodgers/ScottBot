"""
Motor Control and Differential Drive Node
"""

import numpy as np
import dynamixel_functions as dyn
import time
from math import pi as PI, floor, fabs, sin, cos
from rf.node import Node

class MotorControl(Node):

    #=====================================================================
    # initialize this node
    #=====================================================================
    def initialize(self):
        ## Constants / Parameters
        self.wheel_left_ID = 28
        self.wheel_right_ID = 29
        self.pan_servo_ID = 20
        self.tilt_servo_ID = 19

        self.wheel_diameter = 52.0 / 1000  # meters
        self.wheel_separation = 289.6 / 1000  # meters

        self.WHEEL_MAX = 4096
        self.SERVO_MAX = 1024
        self.pan_lower_limit = 256
        self.pan_upper_limit = 768

        self.SERVO_CENTER = 512
        self.MX_MAX_RPM = 117.07  # RPM
        self.MX_MAX_VELOCITY = self.MX_MAX_RPM / 60.0 * 2 * PI  # Radians / sec

        ## Global Variables:
        self.x_pos = 0.0
        self.y_pos = 0.0
        self.theta = 0.0
        self.prev_time = time.time()

        self.pan_current_pos = 512
        self.tilt_current_pos = 512
        self.wheel_left_pos = 0
        self.wheel_right_pos = 0

        # Check which port is being used on your controller
        # ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"
        self.DEVICENAME = "/dev/ttyUSB0".encode('utf-8')

        ## 1) Connect to Dynamixel USB
        self.port_num = dyn.portHandler(self.DEVICENAME)

        # Initialize PacketHandler Structs
        dyn.packetHandler()

        ## 3) Read curren wheel positions
        wheel_left_pos = dyn.get_position(self.port_num, self.wheel_left_ID)
        wheel_right_pos = dyn.get_position(self.port_num, self.wheel_right_ID)

        ## 4) Reset Pan / Tilt to center
        dyn.set_target_position(self.port_num, self.pan_servo_ID, self.SERVO_CENTER)
        dyn.set_target_position(self.port_num, self.tilt_servo_ID, self.SERVO_CENTER)

        #----------------------------
        # Initialize subscribers
        #----------------------------
        self.create_subscriber("cmd-vel",  self.twist_callback)



    ## Set the target velocities in radians/sec
    def set_wheel_velocities(self, v_left, v_right):
        max_linear_velocity = self.MX_MAX_VELOCITY * self.wheel_diameter * 2 * PI
        scale_factor = max(1.0, fabs(v_left / max_linear_velocity), fabs(v_right / max_linear_velocity))

        l_neg = 0
        r_pos = 0  # assuming that right motor is reversed
        if v_left < 0.0:
            l_neg = 1
        if v_right >= 0.0:
            r_pos = 1

        abs_v_left = fabs(v_left)
        abs_v_right = fabs(v_right)
        int_v_left = int(floor(abs_v_left / scale_factor))
        int_v_right = int(floor(abs_v_right / scale_factor))

        dyn.set_target_velocity(self.port_num, self.wheel_left_ID, int_v_left + l_neg * 1024)
        dyn.set_target_velocity(self.port_num, self.wheel_right_ID, int_v_right + r_pos * 1024)
        return 0


    # Get the delta
    def get_wheel_movement_deltas(self):
        global wheel_left_pos, wheel_right_pos

        prev_left_pos = wheel_left_pos
        prev_right_pos = wheel_right_pos

        wheel_left_pos = dyn.get_position(self.port_num, self.wheel_left_ID)
        wheel_right_pos = -dyn.get_position(self.port_num, self.wheel_right_ID)  # Assume right motor is reversed

        delta_left = wheel_left_pos - prev_left_pos
        if delta_left > self.WHEEL_MAX / 2:
            delta_left -= self.WHEEL_MAX
        elif delta_left < -self.WHEEL_MAX / 2:
            delta_left += self.WHEEL_MAX

        delta_right = wheel_right_pos - prev_right_pos
        if delta_right > self.WHEEL_MAX / 2:
            delta_right -= self.WHEEL_MAX
        elif delta_right < -self.WHEEL_MAX / 2:
            delta_right += self.WHEEL_MAX

        linear_left = delta_left * 2 * PI * self.wheel_diameter
        linear_right = delta_right * 2 * PI * self.wheel_diameter

        return linear_left, linear_right


    # Stop Wheels
    def stop_wheels(self):
        dyn.set_target_velocity(self.port_num, self.wheel_left_ID, 0)
        dyn.set_target_velocity(self.port_num, self.wheel_right_ID, 0)
        return


    # Stop everything
    def stop(self):
        self.stop_wheels()
        dyn.set_target_velocity(self.port_num, self.pan_servo_ID, 0)
        dyn.set_target_position(self.port_num, self.pan_servo_ID, dyn.get_position(self.pan_servo_ID))
        dyn.set_target_velocity(self.port_num, self.tilt_servo_ID, 0)
        dyn.set_target_position(self.port_num, self.tilt_servo_ID, dyn.get_position(self.tilt_servo_ID))
        return 0


    #=====================================================================
    #  Given a target forward velocity and target angular velocity,
    #  Compute the linear velocity of left and right motors
    #=====================================================================
    def apply_twist(self, fwd_vel, omega):
        left_vel = fwd_vel - omega * self.wheel_separation
        right_vel = fwd_vel + omega * self.wheel_separation

        self.set_wheel_velocities(left_vel, right_vel)
        return 0


    #=====================================================================
    #  Poll the positions, integrate the forward and angular velocities to
    #  Estimate the new position and orientation
    #=====================================================================
    def compute_odometry(self):
        # globals
        global x_pos, y_pos, theta, prev_time

        # get timestamp compute delta time and store new timestamp
        curr_time = time.time()
        delta_time = curr_time - prev_time
        prev_time = curr_time()

        # poll linear motion of wheels
        delta_left, delta_right = self.get_wheel_movement_deltas()

        # compute forward motion and angular motion
        fwd_delta = (delta_left + delta_right) / 2.0
        angular_delta = (delta_right - delta_left) / 2.0

        # integrate over 10 steps
        N = 10
        for i in range(N):
            x_pos += (fwd_delta / N) * cos(theta)
            y_pos += (fwd_delta / N) * sin(theta)
            theta += angular_delta / N

        # Compute velocities
        ang_vel = angular_delta / delta_time
        fwd_vel = fwd_delta / delta_time
        vx = fwd_vel * cos(theta)
        vy = fwd_vel * sin(theta)

        # Return all odometry values
        return x_pos, y_pos, theta, vx, vy, ang_vel


    #=====================================================================
    # cmd_vel callback function
    #=====================================================================
    def twist_callback(self, cmd_vel):
        fwd_vel = cmd_vel['x']
        ang_vel = cmd_vel['rz']

        self.apply_twist(fwd_vel, ang_vel)


    #=====================================================================
    # Main loop
    #=====================================================================
    def mainloop(self):
        print("..")




#=====================================================================
# Main
#=====================================================================
if __name__ == '__main__':
    n = MotorControl()
    n.run()

    # # Initialize the node and name it
    # rospy.init_node('motor_control')
    #
    # # Connect and set up servos
    # initialize_motors()
    #
    # rate = 50.0
    #
    # # Publish the /odom topic
    # odom_pub = rospy.Publisher('odom', Odometry, queue_size=50)
    # odom_broadcaster = tf.TransformBroadcaster()
    #
    # # Subscribe to the /cmd_Vel topic
    # rospy.Subscriber("cmd_vel", Twist, callback)
    #
    # # Subscribe to the /pan topic
    # # TODO: pan topic
    #
    # # Subscribe to the /tilt topic
    # # TODO: tilt topic
    #
    #
    # # main loop
    # r = rospy.Rate(rate)
    # while not rospy.is_shutdown():
    #     # Get relevant odometry
    #     x, y, th, vx, vy, vth = compute_odometry()
    #
    #     # build quaternion
    #     odom_quat = tf.transformations.quaternion_from_euler(0, 0, theta)
    #
    #     odom_broadcaster.sendTransform((x_pos, y_pos, 0.), odom_quat, prev_time, "base_link", "odom")
    #
    #     odom = Odometry()
    #     odom.header.stamp = prev_time
    #     odom.header.frame_id = "odom"
    #     odom.pose.pose = Pose(Point(x, y, 0.), Quaternion(*odom_quat))
    #     odom.child.frame_id = "base_link"
    #     odom.twist.twist = Twist(Vector3(vx, vy, 0.), Vector3(0., 0., vth))
    #
    #     odom_pub.publish(odom)
    #
    #     r.sleep()
