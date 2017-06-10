"""
The base node class

TODOs:
 - add a subscriber to a global channel for global shutdown commands
 - do we need some sort of heartbeat system to automatically restart nodes that die?
 - Can we write a python file to launch all the nodes for a robot from a single python file,
   with each node launched in a separate process?
 - How would I pass in various options to override topic-names?

"""
import zmq
import random as rnd
import time
from parameters import get_port, get_new_port

class Node:
    def __init__(self):
        # base URL for zmq communications
        self.__base_url = 'tcp://127.0.0.1'

        # update rate
        self.rate = 25.0

        # polling rate
        self.poll_rate = 50.0

        # list of subscribers (topic names)
        self.__subscriber_list = []

        # dict of callbacks (topic name -> callback)
        self.__callbacks = dict()

        # dict of publisher sockets
        self.__pub_socks = dict()

        # dict of subscriber sockets
        self.__sub_socks = dict()

        # local cache port numbers for topics
        self.__port_cache = dict()

        # how often to check for port changes
        self.__pub_refresh = 100
        self.__sub_refresh = 500

        # dicts of refresh counts by topic
        self.__pub_refresh_count = dict()
        self.__sub_refresh_count = dict()

        # last update time
        self.__last_update_time = time.time()

        # done flag
        self.done = False


    def create_publisher(self, topicname):
        port = get_port(topicname)
        self.__port_cache[topicname] = port

        success = False
        while not success:
            ctx = zmq.Context()
            s = ctx.socket(zmq.PUB)
            bind_to = self.__base_url + ":" + str(port)
            s.bind(bind_to)
            print("publish to: %s", bind_to)
            ## TODO: How to check for actual success?
            success = True
            if success:
                self.__pub_socks[topicname] = s
            else:
                port = get_new_port(topicname)
                self.__port_cache[topicname] = port
        return topicname


    def create_subscriber(self, topicname, callback):
        port = get_port(topicname)
        self.__port_cache[topicname] = port
        self.__subscriber_list.append(topicname)
        self.__callbacks[topicname] = callback

        success = False
        ctx = zmq.Context()
        while not success:
            s = ctx.socket(zmq.SUB)
            bind_to = self.__base_url + ":" + str(port)
            s.connect(bind_to)
            print("subscribe to: %s", bind_to)
            s.setsockopt_string(zmq.SUBSCRIBE, '')
            ## TODO: How to check for actual success?
            success = True
            if success:
                self.__sub_socks[topicname] = s
            else:
                port = get_new_port(topicname)
                self.__port_cache[topicname] = port
        return topicname


    def __update_publisher_port(self, topicname):
        port = get_new_port(topicname)
        ctx = zmq.Context()
        success = False
        while not success:
            s = ctx.socket(zmq.PUB)
            bind_to = self.__base_url + ":" + str(port)
            s.bind(bind_to)
            print("publish to: %s", bind_to)
            ## TODO: How to check for actual success?
            success = True
            if success:
                self.__sub_socks[topicname] = s
            else:
                port = get_new_port(topicname)
                self.__port_cache[topicname] = port
        return topicname


    def __update_subscriber_port(self, topicname):
        port = get_new_port(topicname)
        ctx = zmq.Context()
        success = False
        while not success:
            s = ctx.socket(zmq.SUB)
            bind_to = self.__base_url + ":" + str(port)
            s.connect(bind_to)
            print("subscribe to: %s", bind_to)
            s.setsockopt_string(zmq.SUBSCRIBE, '')
            ## TODO: How to check for actual success?
            success = True
            if success:
                self.__sub_socks[topicname] = s
            else:
                port = get_new_port(topicname)
                self.__port_cache[topicname] = port
        return topicname


    def __poll_subscriber(self, topicname):
        if topicname not in self.__sub_refresh_count:
            self.__sub_refresh_count[topicname] = rnd.randint(0,self.__sub_refresh)
        self.__sub_refresh_count[topicname] += 1
        if self.__sub_refresh_count[topicname] > self.__sub_refresh:
            port = get_port(topicname)
            if self.__port_cache[topicname] != port:
                self.create_publisher(topicname)

        s = self.__sub_socks[topicname]
        rc = s.poll(0)
        if rc > 0:
            message = s.recv_pyobj()
            cb = self.__callbacks[topicname]
            cb(message)
            return True
        return False
        # TODO: exception handling?


    def publish_message(self, topicname, message):
        if topicname not in self.__pub_refresh_count:
            self.__pub_refresh_count[topicname] = rnd.randint(0,self.__pub_refresh)
        self.__pub_refresh_count[topicname] += 1
        if self.__pub_refresh_count[topicname] > self.__pub_refresh:
            port = get_port(topicname)
            if self.__port_cache[topicname] != port:
                self.create_publisher(topicname)

        s = self.__pub_socks[topicname]
        s.send_pyobj(message)
        # TODO: exception handling?


    def initialize(self):
        assert False, "Must be implemented"


    def mainloop(self):
        assert False, "Must be implemented"


    def run(self):
        ## run derived node initialization code
        self.initialize()

        ## any other universal initialization
        pass

        while not self.done:
            curr_time = time.time()
            update_delta = 1.0 / self.rate
            poll_delta = 1.0 / self.poll_rate

            # poll all subscribers
            for sub_topic in self.__subscriber_list:
                # more = True
                # while more:
                #     more = self.__poll_subscriber(sub_topic)
                self.__poll_subscriber(sub_topic)

            # call main loop if enough time has passed
            if curr_time - self.__last_update_time > update_delta:
                self.mainloop()
                self.__last_update_time = curr_time

            sleeptime = max(0.0, poll_delta - (time.time() - curr_time ))
            if sleeptime > 0.0:
                time.sleep(sleeptime)