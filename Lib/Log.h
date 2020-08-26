//
//  Log.h
//  ADVobfuscator
//
// Copyright (c) 2010-2017, Sebastien Andrivet
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Get latest version on https://github.com/andrivet/ADVobfuscator

#ifndef Log_h
#define Log_h

#include <iomanip>
#include <iostream> // [fokede] mingw compatibility

namespace andrivet { namespace ADVobfuscator {

// Inspired from work of Martin Stettner and Jimmy J

struct HexChar
{
    unsigned char c_;
    unsigned width_;
    HexChar(unsigned char c, unsigned width) : c_{c}, width_{width} {}
};

inline std::ostream& operator<<(std::ostream& o, const HexChar& c)
{
    return (o << std::setw(c.width_) << std::setfill('0') << std::hex << (int)c.c_ << std::dec);
}

inline HexChar hex(char c, int w = 2)
{
    return HexChar(c, w);
}

}}

#if (defined(ADVLOG) && ADVLOG == 1)
#define LOG(str) std::cerr << str << std::endl
#else
#define LOG(str) ((void)0)
#endif

#endif
