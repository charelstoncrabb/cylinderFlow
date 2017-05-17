#!/usr/bin/python

import matplotlib
matplotlib.use('TkAgg')

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
# implement the default mpl key bindings
from matplotlib.backend_bases import key_press_handler
from matplotlib.figure import Figure
import sys
import os

from Tkinter import *
import Tkinter as Tk
from tkFileDialog import askopenfilename
import ProcScripts.plotmesh as pm

root = Tk.Tk()
root.wm_title("cylinderFlow - v0.1")
filename = ""


f = Figure(figsize=(5, 4), dpi=150)
a = f.add_subplot(111)

# a tk.DrawingArea
canvas = FigureCanvasTkAgg(f, master=root)
canvas.show()
canvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)

toolbar = NavigationToolbar2TkAgg(canvas, root)
toolbar.update()
canvas._tkcanvas.pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)


# Callbacks:
def OpenFile():
	global root
	global filename
	filename = askopenfilename(parent=root, filetypes=[('cylFlow .out files', '*.out')])
	_plot()
    
def About():
    print "cylinderFlow version 0.1 - 2017 NCC"

def _mesh():
	global filename
	datfile = askopenfilename(parent=root, filetypes=[('cylFlow .dat files', '*.dat')])
	filebase = datfile.split('.')
	filename = filebase[0] + '.out'
	command = "./cylinderFlow -i " + datfile + " -o " + filename + " -p off >> Meshing.out"
	os.system(command)
	_plot()

def _plot():
	global canvas
	global a
	a.cla()
	pm.plotmesh(filename,a)
	canvas.draw()

def _quit():
    root.quit()
    root.destroy()

def on_key_event(event):
    key_press_handler(event, canvas, toolbar)
    if event.key == "super+o":
    	OpenFile()
    if event.key == "super+m":
    	_mesh()

canvas.mpl_connect('key_press_event', on_key_event)

# App menu
menu = Menu(root)
root.config(menu=menu)
filemenu = Menu(menu)
menu.add_cascade(label="File", menu=filemenu)
#filemenu.add_command(label="New", command=NewFile)
filemenu.add_command(label="Open...", command=OpenFile)
filemenu.add_separator()
filemenu.add_command(label="Exit", command=_quit)

helpmenu = Menu(menu)
menu.add_cascade(label="Help", menu=helpmenu)
helpmenu.add_command(label="About...", command=About)

# Meshing button
button = Tk.Button(master=root, text='Mesh', command=_mesh)
button.pack(side=Tk.BOTTOM)

Tk.mainloop()
