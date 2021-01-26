import socket
import time
import sys
import select

UDP_IP = '192.168.1.5'
UDP_PORT = 8090

"""
sock = socket.socket(socket.AF_INET, # Internet
socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    data = int.from_bytes(data, "little")
    print("received message x: %s" % data)
"""
class ZMPSW_ADC():

    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        self.sock.bind((UDP_IP, UDP_PORT))

    def get_value(self):
        data = self.sock.recvfrom(2024) # buffer size is 1024 bytes
        data2 = int.from_bytes(data[0], "little")
        return data[0]

    def get(self, socket, timeout):
        ready = select.select([socket], [], [], timeout)
        if not ready[0]:
            raise ValueError('Get timeout')
        dataSize = int.from_bytes(socket.recv(1), "big")
        data = s.recv(dataSize)
        return int.from_bytes(data, "big")
