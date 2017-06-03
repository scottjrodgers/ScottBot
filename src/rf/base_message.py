"""
BaseMessage class
"""

from binascii import a2b_base64, b2a_base64

class BaseMessage:
    def __init__(self):
        assert False, 'BaseMessage is a virtual class and should not be instantiated'


    def encode(self):
        """
        Encodes the message contents to a byte array, then encodes with Base64 to a string.

        This function must be implemented in each derived class

        :return: a Base64-encoded string
        """
        return "<to be implemented>"


    def decode(self, encoded_string):
        """
        Decodes the string into a byte array and then fills the instance members of the
        message object

        This function must by implemented in each derived class

        :param encoded_string
        :return: NA
        """
        pass


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
