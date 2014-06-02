//
//  Inline.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef Inline_h
#define Inline_h

#if __has_attribute(always_inline)
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

#endif
