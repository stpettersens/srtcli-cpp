TARGET = bin/srtcli.exe
SOURCE = src/srtcli.cpp src/clock.cpp src/subtitle.cpp
LIBS = -lboost_chrono -lboost_thread -lboost_system 
INCLUDE_PATH = C:/Dev/MinGW/include 
LIB_PATH = C:/Dev/MinGW/lib

make:
	g++ -w -std=c++11 -I$(INCLUDE_PATH) $(SOURCE) -o $(TARGET) -L$(LIB_PATH) $(LIBS) 
	strip $(TARGET)
