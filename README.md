# fltk-metal
How to use Metal to draw in an FLTK window...

Only for MacOS.
Make a window with [FLTK](https://github.com/fltk/fltk) and use [metal-cpp](https://github.com/bkaradzic/metal-cpp) as C++ interface for rendering with Metal.

## building
All dependencies are delivered with the code, but FLTK is delivered as submodule, so don't forget to call

    git submodule update --init    
    
before you build the code with [CMake](https://cmake.org).
