CC = g++
SRC = $(wildcard src/*.cpp)
SRC += $(wildcard src/*.hpp)
LIBS = -lrt -lm -lasound -lpthread -lportaudio -lfftw3f -lm -lpaho-mqtt3c

main: $(SRC)
	$(CC) -std=c++14 $(SRC) $(LIBS) -o main