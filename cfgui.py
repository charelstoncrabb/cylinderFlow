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
	global newfilename, filehandle, newNodeList, a, canvas
	newfilename = asksaveasfilename(parent=root, initialfile='NewMesh.dat')
	if len(newfilename):
		filehandle = open(newfilename,'w+')
		filehandle.write('ID X Y\n')
		del newNodeList
		newNodeList = []
		a.cla()
		canvas.draw()
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
	global filehandle, unmeshed, newNodeList
	if filehandle is not 0 and not unmeshed:
		print 'TODO'
		# Make sure newNodeList has all the currently meshed nodes, and newNodeID is OK
		# Then proceed with same interface as when creating a new mesh
	else:
		print 'Open or create mesh for editing.'

def _on_key_event(event):
    key_press_handler(event, canvas, toolbar)
    if event.key == "super+o":
    	_openFile()
    if event.key == "super+m":
    	_mesh()
    if event.key == "super+n":
    	_newFile()
    if event.key == "super+s":
    	_save()
    if event.key == "super+e":
    	_editCurrent()

def _clicked(event):
	global newNodeList, newNodeID, filehandle, a, canvas, unmeshed
	if event.inaxes and filehandle is not 0:
		if event.button == 1:
			newNodeID += 1
			newNode = pm.node(newNodeID,event.xdata,event.ydata)
			newNodeList.append(newNode)
		if event.button == 3:
			closestInd = pm.findClosest(newNodeList,event)
			if closestInd in range(0,len(newNodeList)):
				del newNodeList[closestInd]
		pm.scatter(newNodeList,a)
		unmeshed = 1
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
newNodeID = 0
unmeshed = 0
editing = 0

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
filemenu.add_command(label="New", command=_newFile, accelerator="Cmd-N")
filemenu.add_command(label="Open", command=_openFile, accelerator="Cmd-O")
filemenu.add_separator()
filemenu.add_command(label="Save", command=_save, accelerator="Cmd-S")
filemenu.add_command(label="Save As", command=_saveAs)
filemenu.add_command(label="Quit", command=_quit, accelerator="Cmd-Q")

editmenu = Menu(menu)
menu.add_cascade(label="Edit", menu=editmenu)
editmenu.add_command(label="Edit Current Mesh", command=_editCurrent, accelerator="Cmd-E")

helpmenu = Menu(menu)
menu.add_cascade(label="Help", menu=helpmenu)
helpmenu.add_command(label="About...", command=_about, accelerator="Cmd-A")

# Meshing button
button = tk.Button(master=root, text="Mesh (%s%s)" % (u"\u2318","M"), command=_mesh)
button.pack(side=tk.BOTTOM)

root.mainloop()