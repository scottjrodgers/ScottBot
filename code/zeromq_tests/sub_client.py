"""

"""
import zmq
import sys

port = '5556'

if len(sys.argv) > 1:
    port = sys.argv[1]
    int(port)

context = zmq.Context()
socket = context.socket(zmq.SUB)

print("Collecting updates...")

socket.connect("tcp://localhost:%s" % port)

topicfilter = "10001"
socket.setsockopt_string(zmq.SUBSCRIBE, topicfilter)

total_value = 0
for update_nbr in range(15):
    string = socket.recv()
    topic, messagedata = string.split()
    total_value += int(messagedata)
    print("%s %s" % (topic, messagedata))


