all: procesos swap

procesos: procesos.c mmu.o pagefault.o
	gcc -o procesos procesos.c mmu.o pagefault.o

pagefault.o: pagefault.c mmu.h
	gcc -c pagefault.c


swap:
	./createswap
