//
//  MetaFactorial.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef MetaFactorial_h
#define MetaFactorial_h

// N! = 1 * 2 * 3 * 4 * ... * N
// N! = N * (N - 1)!

namespace andrivet { namespace ADVobfuscator {
    
template<int N>
struct Factorial
{
    static const int value = N * Factorial<N-1>::value;
};

template<>
struct Factorial<0>
{
    static const int value = 1;
};

}}
    
#endif
