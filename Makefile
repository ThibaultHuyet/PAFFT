CC = g++
SRC = $(wildcard src/*.cpp)
LIBS = -lrt -lm -lasound -lpthread -lportaudio -lfftw3f -lm -lmosquitto

main: $(SRC)
	$(CC) $(SRC) $(LIBS) -o main