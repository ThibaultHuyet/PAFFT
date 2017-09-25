CC = g++
OBJ = pafft.cpp
LIBS = -lrt -lm -lasound -lpthread -lportaudio -lfftw3f -lm

main: $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o main