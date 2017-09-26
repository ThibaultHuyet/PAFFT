CC = g++
OBJ = pafft.cpp
LIBS = -lrt -lm -lasound -lpthread -lportaudio -lfftw3f -lm -lmosquitto

main: $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o main