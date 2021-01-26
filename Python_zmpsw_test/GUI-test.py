from tkinter import *

import numpy as np

import threading
import time
import logging
import random
import queue

import plot_real_time
import plot_test

import clientADC
import clientCMD

import socket
#import time
import sys
import select

#BUF_SIZE = 10
#q = Queue.Queue(BUF_SIZE)
TCP_IP = '192.168.1.2'
TCP_PORT = 1001
BUFFER_SIZE = 1024

COMMANDS = {'START', 'STOP', 'EXIT', 'PLOT'}


class ConsumerThread(threading.Thread):
    def __init__(self, name=None, args=(), queue = queue.Queue(0), ):
        super(ConsumerThread,self).__init__()
        self.queue = queue

        self.size = 100
        self.x_vec = np.linspace(0,1,self.size+1)[0:-1]
        self.y_vec = np.zeros(len(self.x_vec))
        self.x = 0.0
        self.freq = 1/100
        self.y_vec = np.sin(self.x_vec)
        self.line1 = []
        self.ADC = clientADC.ZMPSW_ADC()


        return

    def run(self):
        command = ''
        start = False
        
        while True:
            if not self.queue.empty():
                command = self.queue.get()

                if(command == "EXIT"):
                    return
                elif(command == "START"):
                    start = True
                elif(command == "STOP"):
                    start = False
                elif(command == "PLOT"):
                    plot_test.plot_sine()
                    
            if(start == True):
                buffer = self.ADC.get_value()
                #print(self.ADC.get_value())
                data = np.frombuffer( buffer, dtype="int16" )
                data2 = data*0.805
                print(data2)
                print(data2.size)
                #print(self.name, ": random data:", random.randrange(1000))
                #self.__print()
            #time.sleep(0.01)

        #return

    def __print(self):
            self.x+=1
            #rand_val = np.sin(2*np.pi*self.x*self.freq)
            self.y_vec[-1] = self.ADC.get_value()
            self.line1 = plot_real_time.live_plotter(self.x_vec,self.y_vec,self.line1)
            self.y_vec = np.append(self.y_vec[1:],0.0)


class Window(Frame):


    def __init__(self, master=None, queue = queue.Queue(0)):

        self.queue = queue        
        Frame.__init__(self, master)   
        self.master = master
        self.init_window()

    #Creation of init_window
    def init_window(self):

        # changing the title of our master widget      
        self.master.title("ZMPSW")

        # allowing the widget to take the full space of the root window
        self.pack(fill=BOTH, expand=1)

        #channel list
        self.ChannelList = ["Channel 0"] 

        self.channel = StringVar(self)
        self.channel.set(self.ChannelList[0])
        self.ch_list = OptionMenu(self, self.channel, *self.ChannelList)

        # creating a button instance
        quitButton = Button(self, text="Exit", width=10, bg="snow",command=self.client_exit)
        continuous_startButton = Button(self, text="Start", width=10, bg="snow",command=self.continuous_start)
        continuous_stopButton = Button(self, text="Stop", width=10, bg="snow",command=self.continuous_stop)
        plot_button = Button(self, text = "Single", width=10, bg="snow", command = self.signle_callback)
        # entry
         
        self.samples_num = Entry(self, width = 10, text = "100")

        # placing the button on my window
        quitButton.place(x=10, y=10)
        continuous_startButton.place(x=10, y=40)
        continuous_stopButton.place(x=10, y=70)
        plot_button.place(x = 10, y = 100)
        self.samples_num.place( x = 100, y = 105)
        self.ch_list.place(x = 100, y = 40 )
       

    def continuous_start(self):
        cmd = clientCMD.ZMPSW_com() 
        cmd.send_msg("START_ADC 0")
        self.queue.put("START")



    def client_exit(self):
        print("Socket closed")
    
   
    def continuous_stop(self):
        cmd = clientCMD.ZMPSW_com()
        cmd.send_msg("STOP_ADC")
        self.queue.put("STOP")

    
    def signle_callback(self):
        cmd = clientCMD.ZMPSW_com()
        msg = "ADC_GET_SAMPLES "+self.samples_num.get()
        print(msg)
        cmd.send_msg(msg)
        

# root window created. Here, that would be the only window, but
# you can later have windows within windows.

# MAIN

root = Tk()
q = queue.Queue(100)

cons = ConsumerThread(queue=q, name='Consumer')

cons.start()
root.geometry("400x300")

#creation of an instance
app = Window(root, queue = q)

#mainloop 
root.mainloop()  