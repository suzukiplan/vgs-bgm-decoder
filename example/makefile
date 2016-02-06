CC=/usr/bin/gcc

all: decoder 

decoder: decoder.c ../src/vgs2tone.c ../src/vgsdec.c
	$(CC) -o decoder -I../src decoder.c ../src/vgs2tone.c ../src/vgsdec.c -lpthread

test: decoder
	./decoder test.bgm test.wav
	open test.wav

