# natrix

natrix is a simple dynamically typed programming language inspired by Python.
The focus of natrix is readable implementation for demonstrating the concepts 
of interpreters and programming languages. Specifically, neither performance, 
compatibility, nor practicality are goals of natrix.


## Building

natrix is written in C17 and uses CMake for building. To build natrix, run the 
following commands:

```sh
mkdir build
cd build
cmake ..
make natrix
```

Currently, natrix is built and tested on WSL2 with Ubuntu 22.04.1 LTS, but it 
should work on other POSIX systems as well.


## Usage

To run a natrix program, use the following command in the `build` directory:

```sh
./natrix <path-to-natrix-file>
```


## Running tests

To build and run the tests, use the following commands in the `build` directory:

```sh
make check
```


## Generating documentation

The sources are documented using doxygen comments. HTML documentation can be
generated using the following command in the project directory:

```sh
doxygen Doxyfile
```
Then open `doc/doxygen-html/index.html` in a web browser.