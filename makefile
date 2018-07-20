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

cpu: *.cpp *.h
	$(CXX) $(CXXFLAGS) -c *.cpp *.h $(LDLIBS)

clean:
	rm -rf $(OUT_FILES)