"""
Subscriber Class
"""

from parameters import get_topic_id
import zmq

class Subscriber:
    def __init__(self, topic_name, msg_template, callback):
        self.topic_name = topic_name
        self.topic_id = get_topic_id(topic_name)
        self.msg_template = msg_template
        self.callback = callback
        self.port = 5558
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.SUB)
        self.socket.bind("tcp://127.0.0.1:%d" % self.port)
        self.topicfilter = "%d" % self.topic_id
        self.socket.setsockopt_string(zmq.SUBSCRIBE, self.topicfilter)

    def check(self):
        events = self.socket.poll(timeout=0, flags=zmq.POLLIN)
        if events != 0:
            message = self.socket.recv_string()


            msg = type(self.msg_template)(string)
            self.callback(msg)
