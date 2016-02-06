CC=/usr/bin/gcc

all: decoder 

decoder: decoder.c ../src/vgs2tone.c ../src/vgsdec.c
	$(CC) -O2 -o decoder -I../src decoder.c ../src/vgs2tone.c ../src/vgsdec.c -lpthread

test: decoder
	vgs2mml test.mml test.bgm
	vgs2pack test
	./decoder test.vgs test.wav
	open test.wav

