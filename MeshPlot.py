#!/usr/bin/python
import os.path
import sys
import numpy as np
import matplotlib.pyplot as plt

class node:
	id = 0
	x = 0
	y = 0
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
		for nd in nodes:
			print nd.id, nd.x, nd.y, nd.adj
			for adjnode in nd.adj:
				plt.plot([nd.x, nodes[adjnode-1].x],[nd.y, nodes[adjnode-1].y], 'k')
		plt.xlim([-1,3])
		plt.ylim([-1,3])
		plt.show()
	else:
		print "ERROR IN main(): unable to open mesh output file!"