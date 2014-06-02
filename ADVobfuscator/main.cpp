//
//  main.cpp
//  ADVobfusctor
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#include <iostream>
#include "MetaFactorial.h"
#include "MetaFibonacci.h"
#include "MetaRandom.h"
#include "MetaString1.h"
#include "MetaString2.h"
#include "MetaString3.h"
#include "MetaString4.h"
#include "ObfuscatedCall.h"

using namespace std;
using namespace andrivet::ADVobfuscator;

void SampleFactorial()
{
    cout << Factorial<5>::value << endl;
}

void SampleFibonacci()
{
    cout << Fibonacci<8>::value << endl;
    cout << Fibonacci<20>::value << endl;
}

void SampleEncrypted1()
{
    cout << "Britney Spears"_obfuscated1 << endl;
    cout << "Miley Cyrus"_obfuscated1 << endl;
    cout << OBFUSCATED1("Katy Perry") << endl;
}

void SampleEncryped2()
{
    cout << "Britney Spears"_obfuscated2 << endl;
    cout << "Miley Cyrus"_obfuscated2 << endl;
    cout << OBFUSCATED2("Katy Perry") << endl;
}


void SampleMetaRandom()
{
    // Do not use a loop, it will not work (it will print 10 tims the same number):
    // loops are executed at run-time,
    // we want to generate 10 different numbers are compile-time.
    cout << MetaRandom<__COUNTER__, 10>::value << endl;
    cout << MetaRandom<__COUNTER__, 10>::value << endl;
    cout << MetaRandom<__COUNTER__, 10>::value << endl;
    cout << MetaRandom<__COUNTER__, 10>::value << endl;
    cout << MetaRandom<__COUNTER__, 10>::value << endl;
    cout << MetaRandom<__COUNTER__, 10>::value << endl;
    cout << MetaRandom<__COUNTER__, 10>::value << endl;
    cout << MetaRandom<__COUNTER__, 10>::value << endl;
    cout << MetaRandom<__COUNTER__, 10>::value << endl;
    cout << MetaRandom<__COUNTER__, 10>::value << endl;
}

void SampleEncryped3()
{
    cout << OBFUSCATED3("Britney Spears")<< endl;
    cout << OBFUSCATED3("Miley Cyrus") << endl;
    cout << OBFUSCATED3("Katy Perry") << endl;
}

void SampleEncryped4()
{
    cout << OBFUSCATED4("Britney Spears") << endl;
    cout << OBFUSCATED4("Britney Spears") << endl;
    cout << OBFUSCATED4("Britney Spears") << endl;
    cout << OBFUSCATED4("Britney Spears") << endl;
}

void SampleEncryped4_differed()
{
    auto miley   = DEF_OBFUSCATED4("Miley Cyrus");
    auto britney = DEF_OBFUSCATED4("Britney Spears");
    auto katy    = DEF_OBFUSCATED4("Katy Perry");
    
    cout << britney.decrypt() << endl;
    cout << katy.decrypt()    << endl;
    cout << miley.decrypt()   << endl;
}

void SampleEncryped4_simple()
{
    OBFUSCATED4("Britney Spears");
    OBFUSCATED4("Britney Spears");
    OBFUSCATED4("Britney Spears");
    OBFUSCATED4("Britney Spears");
}

void SampleFiniteStateMachine_function_to_protect()
{
    cout << OBFUSCATED4("Womenizer") << endl;
};

int SampleFiniteStateMachine_function_to_protect_with_parameter(const char* text)
{
    cout << OBFUSCATED4("Oops I did it ") << text << endl;
    return 12345;
};

void SampleFiniteStateMachine()
{
    ObfuscatedCall(SampleFiniteStateMachine_function_to_protect);
    
    int result = ObfuscatedCall<int>(SampleFiniteStateMachine_function_to_protect_with_parameter, OBFUSCATED4("again"));
    cout << "Result: " << result << endl;
}

int main(int argc, const char * argv[])
{
    //SampleEncryped4_simple();
    //SampleFibonacci();
    //SampleFibonacci();
    SampleFiniteStateMachine();
    //SampleEncryped4_differed();
    return 0;
}

