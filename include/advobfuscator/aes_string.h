// ADVobfuscator - Comnpile-time strings encrypted with AES-CTR
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

#ifndef ADVOBFUSCATOR_AES_STRING_H
#define ADVOBFUSCATOR_AES_STRING_H

#include <cstddef>
#include <algorithm>
#include "aes.h"
#include "call.h"

namespace andrivet::advobfuscator {

  /// A compile-time string encrypted with AES-CTR.
  template<std::size_t N>
  struct AesString {
    /// Construct a compile-time string encrypted with AES-CTR.
    /// \param str Array of characters to be encrypted at compile-time.
    /// \remark A key and a nonce are generated on the fly.
    consteval AesString(const char (&str)[N]) noexcept
    : key_{generate_random_block<16>(generate_sum(str, 0))},
      nonce_{generate_random_block<8>(generate_sum(str, 16))} {
      // Compile-time copy of the data
      std::copy(str, str + N, data_.begin());
      // Compile-time encryption
      auto encrypted = encrypt_ctr(data_, key_, nonce_);
      // Compile-time copy of the encrypted data
      std::copy(encrypted.begin(), encrypted.end(), data_.begin());
    }

    /// Destruct the string by first erasing its content.
    /// \remark The erasing may be omitted by the compiler.
    constexpr ~AesString() noexcept { erase(); }

    /// Implicit conversion to a pointer to (const) characters, like a regular string.
    operator const char *() noexcept {
      constexpr auto random = call::generate_random(__LINE__);
      const ObfuscatedMethodCall call{random, &AesString::decrypt_inplace};
      call(random, this);
      return reinterpret_cast<const char *>(data_.data());
    }

    /// Decrypt the encrypted string.
    [[nodiscard]] constexpr std::string decrypt() const {
      std::array<std::uint8_t, N> buffer;
      std::copy(data_.begin(), data_.end(), buffer.begin());
      if(encrypted_) decrypt_ctr(buffer.begin(), N, key_, nonce_);
      std::string str;
      str.resize(N - 1);
      std::copy(buffer.begin(), buffer.end() - 1, str.begin());
      return str;
    }

    /// Get the raw (encrypted) content.
    [[nodiscard]] const char *raw() const noexcept { return data_.data(); }

    /// Get the actual length of the string.
    [[nodiscard]] constexpr std::size_t size() noexcept { return N - 1; }

    /// Encrypted or decrypted data.
    std::array<Byte, N> data_{};
    /// Is the data encrypted (default) or decrypted (i.e. used)?
    bool encrypted_ = true;
    /// The nonce used to chain blocks (CTR).
    Nonce nonce_{};
    /// The key used to encrypt the data.
    Key key_{};

  private:
    /// Erase the information stored by the string (data, key and nonce)
    constexpr void erase() noexcept {
      if (encrypted_) return;
      std::fill(data_.begin(), data_.end(), 0);
      std::fill(key_.begin(), key_.end(), 0);
      std::fill(nonce_.begin(), nonce_.end(), 0);
    }

    /// Run-time decryption
    void decrypt_inplace() noexcept {
      if(!encrypted_) return;
      decrypt_ctr(reinterpret_cast<Byte*>(data_.data()), N, key_, nonce_);
      encrypted_ = false;
    }
  };

  /// User-defined literal "_aes"
  template<AesString str>
  consteval auto operator""_aes() { return str; }
}

#endif
