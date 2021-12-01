p0: p0.cpp defs.h
	g++ -g p0.cpp -lpthread -o p0

p1: p1.cpp defs.h
	g++ -g p1.cpp -lpthread -o p1

clean:
	rm *.o p0 p1 *.txt