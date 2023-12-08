CXX = g++
CXXFLAGS = -std=c++11 -Wall
SRC = vanditshah.cpp
TARGET = gatorlibraryfile_vanditshah

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
