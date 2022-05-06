CXX = g++
INC_DIR = ../packages/spdlog/include
CXXFLAGS = -std=c++17 -Wall -I$(INC_DIR)

OBJECTS = mContainers.o mLog.o

mContainers.out: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJECTS): m*.h

clean:
	rm -f $(OBJECTS) *.o
