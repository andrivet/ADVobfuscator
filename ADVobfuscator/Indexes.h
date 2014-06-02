//
//  Indexes.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef Indexes_h
#define Indexes_h

// std::index_sequence will be available with C++14 (C++1y). For the moment, implement a (very) simplified and partial version. You can find more complete versions on the Internet

namespace andrivet { namespace ADVobfuscator {

template<int... I>
struct Indexes { using type = Indexes<I..., sizeof...(I)>; };

template<int N>
struct Make_Indexes { using type = typename Make_Indexes<N-1>::type::type; };

template<>
struct Make_Indexes<0> { using type = Indexes<>; };

}}

#endif
