//
//  DetectDebugger.h
//  ADVobfuscator
//
//  Created by sebastien on 22/09/14.
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef ADVobfuscator_DetectDebugger_h
#define ADVobfuscator_DetectDebugger_h

#ifdef __APPLE__

bool AmIBeingDebugged();

#else

// Only Apple platforms are implemented in this example. Other platforms are simulated (always debugged)

bool AmIBeingDebugged()
{
    return true;
}

#endif


#endif
