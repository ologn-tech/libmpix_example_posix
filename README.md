POSIX libmpix example
=====================

An example application that runs on any reasonably POSIX system, such as Linux, BSD, Mac OS, as
well as some RTOS.

```sh
export LIBMPIX="$PWD/libmpix"

# Clone the library and the example repo
git clone https://github.com/libmpix/libmpix
git clone https://github.com/libmpix/libmpix_example_posix

# Build the project
cd libmpix_example_posix
cmake -B build && cmake --build build

# Run the test binary
build/libmpix_example_posix
```
