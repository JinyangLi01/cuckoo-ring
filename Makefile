CFLAGS=-g  -Wall --std=c++11 -o2

all:test tspeed

test:main.o
	g++ ${CFLAGS} -o $@ $+

tspeed:test_speed.o
	g++ ${CFLAGS} -o $@ $+

.cpp.o:
	g++ -c ${CFLAGS} -o $@ $+

clean:
	rm -f *.o test tspeed out.txt hash/*.o
