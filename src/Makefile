nonforwarding: parse.o processor.o nonforwarding.o
	clang++ -std=c++17 -g -fstandalone-debug  build/parse.o build/processor.o build/nonforwarding.o -o build/nonforwarding

nonforwarding.o: nonforwarding.cpp build/processor.o
	clang++ -std=c++17 -g -fstandalone-debug  -c nonforwarding.cpp -o nonforwarding.o
	mv nonforwarding.o build/nonforwarding.o

processor.o: processor.cpp
	clang++ -std=c++17 -g -fstandalone-debug -c processor.cpp -o processor.o
	mv processor.o build/processor.o

parse.o: parse.cpp
	clang++ -std=c++17 -g -fstandalone-debug -c parse.cpp -o parse.o  # Compile without linking
	mv parse.o build/parse.o

clean:
	rm -rf parse *.o nonforwarding forwarding build/*
