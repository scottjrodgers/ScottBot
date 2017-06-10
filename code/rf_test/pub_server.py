import random
import time
from rf.float_message import FloatMessage
from rf.publisher import Publisher
import parameters

parameters.init_parameters()

pub = Publisher("test-float")

while True:
    messagedata = random.randrange(1,215)-80
    msg = FloatMessage()
    msg.value = float(messagedata)
    print(msg.value)
    pub.send(msg)

    time.sleep(1)
    
