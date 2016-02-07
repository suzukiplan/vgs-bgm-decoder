CC=/usr/bin/gcc

all:\
	clean\
	test-general\
	test-reload\
	test-mt

clean:
	-@rm -f ./test-general
	-@rm -f ./test-reload
	-@rm -f ./test-mt

test-general:
	@echo ===== general test =====
	$(CC) -O2 -o test-general -I../src test-general.c ../src/vgs2tone.c ../src/vgsdec.c -lpthread
	valgrind --tool=memcheck ./test-general test.vgs

test-reload:
	@echo ===== reload test =====
	$(CC) -O2 -o test-reload -I../src test-reload.c ../src/vgs2tone.c ../src/vgsdec.c -lpthread
	valgrind --tool=memcheck ./test-reload test.vgs

test-mt:
	@echo ===== multiple thread test =====
	$(CC) -O2 -o test-mt -I../src test-mt.c ../src/vgs2tone.c ../src/vgsdec.c -lpthread
	valgrind --tool=memcheck ./test-mt test.vgs

