"""
GUI for displaying the drone status. Requires pyserial, tkinter, numpy, matplotlib and Python3 
make upload-run-gui
"""
from joystick import js
import sys
import tkinter as tk
import tkinter.ttk as ttk
from matplotlib.pyplot import plot, tight_layout
import serial
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
from matplotlib.figure import Figure
import struct


usb = '/dev/ttyUSB0'
baud_rate = 115200
timeout = 0.1

positions = [(-5, -1, -1), (1, -1, -1), (-1, -5, -1), (-1, 1, -1), (-1, -1, -1)]
sizes = [(4, 2, 2), (4, 2, 2), (2, 4, 2), (2, 4, 2), (2, 2, 2)]
colors = ["limegreen", "crimson", "crimson", "crimson", "black"]

valid_keys = {'space':32, 'Escape':27, 'Left':43, 'Right':44, 'Up':41, 'Down':42,
              "a":'a', "z":'z', "q":'q', "w":'w', "j":'j', "u":'u', "k":'k', "i":'i', "l":'l', "o":'o',
              "1":'1', "2":'2', "3":'3', "4":'4', "5":'5', "6":'6', "7":'7', "8":'8', "9":'9', "0":'0'}



def Rx(theta):
  return np.matrix([[ 1, 0            , 0            ],
                    [ 0, np.cos(theta),-np.sin(theta)],
                    [ 0, np.sin(theta), np.cos(theta)]])
  
def Ry(theta):
  return np.matrix([[ np.cos(theta), 0, np.sin(theta)],
                    [ 0            , 1, 0            ],
                    [-np.sin(theta), 0, np.cos(theta)]])
  
def Rz(theta):
  return np.matrix([[ np.cos(theta), -np.sin(theta), 0 ],
                    [ np.sin(theta), np.cos(theta) , 0 ],
                    [ 0            , 0             , 1 ]])

def cuboid(o, size=(1,1,1)):
    X = [[[0, 1, 0], [0, 0, 0], [1, 0, 0], [1, 1, 0]],
         [[0, 0, 0], [0, 0, 1], [1, 0, 1], [1, 0, 0]],
         [[1, 0, 1], [1, 0, 0], [1, 1, 0], [1, 1, 1]],
         [[0, 0, 1], [0, 0, 0], [0, 1, 0], [0, 1, 1]],
         [[0, 1, 0], [0, 1, 1], [1, 1, 1], [1, 1, 0]],
         [[0, 1, 1], [0, 0, 1], [1, 0, 1], [1, 1, 1]]]
    X = np.array(X).astype(float)
    for i in range(3):
        X[:,:,i] *= size[i]
    X += np.array(o)
    return X

def plot_cuboids(positions, angles=[0,0,0], sizes=None, colors=None, **kwargs):
    psi, theta, phi = angles
    R = Rz(psi) * Ry(theta) * Rx(phi)
    if not isinstance(colors,(list,np.ndarray)): colors=["C0"]*len(positions)
    if not isinstance(sizes,(list,np.ndarray)): sizes=[(1,1,1)]*len(positions)
    g = []
    for p,s,c in zip(positions,sizes,colors):
        g.append(cuboid(p, size=s))
    for k in range(len(g)):
        for i in range(6):
            for j in range(4):
                g[k][i][j] = g[k][i][j] * R.T
    return Poly3DCollection(np.concatenate(g), facecolors=np.repeat(colors,6), **kwargs)


def convert_euler(angles, range=[-32768, 32767]):
    radians = []
    for angle in angles:
        angle = (((float(angle)-range[0])*2*np.pi) / (range[1]-range[0])) + (-np.pi)
        radians.append(angle)
    return radians




