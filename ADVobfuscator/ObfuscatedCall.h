//
//  ObfuscatedCall.h
//  ADVobfuscator
//
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

#ifndef ObfuscatedCall_h
#define ObfuscatedCall_h

#include <iostream>
#include <tuple>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>

#include "Indexes.h"
#include "Log.h"

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace boost::msm::front;

namespace andrivet { namespace ADVobfuscator {

    struct Void {};

    template<typename R, typename F, typename... T>
    struct event
    {
        event(F f, T... t): f_{f}, data_{t...} {}

        R call() const
        {
            using I = typename Make_Indexes<sizeof...(T)>::type;
            return call_(I{});
        }

    private:
        template<int... I>
        R call_(Indexes<I...>) const { return f_(std::get<I>(data_)...); }

    private:
        F f_;
        std::tuple<T...> data_;
    };

    template<typename F, typename... T>
    struct eventv
    {
        eventv(F f, T... t): f_{f}, data_{t...} {}
        
        Void call() const
        {
            using I = typename Make_Indexes<sizeof...(T)>::type;
            return call_(I{});
        }
        
    private:
        template<int... I>
        Void call_(Indexes<I...>) const { f_(std::get<I>(data_)...); return Void{}; }
        
    private:
        F f_;
        std::tuple<T...> data_;
    };
    
    namespace
    {
        struct event1 {};
        struct event2 {};
        struct event3 {};
        struct event4 {};
        struct event5 {};
    }

    template<typename E, typename R = Void>
    struct Machine : public msm::front::state_machine_def<Machine<E, R>>
    {
        struct State1 : public msm::front::state<>{};
        struct State2 : public msm::front::state<>{};
        struct State3 : public msm::front::state<>{};
        struct State4 : public msm::front::state<>{};
        struct State5 : public msm::front::state<>{};
        struct Final  : public msm::front::state<>{};
        
        // transition actions
        struct State5ToFinal
        {
            template<typename EVT, typename FSM, typename SRC, typename TGT>
            void operator()(EVT const& evt, FSM& fsm, SRC& src, TGT& tgt)
            {
                LOG("State5ToFinal called");
                fsm.result_ = evt.call();
            }
        };
        
        // the initial state of the player SM. Must be defined
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
        
        R result_;
    };

    namespace
    {
        template<typename M, typename E, typename F, typename... T>
        void ALWAYS_INLINE ProcessEvents(M machine, F f, T... t)
        {
            machine.start();
            for(int i = 0; i < 55 + MetaRandom<__COUNTER__, 44>::value; ++i)
            {
                machine.process_event(event5{});
                machine.process_event(event2{});
                machine.process_event(event4{});
            }
            machine.process_event(event5{});
            machine.process_event(event2{});
            machine.process_event(event3{});
            machine.process_event(E{f, t...});
        };
    }
        
    template<typename R, typename F, typename... T>
    R ALWAYS_INLINE ObfuscatedCall(F f, T... t)
    {
        using E = event<R, F, T...>;
        using M = msm::back::state_machine<Machine<E, R>>;
        
        M machine;
        ProcessEvents<M, E>(machine, f, t...);
        return machine.result_;
    };

    template<typename F, typename... T>
    void ALWAYS_INLINE ObfuscatedCall(F f, T... t)
    {
        using E = eventv<F, T...>;
        using M = msm::back::state_machine<Machine<E>>;
        
        M machine;
        ProcessEvents<M, E>(machine, f, t...);
    };

}}


#endif
