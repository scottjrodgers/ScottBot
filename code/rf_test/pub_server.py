"""
Test a publisher node
"""

from rf.node import Node

class PubNode(Node):
    def initialize(self):
        self.value = 1
        self.pub = self.create_publisher("integer-test")
        self.rate = 1.0

    def mainloop(self):
        msg = dict(value = self.value)
        self.publish_message(self.pub, msg)
        print("value: %d" % self.value)
        self.value += 1

# Start node
nd = PubNode()
nd.run()
