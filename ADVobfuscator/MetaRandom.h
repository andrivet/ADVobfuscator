//
//  MetaRandom.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef MetaRandom_h
#define MetaRandom_h

// Very simple compile-time random numbers generator.

// For a more complete and sophisticated example, see:
// http://www.researchgate.net/profile/Zalan_Szgyi/publication/259005783_Random_number_generator_for_C_template_metaprograms/file/e0b49529b48272c5a6.pdf


// I use the current (compile time) as a seed

namespace andrivet { namespace ADVobfuscator {
    
constexpr char time[] = __TIME__; // __TIME__ has the following format: hh:mm:ss in 24-hour time

constexpr int DigitToInt(char c) { return c - '0'; }
const int seed = DigitToInt(time[7]) + DigitToInt(time[6]) * 10 +
                      DigitToInt(time[4]) * 60 + DigitToInt(time[3]) * 600 +
                      DigitToInt(time[1]) * 3600 + DigitToInt(time[0]) * 36000;

template<int N>
struct MetaRandomGenerator
{
private:
    static const int a = 16807;
    static const int m = 2147483647;
    static const int q = 127773;
    static const int r = 2836;
  
    static const int s = MetaRandomGenerator<N - 1>::value;
    static const int k = s / q;
    static const int n = a * (s - k * q) - k * r;
    
public:
    static const int value = n < 0 ? n + m : n;
};

template<>
struct MetaRandomGenerator<0>
{
    static const int value = seed;
};

template<int N, int M>
struct MetaRandom
{
    static const int value = MetaRandomGenerator<N + 1>::value % M;
};

}}

#endif
