.PHONY: misc
misc: bin/PCGEN bin/CPCGEN bin/uMETA bin/dMETA bin/uGETVOL bin/dGETVOL bin/uKCORE

bin/PCGEN: src/main/pc_gen_main.cpp build/generators/graphgen.o build/common/unionfind.o build/common/random.o build/common/io.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

bin/CPCGEN: src/main/cpc_gen_main.cpp build/generators/graphgen.o build/common/unionfind.o build/common/random.o build/common/io.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

bin/uMETA: src/main/get_meta_undirected.cpp build/common/io.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

bin/dMETA: src/main/get_meta_directed.cpp build/common/io.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

bin/dGETVOL: src/main/directed_getvolume_main.cpp
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

uGETVOL_OBJS = \
	build/decision/localvc/local-skiptrivial_edgesampling_undirected.o \
	build/balancedvc/pickbest.o \
	build/unbalancedvc/edgesampling_pickbest.o \
	build/localec/degreecounting.o \
	build/common/resettable_graph.o \
	build/common/graphutil.o \
	build/common/graphutil_resettable.o \
	build/common/io.o \
	build/common/random.o \
	build/common/sparsify.o \
	build/common/unionfind.o \
	build/maxflow/xy/basic_ford_fulkerson.o

bin/uGETVOL: src/main/undirected_getvolume_main.cpp  $(uGETVOL_OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

bin/uKCORE: src/main/undirected_kcore_main.cpp build/common/io.o build/util/kcore.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
