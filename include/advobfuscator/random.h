// ADVobfuscator
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

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>

#ifndef ADVOBFUSCATOR_RANDOM_H
#define ADVOBFUSCATOR_RANDOM_H

namespace andrivet::advobfuscator {

  namespace details {

    static constexpr char time[] = __TIME__;
    static constexpr char date[] = __DATE__;

    constexpr int digit_to_int(char c) { return c - '0'; }

    static constexpr unsigned time_seed =
      digit_to_int(time[7]) +
      digit_to_int(time[6]) * 10 +
      digit_to_int(time[4]) * 60 +
      digit_to_int(time[3]) * 600 +
      digit_to_int(time[1]) * 3600 +
      digit_to_int(time[0]) * 36000;

    constexpr int month_to_int(const char* m) {
      return (m[0] == 'J' && m[1] == 'a') ? 1 :
             (m[0] == 'F') ? 2 :
             (m[0] == 'M' && m[2] == 'r') ? 3 :
             (m[0] == 'A' && m[1] == 'p') ? 4 :
             (m[0] == 'M' && m[2] == 'y') ? 5 :
             (m[0] == 'J' && m[2] == 'n') ? 6 :
             (m[0] == 'J' && m[2] == 'l') ? 7 :
             (m[0] == 'A' && m[1] == 'u') ? 8 :
             (m[0] == 'S') ? 9 :
             (m[0] == 'O') ? 10 :
             (m[0] == 'N') ? 11 : 12;
    }

    static constexpr unsigned date_seed =
      (date[4] == ' ' ? digit_to_int(date[5]) : digit_to_int(date[4]) * 10 + digit_to_int(date[5])) +
      month_to_int(date) * 31 +
      (digit_to_int(date[10]) + digit_to_int(date[9]) * 10 + digit_to_int(date[8]) * 100 + digit_to_int(date[7]) * 1000) * 372;

    static constexpr uint64_t base_seed =
      (static_cast<uint64_t>(time_seed) << 32) | static_cast<uint64_t>(date_seed);

    consteval uint64_t splitmix64(uint64_t x) {
      x ^= x >> 30;
      x *= 0xbf58476d1ce4e5b9ULL;
      x ^= x >> 27;
      x *= 0x94d049bb133111ebULL;
      x ^= x >> 31;
      return x;
    }

    struct SFC64 {
      uint64_t a = 0;
      uint64_t b = 0;
      uint64_t c = 0;
      uint64_t w = 0;

      consteval void seed(uint64_t s) {
        a = splitmix64(s);
        b = splitmix64(a);
        c = splitmix64(b);
        w = 1;
        for(int i = 0; i < 12; ++i) next();
      }

      consteval uint64_t next() {
        uint64_t tmp = a + b + w++;
        a = b ^ (b >> 11);
        b = c + (c << 3);
        c = std::rotl(c, 24) + tmp;
        return tmp;
      }
    };

    template<typename T>
    consteval T generate_random(std::size_t count, T max) {
      const uint64_t seed = base_seed ^ splitmix64(static_cast<uint64_t>(count));
      SFC64 rng;
      rng.seed(seed);
      const uint64_t value = rng.next();
      return static_cast<T>(value % static_cast<uint64_t>(max));
    }
  }

  template<typename T = std::size_t>
  consteval T generate_random_not_0(std::size_t count, T max) {
    return static_cast<T>(details::generate_random(count, static_cast<uint64_t>(max) - 1) + 1);
  }

  template<typename T = std::size_t>
  consteval T generate_random(std::size_t count, T min, T max) {
    return static_cast<T>(details::generate_random(count, static_cast<uint64_t>(max) - static_cast<uint64_t>(min)) + static_cast<uint64_t>(min));
  }

  template<typename T = std::size_t>
  consteval T generate_random(std::size_t count, T max) {
    return static_cast<T>(details::generate_random(count, static_cast<uint64_t>(max)));
  }

  template<std::size_t N>
  consteval std::array<std::uint8_t, N> generate_random_block(std::size_t count) {
    std::array<std::uint8_t, N> block{};
    for(std::size_t i = 0; i < N; ++i)
      block[i] = static_cast<std::uint8_t>(details::generate_random(count + i, static_cast<uint64_t>(256)));
    return block;
  }

  template<unsigned N>
  consteval std::size_t generate_sum(char const (&str)[N], std::size_t initial = 0) {
    std::size_t sum = initial;
    for(std::size_t i = 0; i < N; ++i) sum = (sum + static_cast<unsigned char>(str[i])) % 1000;
    return sum;
  }

}

#endif
