processor: parse.o processor.o
	clang++ -std=c+17 -g -fstandalone-debug  build/parse.o build/processor.o -o build/processor

processor.o: processor.cpp
	clang++ -std=c++17 -g -fstandalone-debug -c processor.cpp -o processor.o
	mv processor.o build/processor.o

parse.o: parse.cpp
	clang++ -std=c++17 -g -fstandalone-debug -c parse.cpp -o parse.o  # Compile without linking
	mv parse.o build/parse.o

clean:
	rm -rf parse *.o processor build/*
