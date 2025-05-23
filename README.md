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
cmake -B libmpix_example_posix/build
cmake --build libmpix_example_posix/build

# Run the test binary
libmpix_example_posix/build/posix_example
```
