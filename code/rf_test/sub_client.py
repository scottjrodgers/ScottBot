"""
Test a subscriber node
"""
import numpy as np
from rf.node import Node

class SubNode(Node):
    def int_callback(self, msg):
        value = msg['value']
        print("received-int: %d" % value)

    def np_callback(self, msg):
        value = msg['value']
        print("received-msg: %s" % str(value))

    def initialize(self):
        self.create_subscriber("integer-test",  self.int_callback)
        self.create_subscriber("np-test",  self.np_callback)
        self.rate = 5.0
        self.poll_rate = 100.0


    def mainloop(self):
        print("...")

# Start node
nd = SubNode()
nd.run()

