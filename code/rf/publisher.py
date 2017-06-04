"""
Publisher Class
"""

from base_message import BaseMessage
from parameters import get_topic_id
import zmq

class Publisher:
    def __init__(self, topic_name):
        self.topic_name = topic_name
        self.topic_id = get_topic_id(topic_name)


if __name__ == '__main__':
    x = Publisher('A')
    y = type(x)('B')
    print(x.topic_name)
    print(y.topic_name)