import threading
import time
import logging
import random
import queue

#BUF_SIZE = 10
#q = Queue.Queue(BUF_SIZE)


COMMANDS = {'START', 'STOP', 'EXIT'}


class ProducerThread(threading.Thread):
    def __init__(self, group=None, target=None, name=None,
                 args=(), kwargs=None, verbose=None, queue = queue.Queue(0), debug = 'False'):
        super(ProducerThread,self).__init__()
        self.target = target
        self.name = name
        self.queue = queue
        self.debug = debug

    def run(self):
        try:
            self.__print("wait 1s")
            time.sleep(1)
            self.__print("Queue, put START")
            self.queue.put("START")
            self.__print("wait 5s")
            time.sleep(5)
            self.__print("Queue, put STOP")
            self.queue.put("STOP")
            self.__print("wait 5s")
            time.sleep(5)
            self.__print("wait 10s")
            time.sleep(10)
            self.__print("Queue, put EXIT")
            self.queue.put("EXIT")
        except:
            pass


    def __print(self, data):
        if(self.debug):
            print(self.name,"  ", data)



class ConsumerThread(threading.Thread):
    def __init__(self, group=None, target=None, name=None,
                 args=(), kwargs=None, verbose=None, queue = queue.Queue(0), debug = 'False'):
        super(ConsumerThread,self).__init__()
        self.target = target
        self.name = name
        self.queue = queue
        self.debug = debug



        return

    def run(self):
        command = ''
        start = False
        
        while True:
            if not self.queue.empty():
                command = self.queue.get()
                self.__print("queue get " + command)

                if(command == "EXIT"):
                    self.__print("exit")
                    return
                elif(command == "START"):
                    start = True
                    self.__print("start printing data")
                elif(command == "STOP"):
                    start = False
                    self.__print("stop printing data")


            if(start == True):
                print(self.name, ": random data:", random.randrange(1000))

            time.sleep(0.01)

        return

    def __print(self, data):
        if(self.debug):
            print(self.name,"  ", data)




if __name__ == '__main__':

    q = queue.Queue(100)
    prod = ProducerThread(debug=True, queue=q, name='Producer')
    cons = ConsumerThread(debug=True, queue=q, name='Consumer')

    prod.start()
    cons.start()