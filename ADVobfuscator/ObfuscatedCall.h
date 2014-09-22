//
//  ObfuscatedCall.h
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
//
// Get latest version on https://github.com/andrivet/ADVobfuscator

#ifndef ObfuscatedCall_h
#define ObfuscatedCall_h

#include "MetaFSM.h"

// Obfuscate function call with a finite state machine (FSM).
// This is only a (simplified) example to show the principle
// In this example, the target is called at the end of the FSM so it can be located.
// In production, it would be better to put it in the middle of the FSM with some computing triggering it.

namespace andrivet { namespace ADVobfuscator {

    using namespace ADVfsm;
  
    // Events
    namespace
    {
        struct event1 {};
        struct event2 {};
        struct event3 {};
        struct event4 {};
        struct event5 {};
    }

    // Our Finite State Machine
    template<typename E, typename R = Void>
    struct Machine : public msm::front::state_machine_def<Machine<E, R>>
    {
        // States
        struct State1 : public msm::front::state<>{};
        struct State2 : public msm::front::state<>{};
        struct State3 : public msm::front::state<>{};
        struct State4 : public msm::front::state<>{};
        struct State5 : public msm::front::state<>{};
        struct Final  : public msm::front::state<>{};
        
        // Transition action. It will call our target
        struct State5ToFinal
        {
            template<typename EVT, typename FSM, typename SRC, typename TGT>
            void operator()(EVT const& evt, FSM& fsm, SRC& src, TGT& tgt)
            {
                LOG("State5ToFinal called");
                fsm.result_ = evt.call();
            }
        };
        
        // Initial state of the FSM. Must be defined
        using initial_state = State1;
        
        // Transition table
        struct transition_table : mpl::vector<
            //    Start     Event         Next      Action               Guard
            //  +---------+-------------+---------+---------------------+----------------------+
            Row < State1  , event5      , State2                                               >,
            Row < State1  , event1      , State3                                               >,
            //  +---------+-------------+---------+---------------------+----------------------+
            Row < State2  , event2      , State4                                               >,
            //  +---------+-------------+---------+---------------------+----------------------+
            Row < State3  , none        , State3                                               >,
            //  +---------+-------------+---------+---------------------+----------------------+
            Row < State4  , event4      , State1                                               >,
            Row < State4  , event3      , State5                                               >,
            //  +---------+-------------+---------+---------------------+----------------------+
            Row < State5  , E           , Final,    State5ToFinal                              >
            //  +---------+-------------+---------+---------------------+----------------------+
        > {};
        
        // Result of the target
        R result_;
    };

    // It is not possible to use a template function as a template argument so use a metafunction class
    template<typename M, typename E, typename F, typename... Args>
    struct RunMachine
    {
        static inline void apply(M& machine, F f, Args&&... args)
        {
            // This is just an example of what is possible. In actual production code it would be better to call event E in the middle of this loop and to make transitions more complex.
            
            machine.start();
            
            // Generate a lot of transitions (at least 55, at most 98)
            Unroller<55 + MetaRandom<__COUNTER__, 44>::value>{}([&]()
            {
                machine.process_event(event5{});
                machine.process_event(event2{});
                machine.process_event(event4{});
            });
            
            machine.process_event(event5{});
            machine.process_event(event2{});
            machine.process_event(event3{});
            // This will call our target. In actual production code it would be better to call event E in the middle of the FSM processing.
            machine.process_event(E{f, args...});
        }
    };

}}

// Warning: ##__VA_ARGS__ is not portable (only __VA_ARGS__ is). However, ##__VA_ARGS__ is far better (handles cases when it is empty)

#define OBFUSCATED_CALL(f, ...) ObfuscatedCall<andrivet::ADVobfuscator::Machine, andrivet::ADVobfuscator::RunMachine>(MakeObfuscatedAddress(f, andrivet::ADVobfuscator::MetaRandom<__COUNTER__, 400>::value + 278), ##__VA_ARGS__)
#define OBFUSCATED_CALL_RET(t, f, ...) ObfuscatedCallRet<andrivet::ADVobfuscator::Machine, andrivet::ADVobfuscator::RunMachine, t>(MakeObfuscatedAddress(f, andrivet::ADVobfuscator::MetaRandom<__COUNTER__, 400>::value + 278), ##__VA_ARGS__)


#endif
