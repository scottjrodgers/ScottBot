## smart serial monitor

import serial

ser = serial.Serial("/dev/ttyUSB0", 115200)

def get_byte():
    b = ord(ser.read())
    #print format(b, '#04x')
    return b

def get_short():
    high = get_byte()
    low = get_byte()
    return 256 * high + low

def get_signed_short():
    high = get_byte()
    low = get_byte()
    value = 256 * high + low
    if value >= 1024:
        return -(value - 1024)
    return value


print "Initializing..."

position = [0 for i in xrange(18)]
speed = [0 for i in xrange(18)]
load = [0 for i in xrange(18)]
voltage = [0 for i in xrange(18)]
temperature = [0 for i in xrange(18)]
moving = [0 for i in xrange(18)]


## loop forever
while 1:
    ## Wait until we get a 0xFF
    b = get_byte();

    bprev = 0
    while b != 0xFF or bprev != 0xFF:
        bprev = b
        b = get_byte()

    ## Read the command type
    cmd = get_byte()
    print "cmd =", cmd

    n_actuators = get_byte();
    print "servo-cnt =", n_actuators

    ## Read the number of data bytes
    n_data = get_byte()
    print "data-size =", n_data

    for i in xrange(n_actuators):
        position[i] = get_short()
        speed[i] = get_signed_short()
        load[i] = get_signed_short()
        voltage[i] = get_byte()
        temperature[i] = get_byte()
        moving[i] = get_byte()

    mesg = ""
    for i in xrange(n_actuators):
        value = position[i]
        mesg = mesg + "%4d" % value + " "
    print mesg

    ## Get checksum byte
    chk = get_byte()
    

