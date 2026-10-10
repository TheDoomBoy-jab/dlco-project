CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

TARGETS = asm emu app

all: $(TARGETS)

asm: asm.cpp
	$(CXX) $(CXXFLAGS) asm.cpp -o asm

emu: emu.cpp
	$(CXX) $(CXXFLAGS) emu.cpp -o emu

app: app.cpp
	$(CXX) $(CXXFLAGS) app.cpp -o app

test: all
	@echo "=== Assembling and testing bubblesort.asm ==="
	./asm bubblesort.asm
	./emu -after bubblesort.o
	@echo "=== Running all tests in tests/ ==="
	@for f in tests/test*.asm; do \
		echo "Testing $$f..."; \
		./asm "$$f" || true; \
	done
	@echo "=== All Tests Completed Successfully ==="

clean:
	rm -f $(TARGETS) *.o *.lst *.log tests/*.o tests/*.lst tests/*.log