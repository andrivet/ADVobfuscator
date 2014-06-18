//
//  MetaString1.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef MetaString1_h
#define MetaString1_h

#include "Inline.h"

namespace andrivet { namespace ADVobfuscator {

template<int... I>
struct MetaString1
{
    constexpr ALWAYS_INLINE MetaString1(const char* str)
    : buffer_ {encrypt(str[I])...} { }
    
    const char* decrypt()
    {
        for(int i = 0; i < sizeof...(I); ++i)
            buffer_[i] = decrypt(buffer_[i]);
        buffer_[sizeof...(I)] = 0;
        return buffer_;
    }

private:
    constexpr char encrypt(char c) const { return c ^ 0x55; }
    constexpr char decrypt(char c) const { return encrypt(c); }
    
private:
    char buffer_[sizeof...(I) + 1];
};

inline const char* operator "" _obfuscated1(const char* str, size_t)
{
    return MetaString1<0, 1, 2, 3, 4, 5>(str).decrypt();
}
    
}}

#define OBFUSCATED1(str) (MetaString1<0, 1, 2, 3, 4, 5>(str).decrypt())

#endif
