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

debug: top.h tb_top.cpp cpu.cpp cpu.h memory.cpp memory.h router.cpp router.h ID_Extension.h ID_Extension.cpp
	$(CXX) $(CXXFLAGS) -g ID_Extension.h ID_Extension.cpp router.cpp router.h memory.cpp memory.h cpu.cpp cpu.h top.h tb_top.cpp -o debug.o $(LDLIBS)

top: top.h tb_top.cpp cpu.o memory.o router.o ID_Extension.o
	$(CXX) $(CXXFLAGS) top.h tb_top.cpp router.o cpu.o memory.o ID_Extension.o -o top.o $(LDLIBS)
	./top.o

cpu.o: cpu.cpp cpu.h router.o
	$(CXX) $(CXXFLAGS) -c cpu.h cpu.cpp $(LDLIBS)

memory.o: memory.cpp memory.h  router.o
	$(CXX) $(CXXFLAGS) -c memory.h  memory.cpp $(LDLIBS)
 
router.o: router.cpp router.h ID_Extension.o 
	$(CXX) $(CXXFLAGS) -c router.h router.cpp $(LDLIBS)

ID_Extension.o: ID_Extension.h ID_Extension.cpp
	$(CXX) $(CXXFLAGS) -c ID_Extension.h ID_Extension.cpp $(LDLIBS)

example:
	$(CXX) $(CXXFLAGS) non_blocking.cpp -o non_blocking.o $(LDLIBS)
	./non_blocking.o

clean:
	rm -rf $(OUT_FILES)
