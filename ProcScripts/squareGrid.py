#!/usr/bin/python
import os.path
import sys

dat = open(sys.argv[1],"w+")
dat.write("ID X Y\n")
k = 1
for i in range(0,int(sys.argv[2])):
	for j in range(0,int(sys.argv[3])):
		dat.write(str(k) + " " + str(i) + " " + str(j) + "\n")
		k = k + 1