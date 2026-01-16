# Makefile for Step 3: File Copy Programs
# Compiles both library functions and system calls versions

CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = copy_functions copy_syscalls

# Default target: build all programs
all: $(TARGETS)

# Build copy_functions (uses library functions)
copy_functions: copy_functions.c
	$(CC) $(CFLAGS) -o copy_functions copy_functions.c

# Build copy_syscalls (uses system calls)
copy_syscalls: copy_syscalls.c
	$(CC) $(CFLAGS) -o copy_syscalls copy_syscalls.c

# Run tests
test: all
	@echo "\n========================================="
	@echo "Testing Library Functions Approach"
	@echo "========================================="
	@echo "\n--- Test 1: Copying text file ---"
	./copy_functions test_text.txt output_text_functions.txt
	@echo "\n--- Test 2: Copying binary file ---"
	./copy_functions test_binary.bin output_binary_functions.bin
	@echo "\n--- Test 3: Copying PDF file ---"
	./copy_functions CN_step3.pdf output_pdf_functions.pdf
	@echo "\n========================================="
	@echo "Testing System Calls Approach"
	@echo "========================================="
	@echo "\n--- Test 1: Copying text file ---"
	./copy_syscalls test_text.txt output_text_syscalls.txt
	@echo "\n--- Test 2: Copying binary file ---"
	./copy_syscalls test_binary.bin output_binary_syscalls.bin
	@echo "\n--- Test 3: Copying PDF file ---"
	./copy_syscalls CN_step3.pdf output_pdf_syscalls.pdf
	@echo "\n========================================="
	@echo "Verifying file integrity"
	@echo "========================================="
	@diff test_text.txt output_text_functions.txt && echo "[OK] Text file (functions) copied correctly"
	@diff test_text.txt output_text_syscalls.txt && echo "[OK] Text file (syscalls) copied correctly"
	@diff test_binary.bin output_binary_functions.bin && echo "[OK] Binary file (functions) copied correctly"
	@diff test_binary.bin output_binary_syscalls.bin && echo "[OK] Binary file (syscalls) copied correctly"
	@diff CN_step3.pdf output_pdf_functions.pdf && echo "[OK] PDF file (functions) copied correctly"
	@diff CN_step3.pdf output_pdf_syscalls.pdf && echo "[OK] PDF file (syscalls) copied correctly"
	@echo "\n========================================="
	@echo "All tests passed!"
	@echo "========================================="

# Clean up compiled files and output files
clean:
	rm -f $(TARGETS) output_*.txt output_*.bin output_*.pdf

# Clean everything including test files
cleanall: clean
	rm -f test_binary.bin

.PHONY: all test clean cleanall
