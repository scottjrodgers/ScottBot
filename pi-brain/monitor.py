## smart serial monitor

import serial

## Global Data
position = [0 for i in xrange(18)]
speed = [0 for i in xrange(18)]
load = [0 for i in xrange(18)]
voltage = [0 for i in xrange(18)]
temperature = [0 for i in xrange(18)]
moving = [0 for i in xrange(18)]
ser = serial.Serial("/dev/ttyUSB0", 115200)


## Global State Machine Data
state = 0
counter = 0
data_size = 0
message_id = 0
num_actuators = 0
serial_data = [0] * 1024


# def get_byte():
#     b = ord(ser.read())
#     return b
#
# def get_short():
#     high = get_byte()
#     low = get_byte()
#     return 256 * high + low
#
# def get_signed_short():
#     high = get_byte()
#     low = get_byte()
#     value = 256 * high + low
#     if value >= 1024:
#         return -(value - 1024)
#     return value


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


def process_byte(ch):
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
        if 1 and message_id == 2:
            decode_status(serial_data, num_actuators, data_size)
        state = 0;







## One time Startup
print "Initializing..."

##============================================
## Main Loop
##============================================
while 1:
    ## Wait until we get a 0xFF
    if ser.in_waiting > 0:
        ch = ord(ser.read())
        #print format(ch, '#04x')
        process_byte(ch)


    # bprev = 0
    # while b != 0xFF or bprev != 0xFF:
    #     bprev = b
    #     b = get_byte()
    #
    # ## Read the command type
    # cmd = get_byte()
    # print "cmd =", cmd
    #
    # n_actuators = get_byte();
    # print "servo-cnt =", n_actuators
    #
    # ## Read the number of data bytes
    # n_data = get_byte()
    # print "data-size =", n_data
    #
    # for i in xrange(n_actuators):
    #     position[i] = get_short()
    #     speed[i] = get_signed_short()
    #     load[i] = get_signed_short()
    #     voltage[i] = get_byte()
    #     temperature[i] = get_byte()
    #     moving[i] = get_byte()
    #
    # mesg = ""
    # for i in xrange(n_actuators):
    #     value = position[i]
    #     mesg = mesg + "%4d" % value + " "
    # print mesg
    #
    # ## Get checksum byte
    # chk = get_byte()
    

