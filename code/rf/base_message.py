"""
BaseMessage class
"""

from binascii import a2b_base64, b2a_base64

class BaseMessage:
    def initialize(self):
        assert False, "initialize() must be implemented in the derived class"

    def __init__(self, base64string=None):
        if base64string is not None:
            self.decode(base64string)
        else:
            self.initialize()


    def encode(self):
        """
        Encodes the message contents to a byte array, then encodes with Base64 to a string.

        This function must be implemented in each derived class.  Use struct

        :return: a Base64-encoded string
        """
        assert False, "encode() must be implemented in the derived class"
        return "<to be implemented>"


    def decode(self, encoded_string):
        """
        Decodes the string into a byte array and then fills the instance members of the
        message object

        This function must by implemented in each derived class

        :param encoded_string
        :return: NA
        """
        assert False, "decode() must be implemented in the derived class"


    def bytes_to_string(self, bin_data):
        """
        Base64-encodes a byte array to a string
        :param bytearray:
        :return: a base64-encoded string
        """

        string = b2a_base64(bin_data)

        return string


    def string_to_bytes(self, string):
        """
        Base64-decodes a string into byte array
        :param string: the base64-encoded string
        :return: a decoded byte array
        """
        bin_data = a2b_base64(string)

        return bin_data
