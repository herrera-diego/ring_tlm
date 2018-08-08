# ITCR - Diego Herrera

TEST_FILE = non_blocking
BIN_FILE = top

SRC_DIR = ./src
OBJ_DIR = ./obj
INC_DIR = ./include
TEST_DIR = ./example
BIN_DIR = ./bin
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

ARCH=$(shell getconf LONG_BIT)
SYSCDIR=/usr/local/systemc-2.3.2
LDLIBS_32= -L$(SYSCDIR)/lib-linux -lsystemc -lm
LDLIBS_64= -L$(SYSCDIR)/lib-linux64 -lsystemc -lm
LDLIBS=$(LDLIBS_$(ARCH))

CXXFLAGS = -Wno-deprecated -I$(SYSCDIR)/include -I$(INC_DIR)
CXX=g++

$(BIN_DIR)/$(BIN_FILE).o: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDLIBS)

example:
	$(CXX) $(CXXFLAGS) $(TEST_DIR)/$(TEST_FILE).cpp -o $(BIN_DIR)/$(TEST_FILE).o $(LDLIBS)
	$(BIN_DIR)/$(TEST_FILE).o

clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(BIN_DIR)/*.o
