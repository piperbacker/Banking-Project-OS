p0: p0.cpp defs.h
	g++ -g p0.cpp -lpthread -o p0

clean:
	rm *.o p0 *.txt