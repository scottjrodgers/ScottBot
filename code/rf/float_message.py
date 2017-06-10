"""
FloatMessage class
"""
from base_message import BaseMessage
from struct import pack, unpack

class FloatMessage(BaseMessage):
    def initialize(self):
        self.value = 0.0

    def encode(self):
        ba = pack('d',self.value)
        str64 = self.bytes_to_string(ba)
        return str64

    def decode(self, base64str):
        ba = self.string_to_bytes(base64str)
        data = unpack('d', ba)
        self.value = data[0]

if __name__ == '__main__':
    msg = FloatMessage()
    coded = msg.encode()
    print(coded)
    msg_2 = FloatMessage(coded)

    print("float: %f -> %f" % (msg.value, msg_2.value))

    assert msg.value == msg_2.value, 'float component does not match'
