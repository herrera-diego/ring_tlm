# ITCR - Diego Herrera
ARCH=$(shell getconf LONG_BIT)
SYSCDIR=/usr/local/systemc-2.3.2
LDLIBS_32= -L$(SYSCDIR)/lib-linux -lsystemc -lm
LDLIBS_64= -L$(SYSCDIR)/lib-linux64 -lsystemc -lm
LDLIBS=$(LDLIBS_$(ARCH))
CXXFLAGS=-Wno-deprecated -I$(SYSCDIR)/include
CXX=g++

OUT_FILES=*.so *.o *.vcd *.h.gch

all: top

top: top.h tb_top.cpp cpu memory router
	$(CXX) $(CXXFLAGS) top.h tb_top.cpp cpu.o router.o memory.o -o top.o $(LDLIBS) 
	./top.o
	#gtkwave -a top_config.gtkw ./top.vcd &

cpu: cpu.cpp cpu.h router
	$(CXX) $(CXXFLAGS) -c cpu.h ID_Extension.h cpu.cpp $(LDLIBS)

memory: memory.cpp memory.h ID_Extension.h
	$(CXX) $(CXXFLAGS) -c memory.h ID_Extension.h memory.cpp $(LDLIBS)

router: router.cpp router.h ID_Extension.h
	$(CXX) $(CXXFLAGS) -c router.h ID_Extension.h router.cpp $(LDLIBS)

example:
	$(CXX) $(CXXFLAGS) non_blocking.cpp -o non_blocking.o $(LDLIBS)

clean:
	rm -rf $(OUT_FILES)