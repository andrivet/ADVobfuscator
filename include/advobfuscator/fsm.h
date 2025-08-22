// ADVobfuscator - Finite state machine generated at compile time
//
// Copyright (c) 2025, Sebastien Andrivet
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
// following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
// disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
// following disclaimer in the documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
// products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Get latest version on https://github.com/andrivet/ADVobfuscator

#ifndef ADVOBFUSCATOR_FSM_H
#define ADVOBFUSCATOR_FSM_H

#include <cstdint>
#include <tuple>
#include <utility>

namespace andrivet::advobfuscator {

  constexpr size_t NB_BITS = 32; //< Number of bits recognized.
  constexpr size_t TRANSITIONS_PER_BIT = 8; //< Number of transitions per bit.
  constexpr size_t MAX_TRANSITIONS = NB_BITS * TRANSITIONS_PER_BIT; //< Total number of transitions.

  // For each bit to recognize, we create a small FSM of 4 states and 8 transitions.
  // - The first transition moves the recognizer to the next state.
  // - The 3 other states are in an infinite loop.

  /// A transition between two states
  namespace details {
    template<typename O>
    struct Transition {
      bool input;  ///< Input value (bit: 0 or 1).
      int from; ///< From this state.
      int to; ///< To this state.
      O o; ///< Object to return.
    };

    /// Compute the number of bits to represent a value.
    constexpr int num_bits(auto value) {
      int num = 0;
      for(auto v = value; v > 0; v >>= 1) ++num;
      if(num == 0) num = 1; // edge case for 0
      return num;
    }
  }

  namespace call {
    /// Generate a random number for the recognizer.
    consteval std::uint32_t generate_random(std::size_t count) {
      return andrivet::advobfuscator::generate_random(count, UINT32_MAX / 10, UINT32_MAX);
    };
  }

  /// A finite state machine that recognize a number bit per bit,
  template<typename O>
  struct Fsm {
    /// Construct a new finite state machine that recognize a number and store an object.
    /// \param recognize The number to be recognized by this finite state machine.
    /// \param o The object stored in one of the transition (the active one).
    consteval Fsm(std::uint32_t recognize, O o) {
      // Get a random number for the activate transition of the recognizer.
      // The activate transition is the transition that stores the object.
      const std::uint32_t activate = generate_random_not_0<uint32_t>(recognize % 1000, NB_BITS - 1);
      auto bits = details::num_bits(recognize);

      // For each bit...
      for(int i = 0; i < bits; ++i) {
        // Get the bit's value
        bool bit = (recognize >> (bits - 1 - i)) & 0x01;
        // Transition to the next state of the recognizer and store (or not) the object
        add_transition(bit, 4 * i, 4 * i + 4, i + 1 == activate ? o : O{});
        // Transition to states in an infinite loop
        add_transition(!bit, 4 * i, 4 * i + 1, O{});
        add_transition(0, 4 * i + 1, 4 * i + 2, O{});
        add_transition(1, 4 * i + 1, 4 * i + 3, O{});
        add_transition(0, 4 * i + 2, 4 * i + 3, O{});
        add_transition(1, 4 * i + 2, 4 * i + 1, O{});
        add_transition(0, 4 * i + 3, 4 * i + 1, O{});
        add_transition(0, 4 * i + 3, 4 * i + 2, O{});
      }
    }

    /// Add a transition to the finite state machine.
    /// \param input Input value.
    /// \param from From state.
    /// \param to To state.
    /// \param o Object to be stored in transition.
    consteval void add_transition(bool input, int from, int to, O o) {
      if(nb_transition_ >= MAX_TRANSITIONS) throw std::exception(); // MAX_TRANSITIONS is too small
      transitions_[nb_transition_++] = {.input = input, .from = from, .to = to, .o = o};
    }

    /// Find a transition from a state and with an input value.
    /// \param state The from state of the transition.
    /// \param input The input value.
    /// \return The transition found.
    /// \exception The FSM is supposed to cover all the cases and this member function will always find
    /// a transition. If it is not the case, there is a bug in the generation of the FSM.
    /// In this case, an exception is raised.
    [[nodiscard]] constexpr const details::Transition<O> &find(int state, bool input) const {
      for(int i = 0; i < nb_transition_; ++i) {
        if(transitions_[i].from == state && transitions_[i].input == input)
          return transitions_[i];
      }
      throw std::exception(); // Missing transition in the FSM (i.e. a bug)
    }

    /// Run the finite state machine on a number.
    /// \param value The value to recognize.
    /// \remark The FSM will never return (infinite loop) if the number is wrong.
    /// This is by design to annoy reverse-engineering.
    decltype(auto) run(std::uint32_t value) const {
      auto bits = details::num_bits(value);
      int state = 0;

      // For each bit in reverse...
      for(int i = bits - 1; i >= 0; --i) {
        // Get the value of the bit.
        bool bit = (value >> i) & 1;
        // Find the transition
        auto &transition = find(state, bit);
        // Update the state.
        state = transition.to;
        // Treat the active transition as a final state.
        if(transition.o != O{}) return transition.o;
      }
      throw std::exception(); // Invalid FSM (i.e.bug);
    }

    /// Transitions of the finite state machine
    std::array<details::Transition<O>, MAX_TRANSITIONS> transitions_{};
    /// Number of transitions
    std::size_t nb_transition_{};
  };
}

#endif
