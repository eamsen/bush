# Copyright 2012 Eugen Sawin <sawine@me73.com>
SRCDIR:=src
TSTDIR:=src/test
BINDIR:=bin
OBJDIR:=bin/obj
GFLAGSDIR:=libs/gflags-2.0/.libs
GTESTLIBS:=-lgtest -gtest_main
CXX:=g++ -std=c++0x -Ilibs/gflags-2.0/src
CFLAGS:=-Wall -O3 -g
LIBS:=$(GFLAGSDIR)/libgflags.a -lpthread -lrt
TSTFLAGS:=
TSTLIBS:=$(GTESTLIBS) $(LIBS)
BINS:=bush

TSTBINS:=$(notdir $(basename $(wildcard $(TSTDIR)/*.cc)))
TSTOBJS:=$(addsuffix .o, $(notdir $(basename $(wildcard $(TSTDIR)/*.cc))))
OBJS:=$(notdir $(basename $(wildcard $(SRCDIR)/*.cc)))
OBJS:=$(addsuffix .o, $(filter-out $(BINS), $(OBJS)))
OBJS:=$(addprefix $(OBJDIR)/, $(OBJS))
BINS:=$(addprefix $(BINDIR)/, $(BINS))
TSTBINS:=$(addprefix $(BINDIR)/, $(TSTBINS))

compile: makedirs $(BINS)
	@echo copying bush as vote
	@cp $(BINDIR)/bush $(BINDIR)/vote
	@echo compiled all

profile: CFLAGS=-Wall -O3 -DPROFILE
profile: LIBS+=-lprofiler
profile: clean compile

opt: CFLAGS=-Ofast -flto -mtune=native -DNDEBUG
opt: clean compile

ARGS:=-verbose=false -brief=false
LOG:=perf-results.txt
perftest: opt
	@mkdir -p log; rm -f log/$(LOG);
	@echo "test parameters: LOG=$(LOG) ARGS=$(ARGS)";
	@for i in examples/*.vote;\
		do echo "testing $$i";\
		./bin/ash $(ARGS) $$i >> log/$(LOG);\
		echo " " >> log/$(LOG);\
	done
	@echo "tested all (results in log/$(LOG))";

depend: gflags

makedirs:
	@mkdir -p bin/obj

gflags:
	@cd libs/gflags-2.0/; ./configure; make;
	@echo compiled gflags

test: makedirs $(TSTBINS)
	@for t in $(TSTBINS); do ./$$t; done
	@echo completed tests

checkstyle:
	@python libs/cpplint/cpplint.py --filter=-readability/streams\
		$(SRCDIR)/*.h $(SRCDIR)/*.cc

clean:
	@rm -f $(OBJDIR)/*.o
	@rm -f $(BINS)
	@rm -f $(TSTBINS)
	@echo cleaned

.PRECIOUS: $(OBJS) $(TSTOBJS)
.PHONY: compile profile opt perftest depend makedirs gflags test\
	checkstyle clean

$(BINDIR)/%: $(OBJS) $(SRCDIR)/%.cc
	@$(CXX) $(CFLAGS) -o $(OBJDIR)/$(@F).o -c $(SRCDIR)/$(@F).cc
	@$(CXX) $(CFLAGS) -o $(BINDIR)/$(@F) $(OBJDIR)/$(@F).o $(OBJS) $(LIBS)
	@echo compiled $(BINDIR)/$(@F)

$(BINDIR)/%-test: $(OBJDIR)/%-test.o $(OBJS)
	@$(CXX) $(TSTFLAGS) -o $(BINDIR)/$(@F) $(OBJS) $< $(TSTLIBS)
	@echo compiled $(BINDIR)/$(@F)

$(OBJDIR)/%-test.o: $(TSTDIR)/%-test.cc
	@$(CXX) $(TSTFLAGS) -o $(OBJDIR)/$(@F) -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cc $(SRCDIR)/%.h
	@$(CXX) $(CFLAGS) -o $(OBJDIR)/$(@F) -c $<
