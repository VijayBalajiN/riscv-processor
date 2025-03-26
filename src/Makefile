# Define the build directory
BUILD_DIR = build

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Main executable
$(BUILD_DIR)/nonforwarding: $(BUILD_DIR)/parse.o $(BUILD_DIR)/print_csv.o $(BUILD_DIR)/processor.o $(BUILD_DIR)/nonforwarding.o
	clang++ -std=c++17 -g -fstandalone-debug $^ -o $@

# Object files
$(BUILD_DIR)/nonforwarding.o: nonforwarding.cpp processor.h | $(BUILD_DIR)
	clang++ -std=c++17 -g -fstandalone-debug -c $< -o $@

$(BUILD_DIR)/processor.o: processor.cpp processor.h parse.h print_csv.h | $(BUILD_DIR)
	clang++ -std=c++17 -g -fstandalone-debug -c $< -o $@

$(BUILD_DIR)/print_csv.o: print_csv.cpp print_csv.h | $(BUILD_DIR)
	clang++ -std=c++17 -g -fstandalone-debug -c $< -o $@

$(BUILD_DIR)/parse.o: parse.cpp parse.h | $(BUILD_DIR)
	clang++ -std=c++17 -g -fstandalone-debug -c $< -o $@

# Phony targets
.PHONY: run clean

run: $(BUILD_DIR)/nonforwarding
	./$<

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/nonforwarding