#!/usr/bin/python
import os.path
import sys
import random

helpDict = {}
helpDict['h'] = 'randGrid.py:\nWrites randomly-chosen grid points to data file.\nsyntax: \"./randGrid.py <.dat file> <num X values> <num Y values>\"'

def openOutfile():
	if len(sys.argv) > 1:
		if sys.argv[1][0] == '-':
			if sys.argv[1][1] in helpDict:
				print '\n' + helpDict[sys.argv[1][1]] + '\n'
			return 0
		else:
			return open(sys.argv[1],'w+')

if __name__ == "__main__":
	dat = openOutfile()
	if dat:
		dat.write("ID X Y\n")
		k = 1
		for i in range(0,int(sys.argv[2])):
			for j in range(0,int(sys.argv[3])):
				dat.write(str(k) + " " + str(random.uniform(0,20)) + " " + str(random.uniform(0,20)) + "\n")
				k = k + 1
