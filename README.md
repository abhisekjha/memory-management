# Custom Malloc and Free Library Implementation

This project presents my implementation of the `malloc` and `free` functions, essential for heap management in user processes, as demonstrated in class. 

## Building and Running the Code

My implementation compiles into four shared libraries, tailored to different heap management strategies, and includes six test programs for validation. To build these components, I developed a Makefile that orchestrates the compilation process seamlessly. Execute the following command in the top-level directory:

```bash
make

$ env LD_PRELOAD=lib/libmalloc-ff.so tests/ffnf

# Custom Malloc and Free Library Implementation

To experiment with other strategies, simply replace `libmalloc-ff.so` with the appropriate library:

- Best-Fit: `libmalloc-bf.so`
- First-Fit: `libmalloc-ff.so`
- Next-Fit: `libmalloc-nf.so`
- Worst-Fit: `libmalloc-wf.so`

## Program Requirements and Implementations

Within the provided `malloc` and `free` framework, I meticulously implemented several key features:

- **Splitting and Coalescing of Free Blocks**: I ensured adjacent free blocks are merged, and blocks larger than the requested size are appropriately split.
- **Heap Management Strategies**: I extended the library with three additional strategies: Next Fit, Worst Fit, and Best Fit, complementing the pre-implemented First Fit.
- **Event Counters**: I integrated counters within the library to track `malloc` and `free` calls, block reuse, requests for new blocks, splits, coalesces, the number of blocks in the free list, total memory requested, and maximum heap size.

The library is designed to update these counters accurately, reflecting the dynamic behavior of heap management. Upon exit, the program prints these statistics, offering a transparent view into the allocation patterns and efficiency of the implemented strategies.

## Benchmarking

To validate the effectiveness of my implementations, I conducted extensive benchmarking against the standard system `malloc`. This suite of tests, developed as part of the project, assesses performance, heap growth, fragmentation, and maximum heap size. My benchmarks provided a comparative analysis, revealing the strengths and nuances of each custom allocator strategy in various scenarios.

## Conclusion

Through this assignment, I deepened my understanding of dynamic memory allocation and system-level programming in C. The challenges of implementing efficient and effective heap management strategies have honed my skills and provided invaluable insights into the intricacies of low-level memory management.


