"""
Subscriber Class
"""

from parameters import get_topic_id
import zmq

class Subscriber:
    def __init__(self, topic_name, msg_template):
        self.topic_name = topic_name
        self.topic_id = get_topic_id(topic_name)
        self.msg_template = msg_template
        self.port = 5556
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.SUB)
        self.socket.bind("tcp://127.0.0.1:%s" % self.port)

    topicfilter = "10001"
    socket.setsockopt_string(zmq.SUBSCRIBE, topicfilter)

    def check(self):

        msg_data = msg.encode()
        self.socket.send("%d %d" % (self.topic_id), msg_data)


if __name__ == '__main__':
    x = Publisher('A')
    y = type(x)('B')
    print(x.topic_name)
    print(y.topic_name)