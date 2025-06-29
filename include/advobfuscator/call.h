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

#ifndef ADVOBFUSCATOR_CALL_H
#define ADVOBFUSCATOR_CALL_H

#include "fsm.h"

namespace andrivet::advobfuscator {

  template<typename F>
  struct ObfuscatedCall {
    consteval ObfuscatedCall(std::uint32_t recognize, F fn)
    : fsm_{recognize, fn} {
    }

    template<typename... Args>
    decltype(auto) operator()(std::uint32_t value, Args... args) const {
      auto fn = fsm_.run(value);
      if constexpr (std::is_void_v<decltype(std::invoke(fn, args...))>) {
        std::invoke(fn, args...);
        return;
      }
      else
        return std::invoke(fn, args...);
    }

    Fsm<F> fsm_;
  };

  template<typename F>
  struct ObfuscatedMethodCall {
    consteval ObfuscatedMethodCall(std::uint32_t recognize, F fn)
    : fsm_{recognize, fn} {
    }

    template<typename O, typename... Args>
    decltype(auto) operator()(std::uint32_t value, O o, Args... args) const {
      auto fn = fsm_.run(value);
      if constexpr (std::is_void_v<decltype(std::invoke(fn, o, args...))>) {
        std::invoke(fn, o, args...);
        return;
      }
      else
        return std::invoke(fn, args...);
    }

    Fsm<F> fsm_;
  };

}

#endif
