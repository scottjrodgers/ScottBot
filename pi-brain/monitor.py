## smart serial monitor

import serial
from datetime import datetime, timedelta

## Global Data
actuator_count = 18
position = [0 for i in xrange(actuator_count + 1)]
speed = [0 for i in xrange(actuator_count + 1)]
load = [0 for i in xrange(actuator_count + 1)]
voltage = [0 for i in xrange(actuator_count + 1)]
temperature = [0 for i in xrange(actuator_count + 1)]
moving = [0 for i in xrange(actuator_count + 1)]
ser = serial.Serial("/dev/ttyUSB0", 115200) # 115200

## Target positions and velocities
goal_position = [0 for i in xrange(actuator_count + 1)]
goal_velocity = [0 for i in xrange(actuator_count + 1)]

## Global State Machine Data
state = 0
counter = 0
data_size = 0
message_id = 0
num_actuators = 0
serial_data = [0] * 1024


#============================================================================
# Decode the status message received from ArbotiX-M and update global values
#============================================================================
def decode_status(data, num, ndata):
    global position, speed, load, voltage, temperature, moving
    assert ndata == 9 * num
    n = 0
    for i in xrange(num):

        position[i] = data[n] * 256 + data[n+1]
        n += 2

        speed[i] = data[n] * 256 + data[n + 1]
        if speed[i] > 1024:
            speed[i] = 1024 - speed[i]
        n += 2

        load[i] = data[n] * 256 + data[n + 1]
        if load[i] > 1024:
            load[i] = 1024 - load[i]
        n += 2

        voltage[i] = data[n] / 10.0
        n += 1

        temperature[i] = data[n]
        n += 1

        moving[i] = data[n]
        n += 1

    mesg = ""
    for i in xrange(num):
        value = position[i]
        mesg = mesg + "%4d" % value + " "
    print mesg


#============================================================================
# Process the incoming serial stream one byte at a time.  When a full
#   message is complete, call the handler function
#============================================================================
def process_byte(ch):
    #print format(ch, '#04x')

    global state, counter, data_size, message_id, num_actuators, serial_data
    if state == 0:
        if ch == 0xFF:
            state = 1
    elif state == 1:
        if ch & 0xF0 == 0xF0:
            state = 2
            message_id = ch & 0x0F
        else:
            state = 0
    elif state == 2:
        num_actuators = ch
        state = 3
    elif state == 3:
        data_size = ch
        counter = 0
        state = 4
    elif state == 4:
        serial_data[counter] = ch
        counter += 1
        if counter >= data_size:
            state = 5;
    elif state == 5:
        # check if checksum matches ch
        # if so, call the appropriate function to process this completed
        # message and its data

        # TODO: Compute and check the checksum

        if 1 and message_id == 2:
            decode_status(serial_data, num_actuators, data_size)
        state = 0;


#============================================================================
#   Transmit the target position and velocity to ArbotiX-M
#============================================================================
def transmit_targets():
    global goal_position, goal_velocity, actuator_count

    mesg = [0xFF, 0xF1, actuator_count, 4 * actuator_count]
    for i in xrange(1, actuator_count+1):  # loop from 1 to 18, not 0 to 17
        value = goal_position[i]
        mesg.append(value // 256)
        mesg.append(value & 0xFF)
        value = goal_velocity[i]
        mesg.append(value // 256)
        mesg.append(value & 0xFF)

    mesg.append(0) # no checksum at this time
    ser.write(bytearray(mesg))


##============================================
# returns the elapsed milliseconds since the start of the program
##============================================
def millis(prev, now):
   dt = now - prev
   ms = (dt.days * 24 * 60 * 60 + dt.seconds) * 1000 + dt.microseconds / 1000.0
   return ms

## One time Startup
print "Initializing..."
while ser.in_waiting > 0:
    ser.read();

print "Cache Cleared..."

##============================================
## Main Loop
##============================================
prev_time = None
while 1:
    ## Handle incoming serial communications
    if ser.in_waiting > 0:
        ch = ord(ser.read())
        process_byte(ch)

    ## periodically update the target positions and transmit
    time_now = datetime.now()
    if prev_time is None or millis(prev_time, time_now) > 50:
        prev_time = time_now
        transmit_targets()
        time_now = datetime.now()
        print millis(prev_time, time_now)



