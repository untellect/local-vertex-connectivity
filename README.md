# Readme

The code is a Microsoft Visual Studio 2019 project.

## Data (Planted cuts)

Planted cut data is generated and saved by included code (graphgen.cpp and mains.cpp). There are two generators included. One fast generator does not work if |L| + |S| > k and one slower generator with quadratic space complexity.

## Data (Random Hyperbolic Graphs)

Random hyperbolic graphs are pregenerated using [NetworKIT](https://networkit.github.io/). The files are named a-b-c+d.txt where the graph has 2^a vertices, average degree 2^b and power law factor c. With current configuration they have been stored in directory ./INPUT/RHG/a/, where a is the same parameter The number d distinguishes graphs generated with the same parameters. The generating scripts and actual data are currently not included.

## Data (Real World Networks)

The three real world networks used are from [LiveJournal](https://snap.stanford.edu/data/com-LiveJournal.html), [Epinions.com](https://snap.stanford.edu/data/soc-Epinions1.html) and a web graph of [Berkeley and Stanford](https://snap.stanford.edu/data/web-BerkStan.html), all part of the Stanford Large Network Dataset Collection. The code to generate k-cores is included in mains.cpp.

## Configuration

The configuration "Release (Main)" is currently set up to generate some graphs with planted cuts with vertex connectivity 4, up to 1000 vertices. To run other tests, edit commented out lines in mains.cpp and main.cpp.
