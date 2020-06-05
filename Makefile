default: mymalloc.o
	gcc -o main mymalloc.o

mymalloc.o: mymalloc.c
	gcc -c mymalloc.c
