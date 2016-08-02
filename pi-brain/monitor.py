## smart serial monitor

import serial
from OutputCache import OutputCache
import time
from datetime import datetime, timedelta

## Global Data
actuator_count = 18
position = [0 for i in xrange(actuator_count + 1)]
speed = [0 for i in xrange(actuator_count + 1)]
load = [0 for i in xrange(actuator_count + 1)]
voltage = [0 for i in xrange(actuator_count + 1)]
temperature = [0 for i in xrange(actuator_count + 1)]
moving = [0 for i in xrange(actuator_count + 1)]
ser = serial.Serial("/dev/ttyUSB0", 115200, timeout = 0) # 115200

## Target positions and velocities
goal_position = [384 for i in xrange(actuator_count + 1)]
goal_velocity = [200 for i in xrange(actuator_count + 1)]

## Global State Machine Data
state = 0
counter = 0
data_size = 0
message_id = 0
num_actuators = 0
serial_data = [0] * 1024
checksum = 0


#============================================================================
# Decode the status message received from ArbotiX-M and update global values
#============================================================================
def decode_status(data, num, ndata):
    global position, speed, load, voltage, temperature, moving
    if ndata == 9 * num:
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
    else:
        print "less data than expected..."


#============================================================================
# Process the incoming serial stream one byte at a time.  When a full
#   message is complete, call the handler function
#============================================================================
def process_byte(ch):
    global state, counter, data_size, message_id, num_actuators, serial_data, checksum
    if state == 0:
        if ch == 0xFF:
            checksum = ch
            state = 1
    elif state == 1:
        if ch & 0xF0 == 0xF0:
            checksum += ch
            state = 2
            message_id = ch & 0x0F
        else:
            state = 0
    elif state == 2:
        num_actuators = ch
        checksum += ch
        state = 3
    elif state == 3:
        data_size = ch
        checksum += ch
        counter = 0
        state = 4
    elif state == 4:
        serial_data[counter] = ch
        checksum += ch
        counter += 1
        if counter >= data_size:
            state = 5;
    elif state == 5:
        # check if checksum matches ch
        # if so, call the appropriate function to process this completed
        # message and its data
        if (checksum & 0xFF) == ch:
            if message_id == 2 or message_id == 3:
                # DEBUG
                if 1:
                    mesg = "Dbg: "
                    for i in xrange(data_size):
                        value = serial_data[i]
                        if i % 27 == 0:
                            mesg += "\nDbg: "
                        if i % 9 == 0:
                            mesg += " "
                        mesg = mesg + format(value, "02X") + " "
                    print mesg
                decode_status(serial_data, num_actuators, data_size)
            elif message_id == 0x0E:
                mesg = ""
                for i in xrange(data_size):
                    mesg += chr(serial_data[i])
                print "ArbotiX Message: " + mesg
            else:
                print "Other message code: " + message_id
        else:
            print checksum & 0xFF, "vs", ch
        state = 0;
        checksum = 0;


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

    check = sum(mesg) & 0xFF
    mesg.append(check) # no checksum at this time
    ser.write(bytearray(mesg))


#============================================================================
#   Transmit the target position and velocity to ArbotiX-M
#============================================================================
def transmit_test_message():
    global goal_position, goal_velocity, actuator_count

    message = [0xFF, 0xF3, 18, 9 * 18]
    for i in xrange(1, 19):  # loop from 1 to 18, not 0 to 17
        #goal_position
        value = 32 * i
        message.append(value // 256)
        message.append(value & 0xFF)
        #speed
        value = 200 * i + 315
        message.append(value // 256)
        message.append(value & 0xFF)
        # load
        value = 215 * i + 106
        message.append(value // 256)
        message.append(value & 0xFF)
        # voltage
        value = 17 + i * 10
        message.append(value & 0xFF)
        # temperature
        value = 32 + 7 * i
        message.append(value & 0xFF)
        # movement
        value = i % 2
        message.append(value & 0xFF)

    check = sum(message) & 0xFF
    message.append(check)
    ser.write(bytearray(message))

    mesg = "Sent:"
    for i in xrange(9*18):
        value = message[4+i]
        if i % 27 == 0:
            mesg += "\nSent:"
        if i % 9 == 0:
            mesg += " "
        mesg = mesg + format(value, "02X") + " "
        i += 1
    print mesg

##============================================
# returns the elapsed milliseconds since the start of the program
##============================================
def millis(prev, now):
   dt = now - prev
   ms = (dt.days * 24 * 60 * 60 + dt.seconds) * 1000 + dt.microseconds / 1000.0
   return ms

## One time Startup
print "Initializing..."
ser.flushInput()
ser.flushOutput()

print "Buffers Cleared..."

hexcache = OutputCache()

##============================================
## Main Loop
##============================================
prev_time = None
while 1:
    ## Handle incoming serial communications
    input_byte = ser.read(1)
    if len(input_byte) > 0:
        ch = ord(input_byte)
        #print format(ch, '#04x')
        #hexcache.add(ch)
        process_byte(ch)

    ## periodically update the target positions and transmit
    # time_now = datetime.now()
    # if prev_time is None or millis(prev_time, time_now) > 250:
    #     prev_time = time_now
    #     transmit_test_message()
    #     time_now = datetime.now()
    #     print millis(prev_time, time_now)



