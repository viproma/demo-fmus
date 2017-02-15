Demo FMUs
=========
This is a repository of simple functional mock-up units (FMUs) for testing and
demonstration purposes.

If you don't know what an FMU is, you're probably in the wrong place, but
check out [the FMI page on viproma.no](http://viproma.no/doku.php?id=vpf:fmi)
in any case.

Issues and contributions
------------------------
Please use [GitHub](http://github.com/viproma/demo-fmus) to report issues
and submit pull requests.

Requirements
------------
To build this software, you'll need a C++ compiler with decent C++11
support (VS 2013 or GCC 4.9 will do) as well as [CMake](https://cmake.org)
version 2.8.12 or greater.

In addition, [CPPFMU](https://github.com/viproma/cppfmu) must be pulled in as
a Git submodule, while [fmu-uuid](https://github.com/viproma/fmu-uuid) gets
automatically pulled in and built by the CMake scripts.  Check out their
READMEs for additional dependencies.

Building
--------
To clone the repository and get the CPPFMU submodule, run the following
commands:

    git clone https://github.com/viproma/demo-fmus.git
    cd demo-fmus
    git submodule init
    git submodule update

### Simple build
If you only need to build the FMU contents (without zipping them together to
form the actual FMU) for one platform, you can now use the standard CMake
procedure to do so:

    mkdir build
    cd build
    cmake ..
    cmake --build .

You'll now find the FMU contents under `build/fmu-contents`.  Of course, you
can substitute `build` with a path of your choice; CMake doesn't care.  Just
remember to supply the correct source path in the generation step (third
command).

### Complete build
To simplify the building and packaging of complete FMUs we have created two
scripts:

  * `build-vs.cmd`, a Windows batch script that uses CMake's Visual Studio
    generators. Use this to build FMUs that support both 32- and 64-bit
    Windows platforms.

  * `build-make.cmd`, a Bash script that uses CMake's Makefile generator.
    Use this to build FMUs that support both 32- and 64-bit variants
    of whichever UNIX-like platform you run it on.

Note that one FMU can in principle contain code for several different software
platforms (e.g. Windows, Linux, Mac OS X, etc.), and not just 32- and 64-bit
versions of *one* platform.  However, we don't provide any automated way to
do this; you'll have to manually combine the FMUs yourself.

On Windows, the script is run as follows:

    build-vs <MSVC version> [output directory]

Here, `<MSVC version>` must be replaced with the Visual C++ version, e.g. `12`
for MSVC 12.0 (2013) or `14` for MSVC 14.0 (2015), while `[output directory]`
is an optional path to a directory for the output files.  If not specified,
it will default to `build-vs-output`.  For example:

    build-vs 12 ..\generated-fmus

Terms of use
------------
This software is distributed under the 3-Clause BSD License.
(See accompanying file LICENCE.txt or copy at [GitHub](
https://raw.githubusercontent.com/viproma/demo-fmus/master/LICENCE.txt).)
