default: mymalloc.o test.o
	gcc -o test mymalloc.o test.o

test.o: test.c
	gcc -c test.c

mymalloc.o: mymalloc.c
	gcc -c mymalloc.c
