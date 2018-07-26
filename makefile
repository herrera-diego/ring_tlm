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

top: top.h tb_top.cpp cpu.o memory.o router.o
	$(CXX) $(CXXFLAGS) top.h tb_top.cpp router.o cpu.o memory.o -o top.o $(LDLIBS) 
	./top.o
	#gtkwave -a top_config.gtkw ./top.vcd &

cpu.o: cpu.cpp cpu.h router.o
	$(CXX) $(CXXFLAGS) -c cpu.h cpu.cpp $(LDLIBS)

memory.o: memory.cpp memory.h  router.o
	$(CXX) $(CXXFLAGS) -c memory.h  memory.cpp $(LDLIBS)

router.o: router.cpp router.h ID_Extension.h
	$(CXX) $(CXXFLAGS) -c router.h ID_Extension.h router.cpp $(LDLIBS)

example:
	$(CXX) $(CXXFLAGS) non_blocking.cpp -o non_blocking.o $(LDLIBS)

clean:
	rm -rf $(OUT_FILES)