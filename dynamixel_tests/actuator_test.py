from actuators import ActuatorInterface
import math
import time

actuators = ActuatorInterface()

for i in xrange(24+1):
    theta = 15 * i * math.pi / 180.0
    for id in [14,16,18]:
        actuators[id].torque_enable = True
        actuators[id].torque_limit = 1000
    actuators[14].goal_position = 512 + int(round(100 * math.sin(theta)))
    actuators[16].goal_position = 512 - int(round(200 * math.cos(theta)))
    actuators[18].goal_position = 412 + int(round(200 * math.cos(theta)))
    actuators.update(0.1)

