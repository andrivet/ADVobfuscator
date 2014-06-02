//
//  MetaFibonacci.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef MetaFibonacci_h
#define MetaFibonacci_h

// Fibonacci numbers:
// 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, ...
// Fn = Fn-1 + Fn-2
// F0 = 0, F1 = 1

namespace andrivet { namespace ADVobfuscator {

template<int N>
struct Fibonacci { static constexpr int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value; };

template<>
struct Fibonacci<1> { static constexpr int value = 1; };

template<>
struct Fibonacci<0> { static constexpr int value = 0; };
    
}}

#endif
