from tkinter import *

import numpy as np

import threading
import time
import logging
import random
import queue

import plot_real_time
import plot_test
#BUF_SIZE = 10
#q = Queue.Queue(BUF_SIZE)


COMMANDS = {'START', 'STOP', 'EXIT', 'PLOT'}


class ConsumerThread(threading.Thread):
    def __init__(self, name=None, args=(), queue = queue.Queue(0), ):
        super(ConsumerThread,self).__init__()
        self.queue = queue

#        self.size = 100
#        self.x_vec = np.linspace(0,1,self.size+1)[0:-1]
#        self.y_vec = np.zeros(len(self.x_vec))
#        self.x = 0.0
#        self.freq = 1/100
#        self.y_vec = np.sin(self.x_vec)
#        self.line1 = []


        return

    def run(self):
        command = ''
        start = False
        
        while True:
            if not self.queue.empty():
                command = self.queue.get()
              #  self.__print("queue get " + command)

                if(command == "EXIT"):
                 #   self.__print("exit")
                    return
                elif(command == "START"):
                    start = True
                   # self.__print("start printing data")
                elif(command == "STOP"):
                    start = False
                  #  self.__print("stop printing data")
                elif(command == "PLOT"):
                    plot_test.plot_sine()
                    
            if(start == True):
                print(self.name, ": random data:", random.randrange(1000))
                self.__print()
            time.sleep(0.01)

        return

    def __print(self):
            #print(self.name,"  ", data)
            self.x+=1
            rand_val = np.sin(2*np.pi*self.x*self.freq)
            self.y_vec[-1] = rand_val
            self.line1 = plot_real_time.live_plotter(self.x_vec,self.y_vec,self.line1)
            self.y_vec = np.append(self.y_vec[1:],0.0)

# Here, we are creating our class, Window, and inheriting from the Frame
# class. Frame is a class from the tkinter module. (see Lib/tkinter/__init__)
class Window(Frame):

    # Define settings upon initialization. Here you can specify
    def __init__(self, master=None, queue = queue.Queue(0)):

        self.queue = queue        
        # parameters that you want to send through the Frame class. 
        Frame.__init__(self, master)   

        #reference to the master widget, which is the tk window                 
        self.master = master

        #with that, we want to then run init_window, which doesn't yet exist
        self.init_window()

    #Creation of init_window
    def init_window(self):

        # changing the title of our master widget      
        self.master.title("MARSZLEK PROGRAMUJE")

        # allowing the widget to take the full space of the root window
        self.pack(fill=BOTH, expand=1)

        # creating a button instance
        quitButton = Button(self, text="Exit",command=self.client_exit)
        continuous_startButton = Button(self, text="Start",command=self.continuous_start)
        continuous_stopButton = Button(self, text="Stop",command=self.continuous_stop)
        plot_button = Button(self, text = "Plot", command = self.plot_callback)
        # placing the button on my window
        continuous_startButton.place(x=0, y=20)
        continuous_stopButton.place(x=0, y=40)
        quitButton.place(x=0, y=0)
        plot_button.place(x = 0, y = 60)
       

    def continuous_start(self):
        self.queue.put("START")

    def client_exit(self):
        self.queue.put("EXIT")
   
    def continuous_stop(self):
        self.queue.put("STOP")
    
    def plot_callback(self):
        self.queue.put("PLOT")

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