#!/usr/bin/python

import matplotlib
matplotlib.use('TkAgg')
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.backend_bases import key_press_handler, MouseEvent
from matplotlib.pyplot import Figure
import sys
import os
import Tkinter as tk
from Tkinter import Menu
from tkFileDialog import *
import ProcScripts.plotmesh as pm
import numpy as np

# Callbacks:
def _openFile():
	global root
	global filename
	filename = askopenfilename(parent=root, filetypes=[('cylFlow .out files', '*.out')])
	_plot()

def _newFile():
	global filehandle
	filename = asksaveasfilename(parent=root, filetypes=[('cylFlow .dat files', '*.dat')])
	print filename

def _about():
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

def _on_key_event(event):
    key_press_handler(event, canvas, toolbar)
    if event.key == "super+o":
    	OpenFile()
    if event.key == "super+m":
    	_mesh()
    if event.key == "super+n":
    	_newFile()

def _clicked(event):
	print "button: ", event.button
	print "clicked at", event.xdata, event.ydata

# Global Variables:
root = tk.Tk()
root.wm_title("cylinderFlow - v0.1")

f = Figure(figsize=(6,5), dpi=150)
a = f.add_subplot(111)

filename = ""
filehandle = 0

# Plotting canvas:
canvas = FigureCanvasTkAgg(f, master=root)
canvas.show()
canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)

canvas.mpl_connect('key_press_event',_on_key_event)
canvas.mpl_connect('button_press_event',_clicked)

toolbar = NavigationToolbar2TkAgg(canvas,root)
toolbar.update()
canvas._tkcanvas.pack(side=tk.TOP, fill=tk.BOTH, expand=1)
toolbar

# App menu:
menu = Menu(root)
root.config(menu=menu)
filemenu = Menu(menu)
menu.add_cascade(label="File", menu=filemenu)
filemenu.add_command(label="New", command=_newFile)
filemenu.add_command(label="Open...", command=_openFile)
filemenu.add_separator()
filemenu.add_command(label="Exit", command=_quit)

helpmenu = Menu(menu)
menu.add_cascade(label="Help", menu=helpmenu)
helpmenu.add_command(label="About...", command=_about)

# Meshing button
button = tk.Button(master=root, text='Mesh', command=_mesh)
button.pack(side=tk.BOTTOM)

root.mainloop()