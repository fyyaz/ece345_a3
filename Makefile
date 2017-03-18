CFLAGS = -g -Wall -Werror --std=c++11 -o
TARGET = influence
CFILES = influence.cpp
CXX = g++

influence: influence.cpp 
	$(CXX) $(CFLAGS) $(TARGET) $(CFILES) 

clean:
	rm -rf $(TARGET)
