#!/usr/bin/python
import sys
import matplotlib as plot

# Main function:
if __name__ == "__main__":
	if len(sys.argv) > 1:
		meshOut = open(sys.argv[1])
	else:
		print "Warning: using default file FirstMesh.out."
		meshOut = open("FirstMesh.out")