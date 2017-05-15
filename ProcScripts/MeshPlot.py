#!/usr/bin/python
import os.path
import sys
import numpy as np
import matplotlib.pyplot as plt

helpDict = {}
helpDict['h'] = 'MeshPlot.py:\nPlots the mesh defined in the output file.\nsyntax: \"./MeshPlot.py <output file>\"'

class node:
	id = -1
	x = -1
	y = -1
	isbound = 0
	adj = []

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

# Main function:
if __name__ == "__main__":
	meshOut = openOutfile()
	if meshOut:
		nodes = []
		header = meshOut.readline()
		header = meshOut.readline()
		line = meshOut.readline()
		while len(line) > 1:
			n = node()
			n.adj = []
			nodedata = line.split(' ')
			n.id = int(nodedata[0])
			n.isbound = bool(int(nodedata[1]))
			n.x = float(nodedata[2])
			n.y = float(nodedata[3])
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
				plt.plot(nd.x,nd.y,'gs')
			else:
				plt.plot(nd.x,nd.y,'bs')
			for adjnode in nd.adj:
				plt.plot([nd.x, nodes[findIndByID(nodes,adjnode)].x],[nd.y, nodes[findIndByID(nodes,adjnode)].y], 'k')
		plt.xlim([minX-1,maxX+1])
		plt.ylim([minY-1,maxY+1])
		plt.title(sys.argv[1] + ': ' + str(len(nodes)) + ' nodes')
		plt.show()
