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

template<typename E, typename R>
struct Machine : public msm::front::state_machine_def<Machine<E, R>>
{
    struct State1 : public msm::front::state<>{};
    struct State2 : public msm::front::state<>{};
    struct State3 : public msm::front::state<>{};
    struct State4 : public msm::front::state<>{};
    struct State5 : public msm::front::state<>{};
   
    // the initial state of the player SM. Must be defined
    using initial_state = State1;
    
    // transition actions
    struct State4ToState5
    {
        template<typename EVT, typename FSM, typename SRC, typename TGT>
        void operator()(EVT const& evt, FSM& fsm, SRC& src, TGT& tgt)
        {
            std::cout << "my_machine::State4ToState5" << std::endl;
            fsm.result_ = evt.call();
        }
    };
    
    // guard conditions
    struct always_true
    {
        template<typename EVT, typename FSM, typename SRC, typename TGT>
        bool operator()(EVT const& evt, FSM& fsm, SRC& src, TGT& tgt)
        {
            std::cout << "always_true" << std::endl;
            return true;
        }
    };
    
    struct always_false
    {
        template<typename EVT, typename FSM, typename SRC, typename TGT>
        bool operator()(EVT const& evt, FSM& fsm, SRC& src, TGT& tgt)
        {
            std::cout << "always_false" << std::endl;
            return true;
        }
    };
    
    // Transition table
    struct transition_table : mpl::vector<
        //    Start     Event         Next      Action               Guard
        //  +---------+-------------+---------+---------------------+----------------------+
        Row < State1  , none        , State2                                               >,
        Row < State2  , none        , State3                                               >,
        Row < State3  , none        , State4  , none                , always_false         >,
        //  +---------+-------------+---------+---------------------+----------------------+
        Row < State3  , none        , State4  , none                , always_true          >,
        Row < State4  , E           , State5,   State4ToState5                             >
        //  +---------+-------------+---------+---------------------+----------------------+
    > {};
    
    R result_;
};

template<typename R, typename F, typename... T>
R ObfuscatedCall(F f, T... t)
{
    using Event = event<R, F, T...>;
    Event e{f, t...};

    msm::back::state_machine<Machine<Event, R>> machine;
    machine.start();
    machine.process_event(e);
    return machine.result_;
};

template<typename F, typename... T>
void ObfuscatedCall(F f, T... t)
{
    using Event = eventv<F, T...>;
    Event e{f, t...};
    
    msm::back::state_machine<Machine<Event, Void>> machine;
    machine.start();
    machine.process_event(e);
};

}}


#endif
