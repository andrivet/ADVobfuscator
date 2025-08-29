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

#include <cstdint>

#ifndef ADVOBFUSCATOR_RANDOM_H
#define ADVOBFUSCATOR_RANDOM_H

namespace andrivet::advobfuscator {

  namespace details {

    /// Use current (compile time) as a seed
    static constexpr char time[] = __TIME__; // __TIME__ has the following format: hh:mm:ss in 24-hour time

    /// Convert a digit into the corresponding number
    constexpr int digit_to_int(char c) { return c - '0'; }

    /// Convert time string (hh:mm:ss) into a number
    static constexpr unsigned seed =
      digit_to_int(time[7]) +
      digit_to_int(time[6]) * 10 +
      digit_to_int(time[4]) * 60 +
      digit_to_int(time[3]) * 600 +
      digit_to_int(time[1]) * 3600 +
      digit_to_int(time[0]) * 36000;

    /// Generate a (pseudo) random number.
    /// \tparam T Type of the number to generate (std::size_t by default).
    /// \param count The count for the generation of random numbers.
    /// \param max The maximum value of the number generated (excluded).
    /// \return A number generated randomly.
    /// \remarks Inspired by 1988, Stephen Park and Keith Miller
    /// "Random Number Generators: Good Ones Are Hard To Find", considered as "minimal standard"
    /// Park-Miller 31 bit pseudo-random number generator, implemented with G. Carta's optimisation:
    /// with 32-bit math and without division
    template<typename T = std::size_t>
    consteval T generate_random(std::size_t count, T max) {
      const uint32_t a = 16807;        // 7^5
      const uint32_t m = 2147483647;   // 2^31 - 1

      auto s = seed;
      while(count-- > 0) {
        uint32_t lo = a * (s & 0xFFFF);                // Multiply lower 16 bits by 16807
        uint32_t hi = a * (s >> 16);                   // Multiply higher 16 bits by 16807
        uint32_t lo2 = lo + ((hi & 0x7FFF) << 16);     // Combine lower 15 bits of hi with lo's upper bits
        uint32_t lo3 = lo2 + hi;
        s = lo3 > m ? lo3 - m : lo3;
      }

      // Note: A bias is introduced by the modulo operation.
      // However, I do believe it is negligible in this case (M is far lower than 2^31 - 1)
      return static_cast<T>(s % static_cast<uint32_t>(max));
    }
  }

  /// Generate a (pseudo) random number strictly greater than 0.
  /// \tparam T Type of the number to generate (std::size_t by default).
  /// \param count Randomization counter.
  /// \param max The maximum value of the number generated (excluded).
  template<typename T = std::size_t>
  consteval T generate_random_not_0(std::size_t count, T max) {
    return static_cast<T>(details::generate_random(count, static_cast<uint32_t>(max) - 1) + 1);
  }

  /// Generate a (pseudo) random number in a range.
  /// \tparam T Type of the number to generate (std::size_t by default).
  /// \param count Randomization counter.
  /// \param max The maximum value of the number generated (excluded).
  template<typename T = std::size_t>
  consteval T generate_random(std::size_t count, T min, T max) {
    return static_cast<T>(details::generate_random(count, static_cast<uint32_t>(max) - static_cast<uint32_t>(min)) + static_cast<unsigned>(min));
  }

  /// Generate a (pseudo) random number in a range (0 included).
  /// \tparam T Type of the number to generate (std::size_t by default).
  /// \param count Randomization counter.
  /// \param max The maximum value of the number generated (excluded).
  template<typename T = std::size_t>
  consteval T generate_random(std::size_t count, T max) {
    return static_cast<T>(details::generate_random(count, static_cast<uint32_t>(max)));
  }

  /// Generate block of (pseudo) random numbers.
  /// \tparam N The size of the block of numbers.
  /// \param count Randomization counter.
  /// \return An array of (pseudo) random numbers.
  template<std::size_t N>
  consteval std::array<std::uint8_t, N> generate_random_block(std::size_t count) {
    std::array<std::uint8_t, N> block;
    for(std::size_t i = 0; i < N; ++i) block[i] = details::generate_random(count + i, 256);
    return block;
  }

  /// Compute the sum of a initial number of of the values of characters.
  /// \tparam N The number of characters.
  /// \param str The string of characters.
  /// \param initial The initial value of the sum (0 by default).
  template<unsigned N>
  consteval std::size_t generate_sum(char const (&str)[N], size_t initial = 0) {
    std::size_t sum = initial;
    for(std::size_t i = 0; i < N; ++i) sum = (sum + str[i]) % 1000;
    return sum;
  }

}

#endif

