CC=/usr/bin/gcc

all: decoder decoder_async 

decoder: decoder.c ../src/vgs2tone.c ../src/vgsdec.c ../src/miniz.c
	$(CC) -O2 -o decoder -I../src decoder.c ../src/vgs2tone.c ../src/vgsdec.c ../src/miniz.c -lpthread

decoder_async: decoder_async.c ../src/vgs2tone.c ../src/vgsdec.c ../src/miniz.c
	$(CC) -O2 -o decoder_async -I../src decoder_async.c ../src/vgs2tone.c ../src/vgsdec.c ../src/miniz.c -lpthread

test: all 
	vgs2mml test.mml test.bgm
	vgs2pack test
	@echo ===== TESTING BASIC DECODE =====
	./decoder test.vgs test.wav
	@echo ===== TESTING ASYNC DECODE =====
	./decoder_async test.vgs test2.wav

