CXX = g++
CFLAGS = -Wall -Wextra -pthread
CXXFLAGS = -std=gnu++1z
DEFINES = -DANTILATENCY_PACKAGE_DIR=\"/opt/antilatency/\"
INCLUDE = -I../../include
LDFLAGS = -ldl

tracking: Src/Main.cpp
        $(CXX) $(CFLAGS) $(CXXFLAGS) $(DEFINES) $(INCLUDE) -o $@ $^ $(LDFLAGS)

all: tracking

.PHONY: clean

clean:
        rm -f main.o tracking
