# ITCR - Diego Herrera
ARCH=$(shell getconf LONG_BIT)
SYSCDIR=/usr/local/systemc-2.3.2
LDLIBS_32= -L$(SYSCDIR)/lib-linux -lsystemc -lm
LDLIBS_64= -L$(SYSCDIR)/lib-linux64 -lsystemc -lm
LDLIBS=$(LDLIBS_$(ARCH))
CXXFLAGS=-Wno-deprecated -I$(SYSCDIR)/include
CXX=g++

OUT_FILES=*.so *.out *.vcd *.h.gch

all: cpu_test

cpu_test: cpu
	$(CXX) $(CXXFLAGS) tb_top.cpp cpu -o top.out $(LDLIBS) 
	./top.out
	gtkwave -a top_config.gtkw ./top.vcd &

cpu: cpu.cpp cpu.h ID_Extension.h
	$(CXX) $(CXXFLAGS) -c cpu.h ID_Extension.h cpu.cpp $(LDLIBS)

example2: cputest.cpp
	$(CXX) $(CXXFLAGS) -c cputest.cpp $(LDLIBS)

example:
	$(CXX) $(CXXFLAGS) non_blocking.cpp -o non_blocking.out $(LDLIBS)

clean:
	rm -rf $(OUT_FILES)