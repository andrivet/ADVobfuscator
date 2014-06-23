//
//  MetaString4.h
//  ADVobfuscator
//
// Copyright (c) 2010-2014, Sebastien Andrivet
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

#ifndef MetaString4_h
#define MetaString4_h

#include "Inline.h"
#include "Indexes.h"
#include "MetaRandom.h"
#include "Log.h"

// Fourth implementation of an obfuscated string
// No limitation:
// - No truncation
// - Key generated at compile time
// - Algorithm selected at compile time (there are three examples below)

namespace andrivet { namespace ADVobfuscator {

// Represents an obfuscated string, parametrized with an alrorithm number N, a list of indexes Indexes and a key Key

template<int N, int Key, typename Indexes>
struct MetaString4;

// Partial specialization with a list of indexes I, a key K and algorithm N = 0
// Each character is encrypted (XOR) with the same key, stored at the beginning of the buffer

template<int K, int... I>
struct MetaString4<0, K, Indexes<I...>>
{
    // Constructor. Evaluated at compile time. Key is stored as the first element of the buffer
    constexpr ALWAYS_INLINE MetaString4(const char* str)
    : buffer_ {static_cast<char>(K), encrypt(str[I])...} { }

    // Runtime decryption. Most of the time, inlined
    inline const char* decrypt()
    {
        for(int i = 0; i < sizeof...(I); ++i)
            buffer_[i + 1] = decrypt(buffer_[i + 1]);
        buffer_[sizeof...(I) + 1] = 0;
        LOG("--- Select MetaString4 implementation #" << 0 << " with key 0x" << hex(buffer_[0]));
        return buffer_ + 1;
    }

private:
    // Encrypt / decrypt a character of the original string with the key
    constexpr char key() const { return buffer_[0]; }
    constexpr char encrypt(char c) const { return c ^ key(); }
    constexpr char decrypt(char c) const { return encrypt(c); }

    // Buffer to store the encrypted string + terminating null byte + key
    char buffer_[sizeof...(I) + 2];
};

// Partial specialization with a list of indexes I, a key K and algorithm N = 1
// Each character is encrypted (XOR) with an incremented key. The first key is stored at the beginning of the buffer

template<int K, int... I>
struct MetaString4<1, K, Indexes<I...>>
{
    // Constructor. Evaluated at compile time. Key is stored as the first element of the buffer
    constexpr ALWAYS_INLINE MetaString4(const char* str)
    : buffer_ {static_cast<char>(K), encrypt(str[I], I)...} { }
    
    // Runtime decryption. Most of the time, inlined
    inline const char* decrypt()
    {
        for(int i = 0; i < sizeof...(I); ++i)
            buffer_[i + 1] = decrypt(buffer_[i + 1], i);
        buffer_[sizeof...(I) + 1] = 0;
        LOG("--- Select MetaString4 implementation #" << 1 << " with key 0x" << hex(buffer_[0]));
        return buffer_ + 1;
    }
    
private:
    // Encrypt / decrypt a character of the original string with the key
    constexpr char key(int position) const { return buffer_[0] + position; }
    constexpr char encrypt(char c, int position) const { return c ^ key(position); }
    constexpr char decrypt(char c, int position) const { return encrypt(c, position); }
    
    // Buffer to store the encrypted string + terminating null byte + key
    char buffer_[sizeof...(I) + 2];
};

// Partial specialization with a list of indexes I, a key K and algorithm N = 2
// Shift the value of each character and does not store the key. It is only used at compile-time.

template<int K, int... I>
struct MetaString4<2, K, Indexes<I...>>
{
    // Constructor. Evaluated at compile time. Key is *not* stored
    constexpr ALWAYS_INLINE MetaString4(const char* str)
    : buffer_ {encrypt(str[I])..., 0} { }
    
    // Runtime decryption. Most of the time, inlined
    inline const char* decrypt()
    {
        for(int i = 0; i < sizeof...(I); ++i)
            buffer_[i] = decrypt(buffer_[i]);
        LOG("--- Select MetaString4 implementation #" << 2 << " with key 0x" << hex(key(K)));
        return buffer_;
    }
    
private:
    // Encrypt / decrypt a character of the original string with the key
    constexpr char key(int key) const { return key % 13; }
    constexpr char encrypt(char c) const { return c + key(K); }
    constexpr char decrypt(char c) const { return c - key(K); }
    
    // Buffer to store the encrypted string + terminating null byte. Key is not stored
    char buffer_[sizeof...(I) + 1];
};

// Helper to generate a key
template<int N>
struct MetaRandomChar4
{
    static const char value = static_cast<char>(1 + MetaRandom<N, 0xFF - 1>::value);
};
    
}}

// Prefix notation
#define DEF_OBFUSCATED4(str) MetaString4<andrivet::ADVobfuscator::MetaRandom<__COUNTER__, 3>::value, andrivet::ADVobfuscator::MetaRandomChar4<__COUNTER__>::value, Make_Indexes<sizeof(str) - 1>::type>(str)

#define OBFUSCATED4(str) (DEF_OBFUSCATED4(str).decrypt())

#endif
