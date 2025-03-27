# Define the build directory
BUILD_DIR = build

all:
	@echo "Building all targets..."
	@$(MAKE) build_f
	@$(MAKE) build_n
	@echo "All targets built successfully."
# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Main executables
$(BUILD_DIR)/forward: $(BUILD_DIR)/parse.o $(BUILD_DIR)/print_csv.o $(BUILD_DIR)/processor.o $(BUILD_DIR)/forwarding.o
	clang++ -std=c++17 -g -fstandalone-debug $^ -o $@

$(BUILD_DIR)/noforward: $(BUILD_DIR)/parse.o $(BUILD_DIR)/print_csv.o $(BUILD_DIR)/processor.o $(BUILD_DIR)/nonforwarding.o
	clang++ -std=c++17 -g -fstandalone-debug $^ -o $@

# Object files
$(BUILD_DIR)/forwarding.o: forwarding.cpp processor.h | $(BUILD_DIR)
	clang++ -std=c++17 -g -fstandalone-debug -c $< -o $@

$(BUILD_DIR)/nonforwarding.o: nonforwarding.cpp processor.h | $(BUILD_DIR)
	clang++ -std=c++17 -g -fstandalone-debug -c $< -o $@

$(BUILD_DIR)/processor.o: processor.cpp processor.h parse.h print_csv.h | $(BUILD_DIR)
	clang++ -std=c++17 -g -fstandalone-debug -c $< -o $@

$(BUILD_DIR)/print_csv.o: print_csv.cpp print_csv.h | $(BUILD_DIR)
	clang++ -std=c++17 -g -fstandalone-debug -c $< -o $@

$(BUILD_DIR)/parse.o: parse.cpp parse.h | $(BUILD_DIR)
	clang++ -std=c++17 -g -fstandalone-debug -c $< -o $@

# Phony targets
.PHONY: run run_f run_n build_f build_n clean

# Build targets
build_f: $(BUILD_DIR)/forward
build_n: $(BUILD_DIR)/noforward

# Run targets 
run_f: $(BUILD_DIR)/forward
	./$(BUILD_DIR)/forward ../inputfiles/testcases/test15.txt 200

run_n: $(BUILD_DIR)/noforward
	./$(BUILD_DIR)/noforward  ../inputfiles/testcases/test15.txt 200


# Original targets for backward compatibility
run: $(BUILD_DIR)/forwarding
	./$<

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/forwarding $(BUILD_DIR)/nonforwarding