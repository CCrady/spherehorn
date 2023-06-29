# Spherehorn
**Spherehorn** is an esolang (**eso**teric programming **lang**uage) that [I](https://github.com/CCrady)
created. It's named after the [Alexander horned sphere](https://en.wikipedia.org/wiki/Alexander_horned_sphere),
due to a passing resemblance between the horned sphere construction and the
'circular tree' data structure that Spherehorn uses for code and memory.

For a short guide to the language, see [guide.md](guide.md).

## Compiling Spherehorn
### \*nix with g++
You'll need make and g++ to compile Spherehorn. If you don't have them, use your
package manager of choice to install them; with APT,
`sudo apt install build-essential` works nicely. Once you've `clone`d/downloaded
this repository, navigate to the root directory (where the `makefile` is) in
your terminal emulator and run the command `make`. When it's done executing,
you'll find a new file, `spherehorn`, in the directory. Try running
`echo "3 5 50" | ./spherehorn examples/fizzbuzz.spherehorn` and see what
happens.
### Others
If you want to use a different compiler, switch the line `CXX := g++` out in
`makefile` for whatever you're using. You may also need to alter or remove the
`CXXFLAGS` line below it. If you'd rather not use `make` or one of its
replacements, compile all the .cpp files in the repositiory together except for
`unit_tests.cpp`.

