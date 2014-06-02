//
//  Log.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef Log_h
#define Log_h

#if defined(DEBUG) && DEBUG == 1
#define LOG(str) std::cerr << str << "\n"
#else
#define LOG(str) ((void)0)
#endif

#endif
