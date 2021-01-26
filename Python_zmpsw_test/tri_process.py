import multiprocessing as mp
import time

import matplotlib.pyplot as plt
import numpy as np

class ProcessPlotter(object):
    def __init__(self):
        self.x = []
        self.y = []

    def terminate(self):
        plt.close('all')

    def call_back(self):
        while self.pipe.poll():
            command = self.pipe.recv()
            if command is None:
                self.terminate()
                return False
            else:
                self.x.append(command[0])
                self.y.append(command[1])
                self.ax.plot(self.x, self.y, 'ro')
        self.fig.canvas.draw()
        return True

    def __call__(self, pipe):
        print('starting plotter...')

        self.pipe = pipe
        self.fig, self.ax = plt.subplots()
        timer = self.fig.canvas.new_timer(interval=10)
        timer.add_callback(self.call_back)
        timer.start()

        print('...done')
        plt.show()


def generator()
    send = self.plot_pipe.send
    if finished:
        send(None)
    else:
        self.x+=1
        rand_val = np.sin(2*np.pi*self.x*self.freq)
        data = [self.x,rand_val]
        send(data)
"""
class NBPlot(object):
    def __init__(self):
        
        self.x = 0.0
        self.freq = 1/100
        self.plot_pipe = None
        self.plot_pipe, plotter_pipe = mp.Pipe()
        self.plotter = ProcessPlotter()
        self.plot_process = mp.Process(
            target=self.plotter, args=(plotter_pipe,), daemon=True)
        self.plot_process.start()

    def plot(self, finished=False):
        send = self.plot_pipe.send
        if finished:
            send(None)
        else:
            self.x+=1
            rand_val = np.sin(2*np.pi*self.x*self.freq)
            data = [self.x,rand_val]
            send(data)

    def __call__(self, pipe):
        print('starting plotter...')

        self.plot_pipe= pipe
        self.fig, self.ax = plt.subplots()
        timer = self.fig.canvas.new_timer(interval=100)
        timer.add_callback(self.plot)
        timer.start()

        print('...done')
        plt.show()
"""

def main():

    plotter_pipe = mp.Pipe()
    plotter = ProcessPlotter()
    plot_process = mp.Process(target=plotter, args=(plotter_pipe,), daemon=True)
    plot_process.start()
    pl = NBPlot()
    for ii in range(100):
        pl.plot()
        time.sleep(0.1)
    pl.plot(finished=True)


if __name__ == '__main__':
    if plt.get_backend() == "MacOSX":
        mp.set_start_method("forkserver")
    main()
