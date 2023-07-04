# Spherehorn
**Spherehorn** is an esolang (**eso**teric programming **lang**uage) that [I](https://github.com/CCrady)
created. It's named after the [Alexander horned sphere](https://en.wikipedia.org/wiki/Alexander_horned_sphere),
due to a passing resemblance between the horned sphere construction and the
'circular tree' data structure that Spherehorn uses for code and memory.

This repository is an interpreter for Spherehorn. For a short guide to the
language, see [guide.md](guide.md). There are various example programs in the
[examples/](examples/) directory.

## Compiling Spherehorn

### \*nix with build.sh
You'll need g++, or a compatible compiler, to compile Spherehorn. If you don't
have a C++ compiler, install g++ using your package manager of choice; with APT,
running `sudo apt update && sudo apt install g++` works nicely.

Once you've `clone`d/downloaded this repository, navigate to its root directory
(where `build.sh` is) in your terminal emulator and run the build script with
`sh build.sh`. When it's done executing, you'll find a new file, `spherehorn`,
in the directory. Try running
`echo "3 5 50" | ./spherehorn examples/fizzbuzz.spherehorn` and see what
happens.

If you're using a non-g++ compiler, switch out the line `compiler=g++` in
`build.sh` for your compiler. The new compiler must be compatible with g++'s
command line options; clang should work, but I haven't tested it.

If you don't have the Bourne shell (`sh`) installed, the build script should
also work with bash, ksh, and zsh.

### \*nix with make
The repository includes a makefile if you'd like to use make. It was written for
GNU make, so it may not work correctly with your dialect. You can change the
line `CXX := g++` if you want to use a different compiler; you may also need to
change the compiler flag variables.

### Other systems/compilers
Compile together all the .cpp files in the `src/` directory. The project uses
the C++20 standard.