class GuiApp:
    def add_field(self, x=0.1, y=0.1, w=0.1, h=0.05, text=' '):
        self.fields.append(ttk.Label(self.toplevel1))
        self.fields[-1].configure(anchor='w', font='{Ubuntu} 12 {}', foreground='black', justify='center')
        self.fields[-1].configure(relief='flat', state='normal', takefocus=False, text=text)
        self.fields[-1].place(anchor='nw', bordermode='inside', relheight=str(h), relwidth=str(w), relx=str(x), rely=str(y))

    def add_message(self, x=0.1, y=0.1, w=0.1, h=0.05, key=' '):
        self.messages[key] = ttk.Label(self.toplevel1)
        self.messages[key].configure(anchor='w', background='white', font='{ubuntu mono} 12 {}', foreground='black')
        self.messages[key].configure(justify='center', relief='raised', state='normal', takefocus=False)
        self.messages[key].place(anchor='nw', bordermode='inside', relheight=str(h), relwidth=str(w), relx=str(x), rely=str(y))


    def __init__(self, master=None):
        self.toplevel1 = tk.Toplevel(master, container='false')
        self.fields = []
        self.messages = {}

        self.add_field(0.1, 0.1, 0.1, 0.05, 'battery')
        self.add_message(0.2, 0.1, 0.1, 0.05, 'battery')

        self.add_field(0.1, 0.2, 0.1, 0.05, 'pressure')
        self.add_message(0.2, 0.2, 0.1, 0.05, 'pressure')

        self.add_field(0.08, 0.3, 0.13, 0.05, 'temperature')
        self.add_message(0.2, 0.3, 0.1, 0.05, 'temperature')


        self.add_field(0.07, 0.45, 0.07, 0.05, 'm0')
        self.add_message(0.12, 0.45, 0.07, 0.05, 'mot0')

        self.add_field(0.07, 0.55, 0.07, 0.05, 'm1')
        self.add_message(0.12, 0.55, 0.07, 0.05, 'mot1')

        self.add_field(0.22, 0.45, 0.07, 0.05, 'm2')
        self.add_message(0.27, 0.45, 0.07, 0.05, 'mot2')

        self.add_field(0.22, 0.55, 0.07, 0.05, 'm3')
        self.add_message(0.27, 0.55, 0.07, 0.05, 'mot3')


        self.add_field(0.05, 0.7, 0.1, 0.05, 'ctrl yaw')
        self.add_message(0.12, 0.7, 0.07, 0.05, 'js_yaw')

        self.add_field(0.05, 0.8, 0.1, 0.05, 'ctrl roll')
        self.add_message(0.12, 0.8, 0.07, 0.05, 'js_roll')

        self.add_field(0.2, 0.7, 0.1, 0.05, 'ctrl pitch')
        self.add_message(0.27, 0.7, 0.07, 0.05, 'js_pitch')

        self.add_field(0.2, 0.8, 0.1, 0.05, 'ctrl lift')
        self.add_message(0.27, 0.8, 0.07, 0.05, 'js_lift')


        self.add_field(0.48, 0.6, 0.1, 0.05, 'yaw')
        self.add_message(0.53, 0.6, 0.07, 0.05, 'yaw')

        self.add_field(0.48, 0.7, 0.1, 0.05, 'roll')
        self.add_message(0.53, 0.7, 0.07, 0.05, 'roll')

        self.add_field(0.48, 0.8, 0.1, 0.05, 'pitch')
        self.add_message(0.53, 0.8, 0.07, 0.05, 'pitch')


        self.add_field(0.72, 0.6, 0.1, 0.05, 'gyro x')
        self.add_message(0.8, 0.6, 0.07, 0.05, 'gx')

        self.add_field(0.72, 0.7, 0.1, 0.05, 'gyro y')
        self.add_message(0.8, 0.7, 0.07, 0.05, 'gy')

        self.add_field(0.72, 0.8, 0.1, 0.05, 'gyro z')
        self.add_message(0.8, 0.8, 0.07, 0.05, 'gz')

        self.add_field(0.72, 0.9, 0.1, 0.05, 'mode')
        self.add_message(0.8, 0.9, 0.1, 0.05, 'mode')


        self.button = tk.Button(self.toplevel1, command=self.toggle)
        self.button.configure(anchor='w', font='{ubuntu} 14 {}', justify='center')
        self.button.configure(relief='raised', state='normal', takefocus=False, text='START', bg='green', fg='white')
        self.button.place(anchor='nw', bordermode='inside', relheight='0.05', relwidth='0.1', relx='0.15', rely='0.9')

        self.plot = tk.Canvas(self.toplevel1)
        self.plot.configure(background='white', insertborderwidth='0', state='normal')
        self.plot.place(anchor='nw', bordermode='inside', relheight='0.45', relwidth='0.5', relx='0.4', rely='0.1')

        self.fig = Figure(dpi=100, tight_layout=True)
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.plot)  
        self.ax = self.fig.add_subplot(111, projection="3d")
        self.toolbar = NavigationToolbar2Tk(self.canvas, self.plot)
        self.toolbar.update()
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
        self.canvas.draw()

        self.toplevel1.configure(cursor='arrow', height='600', takefocus=False, width='1000')
        self.toplevel1.resizable(True, True)
        self.toplevel1.title('Demo GUI')

        self.toplevel1.bind('<KeyPress>', self.onKeyPress)
        self.toplevel1.bind('<Left>', self.onKeyPress)
        self.toplevel1.bind('<Right>', self.onKeyPress)
        self.toplevel1.bind('<Up>', self.onKeyPress)
        self.toplevel1.bind('<Down>', self.onKeyPress)

        # Main widget
        self.mainwindow = self.toplevel1
        self.started = False
        self.mainwindow.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.key = ''
    

    def toggle(self):
        if self.button.config('relief')[-1] == 'raised':
            self.button.config(relief="sunken", text='STOP', bg='red', fg='white')
            self.started = True
            self.ser = serial.Serial(usb, baud_rate, timeout=timeout)
        else:
            self.button.config(relief="raised", text='START', bg='green', fg='white')
            self.started = False
            self.ser.close()

    def send_data(self):
        js.update()
        js_data = js.process_input()
        key = 255
        if self.key in valid_keys.keys():
            key = valid_keys[self.key]
            self.key = 255
        js_bytes = [struct.pack('>h', int(b)) for b in js_data[0:4]]
        data_pkg = [b'\xAA']
        if js_data[-1] == '1':
            key = '1'
        if isinstance(key, int):
            data_pkg.append(struct.pack('>B', key))
        else: 
            data_pkg.append(struct.pack('>B', ord(key)))
        data_pkg.extend(js_bytes)
        data_pkg.append(b'\xA0')
        data_pkg = b''.join(data_pkg)
        self.ser.write(bytes(data_pkg))

    def onKeyPress(self, event):
        self.key = event.keysym

    def on_closing(self):
        self.mainwindow.destroy()
        sys.exit()
        
    def run(self):
        self.update_clock()
        self.mainwindow.mainloop()


    def plot_drone(self, roll, pitch, yaw):
        self.ax.clear()
        pc = plot_cuboids(positions, (yaw, pitch, roll), sizes=sizes, colors=colors, edgecolor="k")
        self.ax.add_collection3d(pc)   
        self.ax.set_xlim([-7, 7])
        self.ax.set_ylim([-7, 7])
        self.ax.set_zlim([-7, 7])
        self.ax.grid(False)
        self.canvas.draw()
        

    def update_clock(self):
        if self.started:
            self.send_data()
            try:
                self.ser.reset_input_buffer()
                l = self.ser.read_until()
                data_len = len(l)
                while data_len == 0:
                    l = self.ser.read_until()
                    data_len = len(l) 
                data = [str(i)[2:-1] for i in l.split(b' ')]
                if data[0] == 'gui':
                    roll, pitch, yaw = convert_euler(data[10:13])
                    self.plot_drone(roll, pitch, yaw)

                    self.messages['mode'].config(text=str(data[1]))
                    self.messages['js_yaw'].config(text=str(data[2]))
                    self.messages['js_roll'].config(text=str(data[3]))
                    self.messages['js_pitch'].config(text=str(data[4]))
                    self.messages['js_lift'].config(text=str(data[5]))

                    self.messages['mot0'].config(text=str(data[6]))
                    self.messages['mot1'].config(text=str(data[7]))
                    self.messages['mot2'].config(text=str(data[8]))
                    self.messages['mot3'].config(text=str(data[9]))

                    self.messages['roll'].config(text=str(np.rad2deg(roll)))
                    self.messages['pitch'].config(text=str(np.rad2deg(pitch)))
                    self.messages['yaw'].config(text=str(np.rad2deg(yaw)))

                    self.messages['gx'].config(text=str(data[13]))
                    self.messages['gy'].config(text=str(data[14]))
                    self.messages['gz'].config(text=str(data[15]))

                    self.messages['battery'].config(text=str(data[16]))
                    self.messages['temperature'].config(text=str(data[17]))
                    self.messages['pressure'].config(text=str(data[18]))

                elif data[0] == 'debug':
                    print(' '.join(data[1:-1]))

            except Exception as e: print(e)
        
        self.mainwindow.after(1, self.update_clock) 



if __name__ == '__main__':
    tk.Tk().withdraw()
    app = GuiApp()
    app.run() 

