"""
Testcase for BaseMessage class
"""
from base_message import BaseMessage
from struct import pack, unpack

class TestMessage(BaseMessage):
    def initialize(self):
        self.int = 12345
        self.float = 1.23456789
        self.string = "Test"

    def encode(self):
        ba = pack('id4s',self.int, self.float, self.string.encode('UTF-8'))
        print("num bytes = %d" % len(ba))
        str64 = self.bytes_to_string(ba)
        print("str_len = %d" % len(str64))
        return str64

    def decode(self, base64str):
        ba = self.string_to_bytes(base64str)
        data = unpack('id4s', ba)
        self.int = data[0]
        self.float = data[1]
        self.string = data[2].decode('UTF-8')

if __name__ == '__main__':
    msg = TestMessage()
    coded = msg.encode()
    print(coded)
    msg_2 = TestMessage(coded)

    print("int: %d -> %d" % (msg.int, msg_2.int))
    print("float: %f -> %f" % (msg.float, msg_2.float))
    print("string: %s -> %s" % (msg.string, msg_2.string))

    assert msg.int == msg_2.int, 'integer component does not match'
    assert msg.float == msg_2.float, 'float component does not match'
    assert msg.string == msg_2.string, 'string component does not match'
