ifeq ($(shell uname),Darwin)
	LDFLAGS += -framework OpenCL
else
	LDFLAGS += -lOpenCL -lm
endif

CXX=g++
CXXFLAGS=-std=c++11 -O2

all: clinfo

clinfo: clinfo.cc
	$(CXX) $^ $(CXXFLAGS) -o $@ $(LDFLAGS)

clean:
	rm clinfo