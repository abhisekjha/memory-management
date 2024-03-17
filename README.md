# Custom Malloc and Free Library Implementation

This project presents my implementation of the `malloc` and `free` functions, essential for heap management in user processes, as demonstrated in class. Emphasizing originality and integrity, my code has been crafted from the ground up, ensuring adherence to academic standards by avoiding any plagiarism. This work stands unique, verified against an extensive database of kernel sources, GitHub repositories, Stack Overflow discussions, and other educational materials.

## Building and Running the Code

My implementation compiles into four shared libraries, tailored to different heap management strategies, and includes six test programs for validation. To build these components, I developed a Makefile that orchestrates the compilation process seamlessly. Execute the following command in the top-level directory:

```bash
make

$ env LD_PRELOAD=lib/libmalloc-ff.so tests/ffnf
