processor: parse.o processor.o
	clang++ -std=c+17 -g -fstandalone-debug  parse.o processor.o -o processor

processor.o: processor.cpp
	clang++ -std=c++17 -g -fstandalone-debug -c processor.cpp -o processor.o

parse: parse.o
	clang++ -std=c++17 -o -g -fstandalone-debug parse parse.o  # Link parse.o to create executable

parse.o: parse.cpp
	clang++ -std=c++17 -g -fstandalone-debug -c parse.cpp -o parse.o  # Compile without linking

clean:
	rm -f parse *.o processor # Clean build files
