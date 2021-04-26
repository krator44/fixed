all: fixed

fixed: main.o
	g++ -O3 -o fixed main.o

main.o: main.cc fixed.h
	g++ -O3 -c main.cc

clean:
	rm -v *.o
	rm -v fixed


