# possible compiler

# arm-v4t-linux-uclibcgnueabi-g++
# arm-linux-gnueabi-g++
# arm-linux-gnueabihf-g++
# g++

CPP := arm-linux-gnueabi-g++

all: clock

clock: clock.cpp fbdevice.o
	$(CPP) -Wall -O3 -static clock.cpp -o clock fbdevice.o -std=c++11 -Wno-unused-variable

fbdevice.o: fbdevice.cpp
	$(CPP) -Wall -O3 -c fbdevice.cpp -o fbdevice.o -std=c++11
	
clean:
	rm -rf fbdevice.o clock

