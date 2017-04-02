#!/usr/bin/python
import sys
import numpy as np
import matplotlib.pyplot as plt

class node:
	id = 0
	x = 0
	y = 0
	adj = []

# Main function:
if __name__ == "__main__":
	if len(sys.argv) > 1:
		meshOut = open(sys.argv[1])
	else:
		print "Warning: using default file FirstMesh.out."
		meshOut = open("FirstMesh.out")
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