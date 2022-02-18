CC = g++
CPPFLAGS = -std=c++17 -Wall -Werror -O3 -I include/
TESTFLAGS = $(CPPFLAGS) -I /usr/include -I /usr/src/gtest -pthread -DMAKE_EVERYTHING_PUBLIC

LOCAL_COMMON_OBJS = \
	build/common/resettable_graph.o \
	build/common/graphutil.o \
	build/common/graphutil_resettable.o \
	build/common/io.o \
	build/common/random.o \
	build/common/sparsify.o \
	build/common/unionfind.o \
	build/maxflow/xy/basic_ford_fulkerson.o \
	build/balancedvc/pickbest.o \
	build/unbalancedvc/edgesampling_pickbest.o

# decision directed
bin/ddLOCAL1: $(LOCAL_COMMON_OBJS) build/localec/optimised.o \
		src/main/directed_dec_main.cpp build/decision/localvc/local-skiptrivial_edgesampling_directed.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/ddLOCAL1+: $(LOCAL_COMMON_OBJS) build/localec/degreecounting.o \
		src/main/directed_dec_main.cpp build/decision/localvc/local-skiptrivial_edgesampling_directed.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/ddLOCAL2+: $(LOCAL_COMMON_OBJS) build/localec/marking-degreecounting.o \
		src/main/directed_dec_main.cpp build/decision/localvc/local-skiptrivial_edgesampling_directed.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

# optimisation directed
bin/odLOCAL1: $(LOCAL_COMMON_OBJS) build/localec/optimised.o \
		src/main/directed_main.cpp build/optimisation/pickfirst.o build/decision/localvc/local-skiptrivial_edgesampling_undirected.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/odLOCAL1+: $(LOCAL_COMMON_OBJS) build/localec/degreecounting.o \
		src/main/directed_main.cpp build/optimisation/pickfirst.o build/decision/localvc/local-skiptrivial_edgesampling_undirected.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/odLOCAL2+: $(LOCAL_COMMON_OBJS) build/localec/marking-degreecounting.o \
		src/main/directed_main.cpp build/optimisation/pickfirst.o build/decision/localvc/local-skiptrivial_edgesampling_undirected.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

# decision undirected
bin/duLOCAL1: $(LOCAL_COMMON_OBJS) build/localec/optimised.o \
		src/main/undirected_dec_main.cpp build/decision/localvc/local-skiptrivial_edgesampling_directed.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/duLOCAL1+: $(LOCAL_COMMON_OBJS) build/localec/degreecounting.o \
		src/main/undirected_dec_main.cpp build/decision/localvc/local-skiptrivial_edgesampling_directed.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/duLOCAL2+: $(LOCAL_COMMON_OBJS) build/localec/marking-degreecounting.o \
		src/main/undirected_dec_main.cpp build/decision/localvc/local-skiptrivial_edgesampling_directed.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

# optimisation undirected
bin/ouLOCAL1: $(LOCAL_COMMON_OBJS) build/localec/optimised.o \
		src/main/undirected_main.cpp build/optimisation/pickfirst_undirected.o build/decision/localvc/local-skiptrivial_edgesampling_undirected.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/ouLOCAL1+: $(LOCAL_COMMON_OBJS) build/localec/degreecounting.o \
		src/main/undirected_main.cpp build/optimisation/pickfirst_undirected.o build/decision/localvc/local-skiptrivial_edgesampling_undirected.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/ouLOCAL2+: $(LOCAL_COMMON_OBJS) build/localec/marking-degreecounting.o \
		src/main/undirected_main.cpp build/optimisation/pickfirst_undirected.o build/decision/localvc/local-skiptrivial_edgesampling_undirected.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

LOCALVC_BINS = \
bin/ddLOCAL1 bin/ddLOCAL1+ bin/ddLOCAL2+ \
bin/odLOCAL1 bin/odLOCAL1+ bin/odLOCAL2+ \
bin/duLOCAL1 bin/duLOCAL1+ bin/duLOCAL2+ \
bin/ouLOCAL1 bin/ouLOCAL1+ bin/ouLOCAL2+

# decision directed
bin/ddLOCAL1debug: $(LOCAL_COMMON_OBJS) build/localec/optimised_diagnostics.o \
		src/main/directed_dec_debuglocalec_main.cpp build/util/diagnostics.o build/optimisation/pickfirst.o build/decision/localvc/local-skiptrivial_edgesampling_directed.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/ddLOCAL1+debug: $(LOCAL_COMMON_OBJS) build/localec/degreecounting_diagnostics.o \
		src/main/directed_dec_debuglocalec_main.cpp build/util/diagnostics.o build/optimisation/pickfirst.o build/decision/localvc/local-skiptrivial_edgesampling_directed.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/ddLOCAL2+debug: $(LOCAL_COMMON_OBJS) build/localec/marking-degreecounting_diagnostics.o \
		src/main/directed_dec_debuglocalec_main.cpp build/util/diagnostics.o build/optimisation/pickfirst.o build/decision/localvc/local-skiptrivial_edgesampling_directed.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

# decision undirected
bin/duLOCAL1debug: $(LOCAL_COMMON_OBJS) build/localec/optimised_diagnostics.o \
		src/main/undirected_dec_debuglocalec_main.cpp build/util/diagnostics.o build/optimisation/pickfirst.o build/decision/localvc/local-skiptrivial_edgesampling_undirected.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/duLOCAL1+debug: $(LOCAL_COMMON_OBJS) build/localec/degreecounting_diagnostics.o \
		src/main/undirected_dec_debuglocalec_main.cpp build/util/diagnostics.o build/optimisation/pickfirst.o build/decision/localvc/local-skiptrivial_edgesampling_undirected.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^
bin/duLOCAL2+debug: $(LOCAL_COMMON_OBJS) build/localec/marking-degreecounting_diagnostics.o \
		src/main/undirected_dec_debuglocalec_main.cpp build/util/diagnostics.o build/optimisation/pickfirst.o build/decision/localvc/local-skiptrivial_edgesampling_undirected.o
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

LOCALVC_DEBUG_BINS = \
bin/ddLOCAL1debug bin/ddLOCAL1+debug bin/ddLOCAL2+debug \
bin/duLOCAL1debug bin/duLOCAL1+debug bin/duLOCAL2+debug

.PHONY: localvc
localvc: $(LOCALVC_BINS)

.PHONY: localvcextra
localvcextra: $(LOCALVC_DEBUG_BINS)
