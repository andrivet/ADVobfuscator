What is ADVobfuscator?
======================

ADVobfuscator demonstates how to use C++11/14 language to generate, at compile time, obfuscated code without using any external tool and without modifying the compiler. The technics presented rely only on C++11/14, as standardized by ISO. It shows also how to introduce some form of randomness to generate polymorphic code and it gives some concrete examples like the encryption of strings literals and the obfuscation of calls using finite state machines.

ADVobfuscator is delivered as a set of source file (source code library). You have to include them into your project and call the appropriate functions or macro. This is not a magic black box, some kind of proprocessor or anything like that.

If you are looking for a more transparent obfuscator, look at [Obfuscator-LLVM](https://github.com/obfuscator-llvm/obfuscator/wiki) or [strong.codes](https://strong.codes) (commercial version of Obfuscator-LLVM). *Note: I have no affiliation with Obfuscator-LLVM and strong.codes. I just know some of these swiss folks.*


News
====

August 2016
-----------

Several enhancement (suppress warnings, fix some errors, enhance portability). In more details:

- Increase the warning level for Visual Studio and GCC
- Remove several (all) compilation warnings
- Replace `int` by `size_t` when it is appropriate (`sizeof`, `position`)
- Remove unused parameters
- Keys are now of type `char` (were of type `int`, it was wrong)
- Replace the non-portable `##__VA_ARGS__` by the portable `__VA_ARGS__`. As a consequence, new macros (ending with `0`) are defined when there are no parameters
- Remove some (stupid) syntax errors (`;` at the end of some functions). C/C++ is odd: you have to put a `;` when you define a `struct`, but none when you define a function
- A integral type is computed at compile time to store pointers to functions

August 2015
-----------

ADVobfuscator code has been updated for Visual Studio 2015. ADVobfuscator is now compatible with the RTM release of Visual Studio 2015 (previous versions or CTP releases are not). The whitepaper is not yet updated. The code has also been modified in order to avoid problems with O3 optimization and GCC. GCC with O3 defeats obfuscation (because it optimizes too much) and sometimes generates wrong code (not clear yet if it is a bug in GCC or in ADVobfuscator).


Future Directions
=================

These are ideas for the next few months:

- Switch to C++14. It will simplify some parts of the code and C++ is now well supported by compilers such as GCC and CLANG
- Try to better support Visual Studio 2015, but only if it does not implies a restructuring of the code
- Introduce **White Box Cryptography** (AES in particular). This is my current reseach project
- Clearly separate code from my talks (like `MetaFibonacci.h`, `MetaString1.h`, ...) from the actual code of the library. It will be easier to integrate the library into projects
- Is my code compatible with ASLR? In fact, I don't konw and I have to solve this (I need it for another project)
- Apply the techniques used in `MetaString4.h` into `ObfuscatedCall.h`
- )ntroduce Unit Testing


Prerequisites
=============

You have to install the [Boost library](www.boost.org) in order to use ADVdetector (it is used by FSM). Alternatively, you can comment out lines related to `ObfuscatCall*` (includes and code) in `main.cpp`.

- Debian / Ubuntu: `sudo apt-get install libboost-all-dev`
- Mac OS X: `brew install boost`
- Windows: [Download Boost](https://sourceforge.net/projects/boost/files/boost/1.61.0/boost_1_61_0.zip/download) and install it. Then you have to change the Visual Studio 2015 project to point to Boost

Debug Builds
============

Debug builds are very special: Compiler do not have (and do not most of the time) respect statement such as `inline` or `constexpr`. All optimizations are also, by default, disabled. Compilers are doing this for a good reason: let you debug, single step, etc.

As a consequence, ADVobfuscator is **not** compatible with Debug builds. It works only for Release builds.

Compatibility
=============

ADVobfuscator has been tested with:
- Xcode (LLVM) 7.3.0 under Mac OS X 10.11
- GCC 6.1.1 under Debian 9 (stretch/testing)
- Visual Studio 2015 (Update 3) under Windows 10

Other compilers are probably compatible if they are C++11/14 compliant.

Files
=====

| File                            | Description |
| ------------------------------- |-------------|
| `DetectDebugger.cpp`            | Debugger detection, implemented for Mac OS X and iOS. It is used by ObfuscatedCallWithPredicate (FSM) |
| `DetectDebugger.h`              | Debugger detection, declaration |
| `Indexes.h`                     | Generate list of indexes at compile time (0, 1, 2, ... N) |
| `Log.h`                         | Helper to log messages |
| `MetaFactorial.h`               | Compute factorial at compile time |
| `MetaFibonacci.h`               | Compute fibonacci sequence at compile time |
| `MetaFSM.h`                     | Template to generate Finite State Machines at compile time |
| `MetaRandom.h`                  | Generate a pseudo-random number at compile time |
| `MetaString1.h`                 | Obfuscated string - version 1 |
| `MetaString2.h`                 | Obfuscated string - version 2 - Remove truncation |
| `MetaString3.h`                 | Obfuscated string - version 3 - Random key |
| `MetaString4.h`                 | Obfuscated string - version 4 - Random encryption algorithm |
| `ObfuscatedCall.h`              | Obfuscate function call |
| `ObfuscatedCallWithPredicate.h` | Obfuscate function call, execute a FSM based on a predicate (such as DetectDebugger) |
| `main.cpp`                      | Samples |
| `ADVobfuscator.xcodeproj`       | Project for Apple Xcode |
| `ADVobfuscator.sln`             | Visual Studio 2015 Solution |
| `Makefile`                      | Simple makefile for GCC |


Copyright and license
=====================

Written by Sebastien Andrivet

Copyright &copy; 2010-2016 - Sebastien Andrivet
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.