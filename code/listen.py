import sys
from rf.node import Node

class Listener(Node):
    def initilize(self):
        self.rate = 1.0;
        self.poll_rate=60.0;
        if len(self.args) != 2:
            print('Usage: listen.py <topicname>')
            exit(0)
        else:
            self.create_subscriber(self.args[1], self.callback)

    def callback(self, msg):
        print(msg)

    def mainloop(self):
        pass

if __name__ == '__main__':
    n = Listener(sys.argv)
    n.run()
