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

#ifndef ADVOBFUSCATOR_STRING_H
#define ADVOBFUSCATOR_STRING_H

#include <string>

#include "aes_string.h"
#include "obf.h"
#include "call.h"

namespace andrivet::advobfuscator {

  /// An obfuscated string of characters.
  /// \tparam N The number of bytes of the string (including the null terminal byte).
  template<std::size_t N>
  struct ObfuscatedString {
    /// Construct an obfuscated string of characters.
    /// \param str The array of characters (including the null terminal byte).
    consteval ObfuscatedString(char const (&str)[N]) noexcept
    : algos_{generate_sum(str)} {
      encode(str);
    };

    /// Construct an obfuscated string of characters.
    /// \param str The array of characters (including the null terminal byte).
    /// \param params The parameters for the obfuscation (key and algorithms).
    consteval ObfuscatedString(char const (&str)[N], const Parameters &params) noexcept
    : algos_{params} {
      encode(str);
    }

    /// Construct an obfuscated string of characters.
    /// \param str The array of characters (including the null terminal byte).
    /// \param params An array of parameters for the obfuscations (keys and algorithms).
    template<std::size_t A>
    consteval ObfuscatedString(char const (&str)[N], const Parameters (&params)[A]) noexcept
    : algos_{params} {
      static_assert(A <= details::MAX_NB_ALGORITHMS, "Maximum number of parameters overflow");
      encode(str);
    }

    /// Destruct an obfuscated string by first erasing its content.
    constexpr ~ObfuscatedString() noexcept { erase(); }

    /// Implicit conversion to a pointer to (const) characters, like a regular string.
    operator const char* () noexcept {
      constexpr auto random = call::generate_random(__LINE__);
      const ObfuscatedMethodCall call{random, &ObfuscatedString::decode_inplace};
      call(random, this);
      return data_.data();
    }

    /// Get the raw (encrypted) content.
    [[nodiscard]] const char *raw() const noexcept { return data_.data(); }

    /// Get the actual length of the string.
    [[nodiscard]] constexpr std::size_t size() noexcept { return N - 1; }

    /// Decode the obfuscated string
    [[nodiscard]] constexpr std::string decode() const {
      std::array<std::uint8_t, N> buffer;
      std::copy(data_.begin(), data_.end(), buffer.begin());
      if(obfuscated_) algos_.decode(0, buffer.begin(), buffer.end());
      std::string str;
      str.resize(N - 1);
      std::copy(buffer.begin(), buffer.end() - 1, str.begin());
      return str;
    }

    /// Encoded or decoded data.
    std::array<char, N> data_{};
    /// Obfuscations used to encode the data.
    Obfuscations algos_;
    /// Is the data encoded (default) or decoded (i.e. used)?
    bool obfuscated_ = true;

  private:
    /// Erase the data of the string.
    constexpr void erase() noexcept {
      std::fill(data_.begin(), data_.end(), 0);
    }

    /// Encode an array of characters.
    /// \param str The string of characters to be encoded.
    consteval void encode(char const (&str)[N]) noexcept {
      std::array<std::uint8_t, N> buffer;
      std::copy(str, str + N, buffer.begin());
      algos_.encode(0, buffer.begin(), buffer.end());
      std::copy(buffer.begin(), buffer.end(), data_.begin());
    }

    /// Decode an array of characters in-place.
    void decode_inplace() noexcept {
      if(!obfuscated_) return;
      std::array<std::uint8_t, N> buffer;
      std::copy(data_.begin(), data_.end(), buffer.begin());
      algos_.decode(0, buffer.begin(), buffer.end());
      std::copy(buffer.begin(), buffer.end(), data_.begin());
      obfuscated_ = false;
    }
  };

  // User-defined literal "_obf"
  template<ObfuscatedString str>
  consteval auto operator ""_obf() { return str; }
}

#endif
