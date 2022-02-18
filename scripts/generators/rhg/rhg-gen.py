import networkit as nk

def g(n, d, gamma):
	return nk.generators.HyperbolicGenerator(n, d, gamma).generate()

def save(g, filename):
	nk.graphio.EdgeListWriter(" ", 0).write(g, filename)

def make_and_save(n=2**10, d=2**5, gamma=3, redundancy=1, reduncancy_start=0, directory="."):
	for i in range(redundancy):
		filename = str(n) + "-" + str(d) + "-" + str(gamma)
		if redundancy != 1 or reduncancy_start != 0:
			filename = filename + "+" + str(reduncancy_start + i)
		filename = directory + "/" + filename + ".txt"
		save(g(n,d,gamma), filename)

import sys

n = 1000
d = 32
gamma = 3
redundancy = 1
redundancy_start = 0
directory="."

if len(sys.argv) > 1:
	n = int(sys.argv[1])
if len(sys.argv) > 2:
	d = int(sys.argv[2])
if len(sys.argv) > 3:
	gamma = float(sys.argv[3])
if len(sys.argv) > 4:
	redundancy = int(sys.argv[4])
if len(sys.argv) > 5:
	redundancy_start = int(sys.argv[5])
if len(sys.argv) > 6:
	directory = sys.argv[6]

make_and_save(n, d, gamma, redundancy, redundancy_start, directory)
