CC=/usr/bin/gcc

all: clean test-general 

clean:
	-@rm -f ./test-general

test-general:
	@echo ===== general test =====
	$(CC) -O2 -o test-general -I../src test-general.c ../src/vgs2tone.c ../src/vgsdec.c -lpthread
	valgrind --tool=memcheck ./test-general test.vgs

