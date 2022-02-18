.PHONY: hrg
hrg: bin/ouHRG bin/duHRG bin/odHRG bin/ddHRG

HRG-COMMON-OBJS = \
	build/common/graphutil.o \
	build/common/io.o \
	build/common/random.o \
	build/common/sparsify.o \
	build/common/unionfind.o \
	build/hrg/hrg-common.o \
	build/hrg/min-vc-split.o

HRGu-OBJS = build/optimisation/pickfirst_undirected.o build/decision/hrg/hrg-decision_undirected.o
HRGd-OBJS = build/optimisation/single.o build/decision/hrg/hrg-decision_directed.o

bin/ouHRG: src/main/undirected_main.cpp $(HRGu-OBJS) $(HRG-COMMON-OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

bin/duHRG: src/main/undirected_dec_main.cpp $(HRGu-OBJS) $(HRG-COMMON-OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

bin/odHRG: src/main/directed_main.cpp $(HRGd-OBJS) $(HRG-COMMON-OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^

bin/ddHRG: src/main/directed_dec_main.cpp $(HRGd-OBJS) $(HRG-COMMON-OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -o $@ $^