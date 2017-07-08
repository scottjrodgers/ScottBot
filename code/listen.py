import sys
from rf.node import Node

class Listener(Node):
    def initialize(self):
        self.rate = 1.0;
        self.poll_rate=60.0;
        if len(self.args) != 2:
            print('\n\nUsage: listen.py <topicname>\n')
            exit(0)
        else:
            self.topic = self.args[1]
            self.create_subscriber(self.topic, self.callback)

    def disp_dict(self, d, indent):
        for key, val in d.items():
            if isinstance(val, dict):
                print('%s%s:' % (' '*indent, key))
                self.disp_dict(val, indent + 2)
            else:
                print('%s%s:' % (' '*indent, key), val)
    def callback(self, msg):
        print("Message: %s" % self.topic)
        self.disp_dict(msg, 2)

    def mainloop(self):
        pass

if __name__ == '__main__':
    n = Listener(sys.argv)
    n.run()
