//
//  MetaString4.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef MetaString4_h
#define MetaString4_h

#include "Inline.h"
#include "Indexes.h"
#include "MetaRandom.h"
#include "Log.h"

namespace andrivet { namespace ADVobfuscator {

template<int N, int Key, typename Indexes>
struct MetaString4;

template<int K, int... I>
struct MetaString4<0, K, Indexes<I...>>
{
    constexpr ALWAYS_INLINE MetaString4(const char* str)
    : buffer_ {static_cast<char>(K), encrypt(str[I])...} { }

    const char* decrypt()
    {
        for(int i = 0; i < sizeof...(I); ++i)
            buffer_[i + 1] = decrypt(buffer_[i + 1]);
        buffer_[sizeof...(I) + 1] = 0;
        LOG("--- Select MetaString4 implementation #" << 0 << " with key 0x" << hex(buffer_[0]));
        return buffer_ + 1;
    }

private:
    constexpr char encrypt(char c) const { return c ^ buffer_[0]; }
    constexpr char decrypt(char c) const { return encrypt(c); }

    char buffer_[sizeof...(I) + 2];
};

template<int K, int... I>
struct MetaString4<1, K, Indexes<I...>>
{
    constexpr ALWAYS_INLINE MetaString4(const char* str)
    : buffer_ {static_cast<char>(K), encrypt(str[I], I)...} { }
    
    const char* decrypt()
    {
        for(int i = 0; i < sizeof...(I); ++i)
            buffer_[i + 1] = decrypt(buffer_[i + 1], i);
        buffer_[sizeof...(I) + 1] = 0;
        LOG("--- Select MetaString4 implementation #" << 1 << " with key 0x" << hex(buffer_[0]));
        return buffer_ + 1;
    }
    
private:
    constexpr char key(int position) const { return buffer_[0] + position; }
    constexpr char encrypt(char c, int position) const { return c ^ key(position); }
    constexpr char decrypt(char c, int position) const { return encrypt(c, position); }
    
    char buffer_[sizeof...(I) + 2];
};

// Shift the value of each character and does not store the key. It is only used at compile-time.
template<int K, int... I>
struct MetaString4<2, K, Indexes<I...>>
{
    constexpr ALWAYS_INLINE MetaString4(const char* str)
    : buffer_ {encrypt(str[I])..., key(K)} { }
    
    const char* decrypt()
    {
        for(int i = 0; i < sizeof...(I); ++i)
            buffer_[i] = decrypt(buffer_[i]);
        buffer_[sizeof...(I)] = 0;
        LOG("--- Select MetaString4 implementation #" << 2 << " with key 0x" << hex(key(K)));
        return buffer_;
    }
    
private:
    constexpr char key(int key) const { return key % 13; }
    constexpr char encrypt(char c) const { return c + key(K); }
    constexpr char decrypt(char c) const { return c - key(K); }
    
    char buffer_[sizeof...(I) + 1];
};

template<int N>
struct MetaRandomChar4
{
    static const char value = static_cast<char>(1 + MetaRandom<N, 0xFF - 1>::value);
};
    
}}

#define DEF_OBFUSCATED4(str) MetaString4<andrivet::ADVobfuscator::MetaRandom<__COUNTER__, 3>::value, andrivet::ADVobfuscator::MetaRandomChar4<__COUNTER__>::value, Make_Indexes<sizeof(str) - 1>::type>(str)

#define OBFUSCATED4(str) (DEF_OBFUSCATED4(str).decrypt())

#endif
