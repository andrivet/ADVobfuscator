//
//  Log.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef Log_h
#define Log_h

#include <iomanip>

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
    return (o << std::setw(c.width_) << std::setfill('0') << std::hex << (int)c.c_);
}

inline HexChar hex(char c, int w = 2)
{
    return HexChar(c, w);
}
    
}}

#if defined(DEBUG) && DEBUG == 1
#define LOG(str) std::cerr << str << "\n"
#else
#define LOG(str) ((void)0)
#endif

#endif
