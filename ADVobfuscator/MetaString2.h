//
//  MetaString2.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef MetaString2_h
#define MetaString2_h

#include "Inline.h"
#include "Indexes.h"

namespace andrivet { namespace ADVobfuscator {

template<typename Indexes>
struct MetaString2;

template<int... I>
struct MetaString2<Indexes<I...>>
{
    constexpr ALWAYS_INLINE MetaString2(const char* str)
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


#define OBFUSCATED2(str) (MetaString2<Make_Indexes<sizeof(str) - 1>::type>(str).decrypt())

constexpr ALWAYS_INLINE const char* operator "" _obfuscated2(const char* str, size_t n)
{
    using I = Make_Indexes<32>::type;
    return MetaString2<I>(str).decrypt();
}
    
}}

#endif
