CC = g++
SRC = $(wildcard src/*.cpp)
SRC += $(wildcard src/*.hpp)
LIBS = -lrt -lm -lasound -lpthread -lportaudio -lfftw3 -lm -lpaho-mqtt3c -larmadillo -lblas -llapack
INCLUDES = -Isigpack-1.2.4/sigpack

main: $(SRC)
	$(CC) -std=c++14 $(SRC) $(LIBS) $(INCLUDES) -o main