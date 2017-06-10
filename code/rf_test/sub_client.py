"""
Test subscription client
"""
from rf.subscriber import Subscriber
from rf.float_message import FloatMessage
import time

def float_callback(msg):
    val = msg.value
    print("recieved %f", val)

sub = Subscriber("test-float", FloatMessage(), float_callback)

while 1:
    sub.check()
    print(".")
    time.sleep(0.2)


