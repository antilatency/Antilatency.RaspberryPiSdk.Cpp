TARGET = ipnetwork
CXX = g++
CFLAGS = -Wall -Wextra -pthread
CXXFLAGS = -std=gnu++1z
INCLUDE = -I../../include
DEFINES = -DANTILATENCY_PACKAGE_DIR=\"/opt/antilatency/\"
LDFLAGS = -ldl

$(TARGET): Src/Main.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(DEFINES) $(INCLUDE) -o $@ $^ $(LDFLAGS)

all: $(TARGET)

.PHONY: clean

clean:
	rm -f $(TARGET)

