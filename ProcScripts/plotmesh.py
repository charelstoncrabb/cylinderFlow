
import os.path
import sys
import numpy as np
import matplotlib.pyplot as plt

helpDict = {
	'h': 'MeshPlot.py:\nPlots the mesh defined in the output file.\nsyntax: \"./MeshPlot.py <output file>\"'}

class node:
	def __init__(self,id = -1,x = -1,y = -1,isbound = 0):
		self.id = id
		self.x = x
		self.y = y
		self.isbound = isbound
		self.adj = []

def findClosest(nodelist,event):
	closestInd = -1
	if len(nodelist) > 0:
		minDist = 1e99
		for i in range(0,len(nodelist)):
			if np.argmin([minDist,np.abs(event.xdata-nodelist[i].x)+np.abs(event.ydata-nodelist[i].y)]) == 1:
				minDist = np.abs(event.xdata-nodelist[i].x)+np.abs(event.ydata-nodelist[i].y)
				closestInd = i
	return closestInd

def writeListToDatFile(filehandle,nodelist):
	for node in nodelist:
		filehandle.write(str(node.id) + ' ' + str(node.x) + ' ' + str(node.y) + '\n')

def openOutfile():
	if len(sys.argv) > 1:
		if sys.argv[1][0] == '-':
			if sys.argv[1][1] in helpDict:
				print '\n' + helpDict[sys.argv[1][1]] + '\n'
			return 0
		else:
			if os.path.isfile(sys.argv[1]):
				return open(sys.argv[1])
			else:
				print "ERROR in openOutfile(): file ", sys.argv[1], " not found!"
				return 0
	else:
		if os.path.isfile("FirstMesh.out"):
			print "Warning: using default file FirstMesh.out."
			return open("FirstMesh.out")
		else:
			print "ERROR IN openOutfile(): file FirstMesh.out not found!"
			return 0

def findIndByID(nodes,ID):
	for i in range(0,len(nodes)):
		if nodes[i].id == ID:
			return i

def plotmesh(filename,p):
	p.cla()
	meshOut = open(filename)
	if meshOut:
		nodes = []
		header = meshOut.readline()
		header = meshOut.readline()
		line = meshOut.readline()
		while len(line) > 1:
			nodedata = line.split(' ')
			n = node(int(nodedata[0]),float(nodedata[2]),float(nodedata[3]),bool(int(nodedata[1])))
			for i in range(4,len(nodedata)-1):
				n.adj.append(int(nodedata[i]))
			nodes.append(n)
			line = meshOut.readline()
		minX = 99999
		maxX = -1
		minY = 99999
		maxY = -1
		for nd in nodes:
			#print nd.id, nd.x, nd.y, nd.adj
			if nd.x < minX:
				minX = nd.x
			if nd.x > maxX:
				maxX = nd.x
			if nd.y < minY:
				minY = nd.y
			if nd.y > maxY:
				maxY = nd.y
			if nd.isbound:
				p.plot(nd.x,nd.y,'gs')
			else:
				p.plot(nd.x,nd.y,'bs')
			for adjnode in nd.adj:
				p.plot([nd.x, nodes[findIndByID(nodes,adjnode)].x],[nd.y, nodes[findIndByID(nodes,adjnode)].y], 'k')
		winrng = [maxX-minX,maxY-minY]
		p.set_xlim([minX-0.05*winrng[0],maxX+0.05*winrng[0]])
		p.set_ylim([minY-0.05*winrng[1],maxY+0.05*winrng[1]])
		p.set_title(str(len(nodes)) + ' nodes')
	return nodes

def scatter(nodelist,p):
	p.cla()
	for node in nodelist:
		p.plot(node.x,node.y,'bs')
	p.set_xlim([0,1])
	p.set_ylim([0,1])