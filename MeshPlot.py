#!/usr/bin/python
import os.path
import sys
import numpy as np
import matplotlib.pyplot as plt

class node:
	id = -1
	x = -1
	y = -1
	adj = []

def openOutfile():
	if len(sys.argv) > 1:
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
		for line in meshOut:
			n = node()
			n.adj = []
			nodedata = line.split(' ')
			n.id = int(nodedata[0])
			n.x = float(nodedata[1])
			n.y = float(nodedata[2])
			for i in range(3,len(nodedata)-1):
				n.adj.append(int(nodedata[i]))
			nodes.append(n)
		maxX = -1
		maxY = -1
		for nd in nodes:
			print nd.id, nd.x, nd.y, nd.adj
			if nd.x > maxX:
				maxX = nd.x
			if nd.y > maxY:
				maxY = nd.y
			plt.plot(nd.x,nd.y,'bs')
			for adjnode in nd.adj:
				plt.plot([nd.x, nodes[findIndByID(nodes,adjnode)].x],[nd.y, nodes[findIndByID(nodes,adjnode)].y], 'k')
		plt.xlim([-1,maxX+1])
		plt.ylim([-1,maxY+1])
		plt.show()
	else:
		print "ERROR IN main(): unable to open mesh output file!"




