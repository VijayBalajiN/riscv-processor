parse: parse.o
	clang++ -std=c++11 -o parse parse.o  # Link parse.o to create executable

parse.o: parse.cpp
	clang++ -std=c++11 -c parse.cpp -o parse.o  # Compile without linking

clean:
	rm -f parse parse.o  # Clean build files
