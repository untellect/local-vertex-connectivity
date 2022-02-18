#!/usr/bin/python

from sys import argv
from os import listdir
from os.path import isdir, dirname, abspath 

def read_meta(path):
	f = open(path, "r")
	meta = dict()
	for line in f.readlines()[1:]:
		meta[line.split()[0]] = int(line.split()[1])
	return meta

def read_sample(path):
	f = open(path, "r")
	data = [line.split() for line in f.readlines()[1:]]
	def sample(line):
		sample = dict()
		sample["k"] = int(line[0])
		sample["seconds"] = float(line[1])
		return sample
	return [sample(line) for line in data[1:]]

def read_debug(path):
	f = open(path, "r")
	data = [line.split() for line in f.readlines()[1:]]
	def sample(line, header):
		sample = dict()
		for i in range(min(len(header), len(line))):
			if header[i] not in {"nu", "N"}: # nu and N do not make sense in compact form
				sample[header[i]] = int(line[i])
		return sample
	return [sample(line, data[0]) for line in data[1:]]

def read_graphdir(path):
	data = dict()
	for filename in listdir(path):
		if filename in {"META.txt", "dMETA.txt", "uMETA.txt"}:
			meta = read_meta(path + "/" + filename)
		elif "debug" in filename:
			alg = filename.split(".")[0]
			data[alg] = read_debug(path + "/" + filename)
	return meta, data

def merge_datapoint(data):
	merged_data = dict()
	for alg in data:
		if len(data[alg]) > 0:
			point = dict()
			point["E"]  = sum(sample["E"] for sample in data[alg]) #/ len(data[alg])
			point["V"]  = sum(sample["V"] for sample in data[alg]) #/ len(data[alg])
			point["UV"] = sum(sample["UV"] for sample in data[alg]) #/ len(data[alg])
			point["N"] = len(data[alg])
			merged_data[alg] = point
	return merged_data

def read_paramdir(path):
	meta = dict()
	data = dict()
	for graphdir in listdir(path):
		if isdir(path + "/" + graphdir):
			new_meta, new_data = read_graphdir(path + "/" + graphdir)
			for key in new_meta:
				if key in meta:
					meta[key].append(new_meta[key])
				else:
					meta[key] = [new_meta[key]]
			for alg in new_data:
				if alg in data:
					data[alg] += new_data[alg] # Join the lists
				else:
					data[alg] = new_data[alg]
	# Merge meta
	for key in meta:
		tmp = meta[key][0]
		meta[key] = tmp + sum(i - tmp for i in meta[key]) // len(meta[key]) # Stable average
	return meta, merge_datapoint(data)

# Average results over a set of graphs with same parameters
def read_fully_merged(path):
	return [read_paramdir(path + "/" + paramdir) for paramdir in listdir(path) if isdir(path + "/" + paramdir)]

if len(argv) == 2:
	root = dirname(dirname(abspath(__file__)))
	path = root + "/data/current/" + argv[1]
	#data = read_half_merged(path)
	data = read_fully_merged(path)
	data = [(meta, sample) for meta, sample in data if sample]

	meta_header = { key for meta, samples in data for key in meta }
	meta_header = [key for key in ["n", "m", "dmin", 'rdmin'] if key in meta_header] + [key for key in meta_header if key not in ["n", "m", "dmin", 'rdmin']]

	alg_header = { alg:samples[alg].keys() for meta, samples in data for alg in samples }

	if "L" in meta_header:
		data.sort(key=lambda row : row[0]["L"])
	else:
		if "kappa" in meta_header:
			data.sort(key=lambda row : row[0]["kappa"])
		if "m" in meta_header:
			data.sort(key=lambda row : row[0]["m"])
		if "n" in meta_header:
			data.sort(key=lambda row : row[0]["n"])

	with open(path + "/results-debug.csv", mode='w') as results:
		results.write(",".join(str(x) for x in meta_header))
		for alg in alg_header:
			results.write("," + ",".join(alg + "_" + key for key in alg_header[alg]))
		results.write("\n")
		for meta, samples in data:
			results.write(",".join((str(meta[key]) if key in meta else "") for key in meta_header) + ",")
			results.write(",".join((str(samples[alg][key]) if alg in samples and key in samples[alg] else "") for alg in alg_header for key in alg_header[alg]) + "\n")
