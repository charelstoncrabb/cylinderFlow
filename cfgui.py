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
	global root, filename
	filename = askopenfilename(parent=root, filetypes=[('cylFlow .out files', '*.out')])
	_plot()

def _newFile():
	global newfilename, filehandle
	newfilename = asksaveasfilename(parent=root, initialfile='NewMesh.dat')
	if len(newfilename):
		filehandle = open(newfilename,'w+')
		filehandle.write('ID X Y\n')
	else:
		print 'Please select a valid file.'

def _save():
	global filehandle, newNodeList, unmeshed
	if filehandle is not 0:
		pm.writeListToDatFile(filehandle,newNodeList)
		filehandle.close()
		filehandle = 0

def _saveAs():
	global filehandle, newNodeList, unmeshed
	filestring = asksaveasfilename(parent=root, initialfile='NewMesh.dat')
	if len(filestring):
		filehandle = open(filestring,'w+')
		filehandle.write('ID X Y\n')
		pm.writeListToDatFile(filehandle,newNodeList)
		filehandle.close()
		filehandle = 0

	else:
		print 'Please select a valid file.'

def _about():
    print "cylinderFlow version 0.1 - 2017 NCC"

def _mesh():
	global filename, filehandle, newfilename, unmeshed, newNodeList
	if unmeshed:
		_save()
		datfile = newfilename
	else:
		datfile = askopenfilename(parent=root, filetypes=[('cylFlow .dat files', '*.dat')])
	if len(datfile):
		filebase = datfile.split('.')
		filename = filebase[0] + '.out'
		if os.path.isfile('cylinderFlow'):
			command = "./cylinderFlow -i " + datfile + " -o " + filename + " -p off"
			os.system(command)
			_plot()
		else:
			print 'cylinderFlow needs to be compiled!'

def _plot():
	global canvas, filename, a, unmeshed
	if len(filename):
		pm.plotmesh(filename,a)
	if unmeshed:
		a.set_xlim([0,1])
		a.set_ylim([0,1])
		unmeshed = 0
	canvas.draw()

def _quit():
	root.quit()
	root.destroy()

def _editCurrent():
	print 'TODO: add editing functionality.'

def _on_key_event(event):
    key_press_handler(event, canvas, toolbar)
    if event.key == "super+o":
    	OpenFile()
    if event.key == "super+m":
    	_mesh()
    if event.key == "super+n":
    	_newFile()
    if event.key == "super+s":
    	_save()

def _clicked(event):
	global newNodeList, filehandle, a, canvas, unmeshed
	if event.inaxes and event.button == 1 and filehandle is not 0:
		newNode = pm.node(len(newNodeList)+1,event.xdata,event.ydata)
		newNodeList.append(newNode)
		unmeshed = 1
	if event.inaxes and event.button == 3 and filehandle is not 0:
		closestInd = pm.findClosest(newNodeList,event)
		print closestInd
		if closestInd in range(0,len(newNodeList)):
			newNodeList.pop(closestID)
	pm.scatter(newNodeList,a)
	canvas.draw()

# Global Variables:
root = tk.Tk()
root.wm_title("cylinderFlow - v0.1")

f = Figure(figsize=(6,5), dpi=150)
a = f.add_subplot(111)

filename = ""
newfilename = ""
filehandle = 0
newNodeList = []
unmeshed = 0

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
filemenu.add_command(label="Open", command=_openFile)
filemenu.add_separator()
filemenu.add_command(label="Save", command=_save)
filemenu.add_command(label="Save As", command=_saveAs)
filemenu.add_command(label="Exit", command=_quit)

editmenu = Menu(menu)
menu.add_cascade(label="Edit", menu=editmenu)
editmenu.add_command(label="Edit Current Mesh", command=_editCurrent)

helpmenu = Menu(menu)
menu.add_cascade(label="Help", menu=helpmenu)
helpmenu.add_command(label="About...", command=_about)

# Meshing button
button = tk.Button(master=root, text='Mesh', command=_mesh)
button.pack(side=tk.BOTTOM)

root.mainloop()