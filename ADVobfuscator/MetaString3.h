//
//  MetaString3.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef MetaString3_h
#define MetaString3_h

#include "Inline.h"
#include "Indexes.h"
#include "MetaRandom.h"

namespace andrivet { namespace ADVobfuscator {

template<typename Indexes, int K>
struct MetaString3;

template<int... I, int K>
struct MetaString3<Indexes<I...>, K>
{
    constexpr ALWAYS_INLINE MetaString3(const char* str)
    : buffer_ {static_cast<char>(K), encrypt(str[I])...} { }
    
    const char* decrypt()
    {
        for(int i = 0; i < sizeof...(I); ++i)
            buffer_[i + 1] = decrypt(buffer_[i + 1]);
        buffer_[sizeof...(I) + 1] = 0;
        return buffer_ + 1;
    }
    
private:
    constexpr char key() const { return buffer_[0]; }
    constexpr char encrypt(char c) const { return c ^ key(); }
    constexpr char decrypt(char c) const { return encrypt(c); }
    
private:
    char buffer_[sizeof...(I) + 2];
};

template<int N>
struct MetaRandomChar3
{
    static const char value = static_cast<char>(1 + MetaRandom<N, 0xFF - 1>::value);
};
    
}}

#define OBFUSCATED3(str) (MetaString3<Make_Indexes<sizeof(str) - 1>::type, MetaRandomChar3<__COUNTER__>::value>(str).decrypt())

// It is still possible to define operator "" but it will always use the same key for all encrypted string.
// This is not what we want so use instead the macro ENCRYPTED3.

/*constexpr ALWAYS_INLINE const char* operator "" _obfuscated3(const char* str, size_t n)
{
    using I = Make_Indexes<32>::type;
    return MetaString3<I>(str).decrypt();
}*/

#endif
