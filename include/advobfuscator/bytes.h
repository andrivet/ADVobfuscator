// ADVobfuscator - Obfuscation of a block of bytes
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

#ifndef ADVOBFUSCATOR_BYTES_H
#define ADVOBFUSCATOR_BYTES_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <array>
#include <vector>
#include "obf.h"

namespace andrivet::advobfuscator {

  /// A block of obfuscated bytes
  template<std::size_t N>
  struct ObfuscatedBytes {
    /// Construct a compile-time block of bytes from a string.
    /// \lparam str Array of characters representing bytes to be encrypted at compile-time.
    /// The format of the string is: two hexadecimal digits seperated by spaces.
    /// Example: "01 02 03 1F".
    /// \remark A set of obfuscation algorithms are generated on the fly.
    consteval ObfuscatedBytes(const char (&str)[N])
    : algos_{generate_sum(str)} {
      parse(str);
      encode();
    }

    /// Destruct the block by first erasing its content.
    /// \remark The erasing may be omitted by the compiler.
    constexpr ~ObfuscatedBytes() noexcept { erase(); }

    /// Get the decoded (deobfuscated) bytes
    [[nodiscard]] const std::uint8_t *data() noexcept { decode(); return data_.data(); }

    /// Get the raw (obfuscated) content.
    [[nodiscard]] const std::uint8_t *raw() const noexcept { return data_.data(); }

    /// Get the actual size of the block of bytes.
    [[nodiscard]] std::size_t size() const noexcept { return N / 3; }

    /// Direct access to a byte in the block.
    /// \lparam pos Position of the byte in the block.
    /// \return The decoded byte.
    [[nodiscard]] constexpr std::uint8_t operator[](std::size_t pos) const {
      std::uint8_t b{data_[pos]};
      algos_.decode(pos, &b, &b + 1);
      return b;
    }

    /// Decode (deobfuscate) the block of bytes.
    /// \return The decoded bytes.
    [[nodiscard]] constexpr std::array<std::uint8_t, N / 3> decode() const noexcept {
      std::array<uint8_t, N / 3> buffer{};
      std::copy(data_.begin(), data_.end(), buffer.begin());
      algos_.decode(0, buffer.begin(), buffer.end());
      return buffer;
    }

    /// Obfuscated or decoded data.
    std::array<std::uint8_t, N / 3> data_{};
    /// Set of algorithms used for the obfuscation.
    Obfuscations algos_;
    /// Is the data obfuscated (default) or decoded (i.e. used)?
    bool obfuscated_ = true;

  private:
    /// Convert an hexadecimal digit to its value.
    static consteval std::uint8_t hex_char_value(char c) {
      if('0' <= c && c <= '9') return c - '0';
      if('a' <= c && c <= 'f') return c - 'a' + 10;
      if('A' <= c && c <= 'F') return c - 'A' + 10;
      throw std::invalid_argument("Invalid hex character");
    }

    /// Parse a string representing bytes in hexadecimal separated by spaces.
    consteval void parse(const char (&str)[N]) {
      size_t byte_index = 0;
      uint8_t high = 0;
      bool half = false;

      // For each character (except the terminal null byte)...
      for(size_t i = 0; i < N - 1; ++i) {
        char c = str[i];
        // Is it a space?
        if(c == ' ') continue;
        // Get the corresponding nibble value
        uint8_t value = hex_char_value(c);
        // First nibble (half)?
        if(!half) {
          high = value << 4;
          half = true;
        }
        else {
          // We have the two nibbles, store them
          data_[byte_index++] = high | value;
          half = false;
        }
      }
    }

    /// Erase the information stored by the block (data)
    constexpr void erase() noexcept {
      std::fill(data_.begin(), data_.end(), 0);
    }

    /// Encode (obfuscate) the block of data.
    consteval void encode() noexcept {
      algos_.encode(0, data_.begin(), data_.end());
    }

    /// Decode (deobfuscate) the block of data.
    void decode() noexcept {
      if(!obfuscated_) return;
      algos_.decode(0, data_.begin(), data_.end());
      obfuscated_ = false;
    }
  };

  /// User-defined literal "_obf_bytes"
  template<ObfuscatedBytes block>
  consteval auto operator""_obf_bytes() { return block; }

}

#endif
