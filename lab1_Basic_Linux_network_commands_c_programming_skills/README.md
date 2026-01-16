# Step 3: File Copy Performance Comparison

## Overview
This project compares two approaches for copying files in C:
1. **Library Functions**: Using `fopen()`, `fread()`, `fwrite()`, `fclose()`
2. **System Calls**: Using `open()`, `read()`, `write()`, `close()`

## Files
- `copy_functions.c` - File copy using C library functions
- `copy_syscalls.c` - File copy using UNIX system calls
- `Makefile` - Build and test automation
- `test_text.txt` - Small text test file (747 bytes)
- `test_binary.bin` - Large binary test file (10 MB)
- `CN_step3.pdf` - Medium PDF test file (~509 KB)

## Compilation

```bash
make all
```

This compiles both programs with:
- `-Wall -Wextra` for comprehensive warnings
- `-O2` for optimization

## Usage

### Library Functions Version
```bash
./copy_functions <source_file> <destination_file>
```

### System Calls Version
```bash
./copy_syscalls <source_file> <destination_file>
```

## Running Tests

```bash
make test
```

This will:
1. Compile both programs
2. Test with text, binary, and PDF files
3. Verify file integrity using `diff`
4. Display timing results

## Performance Results

**Test Environment**: Linux Virtual Machine (linux10620)

### Test 1: Small Text File (747 bytes)
| Method            | Time (seconds) |
|-------------------|----------------|
| Library Functions | 0.000182       |
| System Calls      | 0.000156       |

**Result**: System calls ~14% faster

### Test 2: Large Binary File (10 MB)
| Method            | Time (seconds) |
|-------------------|----------------|
| Library Functions | 0.007378       |
| System Calls      | 0.005866       |

**Result**: System calls ~20% faster

### Test 3: Medium PDF File (~509 KB)
| Method            | Time (seconds) |
|-------------------|----------------|
| Library Functions | 0.000637       |
| System Calls      | 0.000358       |

**Result**: System calls ~44% faster

## Analysis

### Key Observations

1. **Similar Buffer Sizes**: Both implementations use 4KB buffers, which minimizes the traditional advantage of library function buffering.

2. **CPU Time vs Wall-Clock Time**: The `clock()` function measures CPU time used by the process, not actual elapsed time. This includes:
   - Time spent executing instructions
   - Does NOT include time waiting for I/O
   - Does NOT include time the OS spends in kernel mode for system calls

3. **System Call Overhead**: In traditional benchmarks, library functions are faster because:
   - They buffer multiple operations into fewer system calls
   - Reduces context switches between user mode and kernel mode
   - However, with our 4KB buffer, both approaches make similar numbers of system calls

4. **Optimization Level**: Both programs are compiled with `-O2`, which applies compiler optimizations that can affect timing.

### Why System Calls Appear Faster Here

The system calls version shows slightly better performance in these tests for several reasons:

1. **Less Overhead**: Library functions (`fread`/`fwrite`) add an extra layer of abstraction and error checking beyond what system calls provide.

2. **Direct Kernel Access**: System calls go directly to the kernel, while library functions must:
   - Maintain internal buffers and state
   - Perform additional bookkeeping
   - Handle more complex error scenarios

3. **Buffer Size Parity**: Since both use 4KB buffers, the library functions don't get their traditional advantage of batching many small operations.

4. **Modern OS Optimization**: Modern operating systems have highly optimized I/O subsystems that make system calls very efficient.

### Real-World Implications

In practice:
- For **small, frequent operations** without explicit buffering, library functions would be significantly faster
- For **large block operations** with manual buffering (as we've done), the difference is minimal
- The **convenience and portability** of library functions often outweighs minor performance differences

## Technical Details

### Library Functions Approach
- Uses standard I/O buffering
- Binary mode (`"rb"`, `"wb"`) for universal file handling
- Automatic buffer management by the C library
- More portable across different systems

### System Calls Approach
- Direct UNIX system calls
- Manual buffer management
- Lower-level control over I/O operations
- UNIX/Linux specific (requires `<fcntl.h>` and `<unistd.h>`)

### Timing Methodology
Both programs use the same timing approach:
```c
#include <time.h>

clock_t start, end;
double cpu_time_used;

start = clock();
// ... file copy operations ...
end = clock();

cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
```

## Verification
All copied files are verified using `diff` to ensure byte-for-byte accuracy:
- [OK] Text files copied correctly
- [OK] Binary files copied correctly
- [OK] PDF files copied correctly

## Cleanup

Remove compiled programs and output files:
```bash
make clean
```

Remove everything including test files:
```bash
make cleanall
```

## Conclusions

1. Both approaches successfully copy binary and text files with 100% accuracy
2. Performance differences are minimal with proper buffering
3. System calls showed slightly better performance in these tests due to:
   - Reduced overhead with explicit buffering
   - Direct kernel access
   - Modern OS optimizations
4. The choice between approaches should consider:
   - Portability requirements
   - Code complexity
   - Specific use case requirements
   - Maintainability

## References
- [GeeksforGeeks: fwrite() vs write()](https://www.geeksforgeeks.org/c/fwrite-vs-write/)
- [System Call vs Library Call](https://www.geeksforgeeks.org/difference-between-system-call-and-library-call/)
