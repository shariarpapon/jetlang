## jetlang

jetlang is an experiemental programming language and compiler project. I mainly started this project for exploring low level systems design and compiler architecture.

## Features

- Custom syntax and AST parsing utility.
- All tools and data structures (arena allocator, hashtables, logger, etc) built as stb-style libraries from scratch.
- Modular design focusing on clarity and runtime memory safety.
- Pretty printing utilities (for data structures) for debug purposes.

## Getting Started

### Tools
    - C Compiler (gcc or compatible)
    - Make (to use the provided makefile with gcc)


### Step 1: Clone the repository
    git clone https://github.com/shariarpapon/jetlang.git
    cd jetlang

### Step 2: Compile

#### With make
##### Simply run:
    make

#### Without make
##### Compile all .c files manually with GCC, including the header directories:
    gcc -Iinclude -Iutils src/*.c utils/*.c -o jet

### Step 3: Run

##### Flags:
    -p,a = print AST.
    -p,t = print token array.
    -p,pe = print parser errors.

For the same command, arguments can be supplied together (e.g. ```-p,a,t,pe```).  
File command ```-f,filepath``` is required.  

- Linux:
    ./jet -f,filepath

- Windows:
    jet.exe -f,filepath

