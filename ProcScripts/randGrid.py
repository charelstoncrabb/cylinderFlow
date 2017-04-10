#!/usr/bin/python
import os.path
import sys
import random

dat = open(sys.argv[1],"w+")
dat.write("ID X Y\n")
k = 1
for i in range(0,int(sys.argv[2])):
	for j in range(0,int(sys.argv[3])):
		dat.write(str(k) + " " + str(random.uniform(0,20)) + " " + str(random.uniform(0,20)) + "\n")
		k = k + 1