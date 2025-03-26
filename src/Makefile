all: forwarding nonforwarding

forwarding: parse.o processor.o forwarding.o
	clang++ -std=c++17 -g -fstandalone-debug build/parse.o build/print_csv.o build/processor.o build/forwarding.o -o build/forwarding

forwarding.o: forwarding.cpp processor.o
	clang++ -std=c++17 -g -fstandalone-debug  -c forwarding.cpp -o forwarding.o
	mv forwarding.o build/forwarding.o

nonforwarding: parse.o processor.o nonforwarding.o
	clang++ -std=c++17 -g -fstandalone-debug  build/parse.o build/print_csv.o build/processor.o build/nonforwarding.o -o build/nonforwarding

nonforwarding.o: nonforwarding.cpp processor.o
	clang++ -std=c++17 -g -fstandalone-debug  -c nonforwarding.cpp -o nonforwarding.o
	mv nonforwarding.o build/nonforwarding.o

processor.o: processor.cpp print_csv.o
	clang++ -std=c++17 -g -fstandalone-debug -c processor.cpp -o processor.o
	mv processor.o build/processor.o

parse.o: parse.cpp
	clang++ -std=c++17 -g -fstandalone-debug -c parse.cpp -o parse.o  # Compile without linking
	mv parse.o build/parse.o

print_csv.o: print_csv.cpp
	clang++ -std=c++17 -g -fstandalone-debug -c print_csv.cpp -o print_csv.o
	mv print_csv.o build/print_csv.o

run: build/nonforwarding
	build/nonforwarding

clean:
	rm -rf parse *.o nonforwarding forwarding build/*
