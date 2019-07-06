cc := gcc
target := serial_com.so.1
test := serial_test
all: share_lib

share_lib :
	$(CC) -c *.c
	$(cc) -c serial_com.c -shared -fPIC -L./ -o $(target)
	$(cc) -o $(test) serial_test.o *.so*
