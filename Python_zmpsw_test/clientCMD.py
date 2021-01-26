import socket
import time
import sys
import select

TCP_IP = '192.168.1.2'
TCP_PORT = 1001
BUFFER_SIZE = 1024

class ZMPSW_com():

    def __init__(self):
        self.x = 0

    def send_msg(self, MESSAGE):
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print("Socket created")
            s.connect((TCP_IP, TCP_PORT))
            print("Connected to server")
            self.write(s, MESSAGE)
            print("Request send:", MESSAGE)
            data = self.get(s, 4)
            print("Response:", data)
            s.close()
            print("Socket closed")
        except:
            print("Failed, error:", sys.exc_info()[0])
            s.close()


    def get(self, socket, timeout):
        ready = select.select([socket], [], [], timeout)
        if not ready[0]:
            raise ValueError('Get timeout')
        dataSize = int.from_bytes(socket.recv(1), "big")
        data = socket.recv(dataSize)
        return data.decode("utf-8") 

    def write(self, socket, data_string):
        dataSize=len(data_string)
        #send data size
        socket.send(dataSize.to_bytes(1, byteorder='big'))
        #send data
        socket.send(bytes(data_string, 'utf-8'))
    
""" 
while True:
    
    MESSAGE = "START_ADC"
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print("Socket created")
        s.connect((TCP_IP, TCP_PORT))
        print("Connected to server")
        write(s, MESSAGE)
        print("Request send:", MESSAGE)
        data = get(s, 4)
        print("Response:", data)
        s.close()
        print("Socket closed")
    except:
        print("Failed, error:", sys.exc_info()[0])
        s.close()
        continue
    time.sleep(10)

    MESSAGE = "STOP_ADC"
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print("Socket created")
        s.connect((TCP_IP, TCP_PORT))
        print("Connected to server")
        write(s, MESSAGE)
        print("Request send:", MESSAGE)
        data = get(s, 4)
        print("Response:", data)
        s.close()
        print("Socket closed")
    except:
        print("Failed, error:", sys.exc_info()[0])
        s.close()
        continue
    time.sleep(10)

"""