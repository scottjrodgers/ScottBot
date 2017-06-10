"""
Test a publisher node
"""

from rf.node import Node
import numpy as np

class PubNode2(Node):
    def initialize(self):
        self.pub = self.create_publisher("np-test")
        self.rate = 2.5

    def mainloop(self):
        v = np.random.uniform(0, 10, 3)
        msg = dict(value = v)
        self.publish_message(self.pub, msg)
        print("value: %s" % str(v))

# Start node
nd = PubNode2()
nd.run()
