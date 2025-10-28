CXX = g++
CXXFLAGS = -Wall -Iinclude

all: ii

# Compila main.cpp y ii.cpp en un ejecutable llamado ii
ii: src/main.cpp src/ii.cpp
	$(CXX) $(CXXFLAGS) -o ii src/main.cpp src/ii.cpp

clean:
	rm -f ii
