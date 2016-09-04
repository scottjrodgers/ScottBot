"""
Class Actuators:
  Abstracts the interface with the Dynamixel actuators
"""
import dynamixel
import sys
import time
import math

# Max rotation speed of servo at full power
_RPM = 55

# Speed = ( | target - current | / time ) * speed_factor
_speed_factor = _RPM * 360.0 / 60.0 / 1023


class ActuatorInterface:
    #==========================================================================================
    # Initialize Interface
    #==========================================================================================
    def __init__(self, port = '/dev/ttyUSB1'):
        self.baudrate = 1000000
        self.port = port
        self.maxID = 18
        self.actuators = [None] * (self.maxID + 1)

        # Establish a serial connection to the dynamixel network.
        # This usually requires a USB2Dynamixel
        self.serial = dynamixel.SerialStream(port=self.port,
                                        baudrate=self.baudrate,
                                        timeout=1)
        # Instantiate our network object
        self.net = dynamixel.DynamixelNetwork(self.serial)

        # Populate our network with dynamixel objects
        for servoId in xrange(1,self.maxID+1):
            newDynamixel = dynamixel.Dynamixel(servoId, self.net)
            self.net._dynamixel_map[servoId] = newDynamixel
            self.actuators[servoId] = newDynamixel

        # Verify Initialization
        if not self.net.get_dynamixels():
            print 'No Dynamixels Found!'

            # TODO: Throw exception instead
            sys.exit(0)
        else:
            print "Dynamixels connected."

        # Initialize servos
        for servoId in xrange(1,self.maxID+1):
            dyn = self.actuators[servoId]
            dyn.torque_enable = True
            dyn.goal_position = dyn.current_position



    #==========================================================================================
    # Returns list of actuators (Dynamixel objects)
    #   Fields:
    #       - goal_position (0 - 1023)
    #       - moving_speed (1 - ?)
    #       - torque_enable (boolean)
    #       - torque_limit (1 - ?)
    #       - max_torque (1 - ?)
    #==========================================================================================
    def get_actuators(self):
        return self.net.get_dynamixels()

    #==========================================================================================
    # Synchronize the movement of the actuators to the new target pose
    #   then sleep for t seconds.
    #==========================================================================================
    def update(self, t):
        for id in range(1,self.maxID+1):
            dyn = self.actuators[id]
            target_speed = int(math.floor(abs(dyn.goal_position - dyn.current_position)/t * _speed_factor))
            target_speed -= 1
            if target_speed < 10:
                target_speed = 10
            if target_speed > 1023:
                target_speed = 1023
            if id in [14,16,18]:
                print "id %d: %d" % (id, target_speed)
            dyn.moving_speed = target_speed
        self.net.synchronize()
        time.sleep(t)

    # ==========================================================================================
    # Access a particular Dynamixel
    # ==========================================================================================
    def __getitem__(self, id):
        if id > 0 and id <= self.maxID:
            return self.actuators[id]
        return None
