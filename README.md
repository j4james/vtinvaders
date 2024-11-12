VT Invaders
===========

![Screenshot](screenshot.png)

This is a clone of the 1978 [Space Invaders] arcade game, designed to be
played on a DEC VT terminal. It requires at least a VT320 (or something of
comparable functionality), but a VT525 is best if you want color.

You'll also need at least a 19200 baud connection to play at the default
frame rate. If you find the input is lagging, try selecting a slower speed
using the command line option `--speed 4` or  `--speed 3`.

[Space Invaders]: https://en.wikipedia.org/wiki/Space_Invaders


Controls
--------

Use the `←` `→` arrow keys to move, `SPACE` to fire, and `Q` to quit.


Download
--------

The latest binaries can be found on GitHub at the following url:

https://github.com/j4james/vtinvaders/releases/latest

For Linux download `vtinvaders`, and for Windows download `vtinvaders.exe`.


Build Instructions
------------------

If you want to build this yourself, you'll need [CMake] version 3.15 or later
and a C++ compiler supporting C++20 or later.

1. Download or clone the source:  
   `git clone https://github.com/j4james/vtinvaders.git`

2. Change into the build directory:  
   `cd vtinvaders/build`

3. Generate the build project:  
   `cmake -D CMAKE_BUILD_TYPE=Release ..`

4. Start the build:  
   `cmake --build . --config Release`

[CMake]: https://cmake.org/


Supported Terminals
-------------------

| Terminal           | Color |
|--------------------|:-----:|
| DEC VT320          | no    |
| DEC VT330/340      | no    |
| DEC VT382          | no    |
| DEC VT420          | no    |
| DEC VT510/520      | no    |
| DEC VT525          | yes   |
| KoalaTerm          | no    |
| MLTerm             | yes   |
| PowerTerm          | no    |
| Reflection Desktop | no    |
| RLogin             | yes   |
| VTStar             | yes   |
| Windows Terminal   | yes   |

You could also get by with a VT220 or VT240, but those terminals don't have
full-cell fonts, so the graphics will look a bit messed up.


License
-------

The VT Invaders source code and binaries are released under the MIT License.
See the [LICENSE] file for full license details.

[LICENSE]: LICENSE.txt
