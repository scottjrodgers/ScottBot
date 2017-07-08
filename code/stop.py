from rf.node import Node
import time

n = Node()
pub = n.create_publisher('cmd-vel')

for i in range(10):
    n.publish_message(pub, dict(vx=0, rz=0))
    time.sleep(0.1)