// ADVobfuscator - A compile-time implementation of AES
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

#ifndef ADVOBFUSCATOR_AES_H
#define ADVOBFUSCATOR_AES_H

// References:
// * https://csrc.nist.gov/csrc/media/projects/cryptographic-standards-and-guidelines/documents/aes-development/rijndael-ammended.pdf#page=19
// * https://csrc.nist.gov/csrc/media/publications/fips/197/final/documents/fips-197.pdf
// * https://cr.yp.to/aes-speed.html
// * https://stackoverflow.com/questions/69066821/rijndael-s-box-in-c
// * https://link.springer.com/content/pdf/10.1007/3-540-36400-5_13.pdf

#include <cstddef>
#include <cstdint>
#include <array>
#include "random.h"
#include "bytes.h"

namespace andrivet::advobfuscator {
  /// Length of the cypher key
  static constexpr std::size_t n_key{128}; // 128-bit or 192-bit or 256-bit

  // I prefer to use std::uin8_t instead of std::byte for the implicit conversions (from int)
  using Byte = std::uint8_t;
  using Block = std::array<Byte, 128 / 8>;
  using Key = std::array<Byte, n_key / 8>;
  using Nonce = std::array<Byte, 8>;

  // ------------------------------------------------------------------
  // Internal details
  // ------------------------------------------------------------------

  namespace details {
    // A 32-bit word
    using Word = std::array<Byte, 4>;
    // An array of 4 columns, each column has 4 rows (s[column][row]).
    using State = std::array<Word, 4>;

    /// Number of rounds
    consteval std::size_t n_rounds() {
      static_assert(n_key == 128 || n_key == 192 || n_key == 256);
      return n_key == 128 ? 10 : n_key == 192 ? 12 : 14;
    }

    // Expanded key
    using EKey = std::array<Word, 4 * (n_rounds() + 1)>;

    // Rijndael S-Box (obfuscated)
    static constexpr ObfuscatedBytes<16 * 3> sbox[16] = {
      "63 7C 77 7B F2 6B 6F C5 30 01 67 2B FE D7 AB 76"_obf_bytes,
      "CA 82 C9 7D FA 59 47 F0 AD D4 A2 AF 9C A4 72 C0"_obf_bytes,
      "B7 FD 93 26 36 3F F7 CC 34 A5 E5 F1 71 D8 31 15"_obf_bytes,
      "04 C7 23 C3 18 96 05 9A 07 12 80 E2 EB 27 B2 75"_obf_bytes,
      "09 83 2C 1A 1B 6E 5A A0 52 3B D6 B3 29 E3 2F 84"_obf_bytes,
      "53 D1 00 ED 20 FC B1 5B 6A CB BE 39 4A 4C 58 CF"_obf_bytes,
      "D0 EF AA FB 43 4D 33 85 45 F9 02 7F 50 3C 9F A8"_obf_bytes,
      "51 A3 40 8F 92 9D 38 F5 BC B6 DA 21 10 FF F3 D2"_obf_bytes,
      "CD 0C 13 EC 5F 97 44 17 C4 A7 7E 3D 64 5D 19 73"_obf_bytes,
      "60 81 4F DC 22 2A 90 88 46 EE B8 14 DE 5E 0B DB"_obf_bytes,
      "E0 32 3A 0A 49 06 24 5C C2 D3 AC 62 91 95 E4 79"_obf_bytes,
      "E7 C8 37 6D 8D D5 4E A9 6C 56 F4 EA 65 7A AE 08"_obf_bytes,
      "BA 78 25 2E 1C A6 B4 C6 E8 DD 74 1F 4B BD 8B 8A"_obf_bytes,
      "70 3E B5 66 48 03 F6 0E 61 35 57 B9 86 C1 1D 9E"_obf_bytes,
      "E1 F8 98 11 69 D9 8E 94 9B 1E 87 E9 CE 55 28 DF"_obf_bytes,
      "8C A1 89 0D BF E6 42 68 41 99 2D 0F B0 54 BB 16"_obf_bytes};

    // Rijndael Inverse S-Box (obfuscated)
    static constexpr ObfuscatedBytes<16 * 3> inv_sbox[16] = {
      "52 09 6a d5 30 36 a5 38 bf 40 a3 9e 81 f3 d7 fb"_obf_bytes,
      "7c e3 39 82 9b 2f ff 87 34 8e 43 44 c4 de e9 cb"_obf_bytes,
      "54 7b 94 32 a6 c2 23 3d ee 4c 95 0b 42 fa c3 4e"_obf_bytes,
      "08 2e a1 66 28 d9 24 b2 76 5b a2 49 6d 8b d1 25"_obf_bytes,
      "72 f8 f6 64 86 68 98 16 d4 a4 5c cc 5d 65 b6 92"_obf_bytes,
      "6c 70 48 50 fd ed b9 da 5e 15 46 57 a7 8d 9d 84"_obf_bytes,
      "90 d8 ab 00 8c bc d3 0a f7 e4 58 05 b8 b3 45 06"_obf_bytes,
      "d0 2c 1e 8f ca 3f 0f 02 c1 af bd 03 01 13 8a 6b"_obf_bytes,
      "3a 91 11 41 4f 67 dc ea 97 f2 cf ce f0 b4 e6 73"_obf_bytes,
      "96 ac 74 22 e7 ad 35 85 e2 f9 37 e8 1c 75 df 6e"_obf_bytes,
      "47 f1 1a 71 1d 29 c5 89 6f b7 62 0e aa 18 be 1b"_obf_bytes,
      "fc 56 3e 4b c6 d2 79 20 9a db c0 fe 78 cd 5a f4"_obf_bytes,
      "1f dd a8 33 88 07 c7 31 b1 12 10 59 27 80 ec 5f"_obf_bytes,
      "60 51 7f a9 19 b5 4a 0d 2d e5 7a 9f 93 c9 9c ef"_obf_bytes,
      "a0 e0 3b 4d ae 2a f5 b0 c8 eb bb 3c 83 53 99 61"_obf_bytes,
      "17 2b 04 7e ba 77 d6 26 e1 69 14 63 55 21 0c 7d"_obf_bytes};

    // Rijndael round constants (obfuscated)
    static constexpr auto rcon = "01 02 04 08 10 20 40 80 1b 36"_obf_bytes;

    /// Get high nibble
    /// \param b Byte to which we want to extract the high nibble.
    /// \return The high nibble
    [[nodiscard]] constexpr Byte high(Byte b) { return b >> 4; }

    /// Get low nibble
    /// \param b Byte to which we want to extract the low nibble.
    /// \return The low nibble
    [[nodiscard]] constexpr Byte low(Byte b) { return b & 0x0F; }

    /// Multiplication in GF(2^8) of two bytes.
    /// \param v0 First argument
    /// \param v1 Second argument
    /// \return Result of the multiplication of v0 and v1 in GF(2^8)
    /// \remark  https://en.wikipedia.org/wiki/Rijndael_MixColumns
    [[nodiscard]] constexpr Byte gmul(Byte v0, Byte v1) {
      Byte product = 0; // Initial product value
      // For each bit...
      for(std::size_t i = 0; i < 8; ++i) {
        // If least significant bit is set, add (xor) v0 to product
        if(v1 & 1) product ^= v0;
        // Set high_bit to the x^7 term of v0
        const bool high_bit = v0 & 0x80;
        // Shift v0 to the left to multiply it by x (v0 = v0 * x)
        v0 <<= 1;
        // Turn x^8 into x^4+x^3+x+1
        if(high_bit) v0 ^= 0x1B;
        // Right shift v1
        v1 >>= 1;
      }
      return product;
    }

    /// SubWord Transformation - non-linear byte substitution using sbox.
    /// \param word Word to transform.
    /// \return Transformed Word.
    [[nodiscard]] constexpr Word sub_word(const Word &word) {
      return Word{
        sbox[high(word[0])][low(word[0])],
        sbox[high(word[1])][low(word[1])],
        sbox[high(word[2])][low(word[2])],
        sbox[high(word[3])][low(word[3])]
      };
    }

    /// SubBytes Transformation - non-linear byte substitution using sbox.
    /// \param state State to transform.
    /// \return Transformed state.
    [[nodiscard]] constexpr State sub_bytes(const State &state) {
      return State{
        sub_word(state[0]),
        sub_word(state[1]),
        sub_word(state[2]),
        sub_word(state[3])};
    }

    /// InvSubWord Transformation -Inverse of SubWord.
    /// \param word Word to transform.
    /// \return Transformed Word.
    [[nodiscard]] constexpr Word inv_sub_word(const Word &word) {
      return Word{
        inv_sbox[high(word[0])][low(word[0])],
        inv_sbox[high(word[1])][low(word[1])],
        inv_sbox[high(word[2])][low(word[2])],
        inv_sbox[high(word[3])][low(word[3])]
      };
    }

    /// InvSubBytes Transformation - Inverse of SubBytes.
    /// \param state State to transform.
    /// \return Transformed state.
    [[nodiscard]] constexpr State inv_sub_bytes(const State &state) {
      return State{
        inv_sub_word(state[0]),
        inv_sub_word(state[1]),
        inv_sub_word(state[2]),
        inv_sub_word(state[3])};
    }

    /// ShiftRows Transformation - bytes in the last three rows are cyclically shifted.
    /// \param state State to transform.
    /// \return Transformed state.
    /// \remark Section 5.1.2
    [[nodiscard]] constexpr State shift_rows(const State &state) {
      return State{
        Word{state[0][0], state[1][1], state[2][2], state[3][3]}, // c0
        Word{state[1][0], state[2][1], state[3][2], state[0][3]}, // c1
        Word{state[2][0], state[3][1], state[0][2], state[1][3]}, // c2
        Word{state[3][0], state[0][1], state[1][2], state[2][3]}  // c3
      };
    }

    /// InvShiftRows Transformation - Inverse of ShiftRows.
    /// \param state State to transform.
    /// \return Transformed state.
    /// \remark Section 5.1.2
    [[nodiscard]] constexpr State inv_shift_rows(const State &state) {
      return State{
        Word{state[0][0], state[3][1], state[2][2], state[1][3]}, // c0
        Word{state[1][0], state[0][1], state[3][2], state[2][3]}, // c1
        Word{state[2][0], state[1][1], state[0][2], state[3][3]}, // c2
        Word{state[3][0], state[2][1], state[1][2], state[0][3]}  // c3
      };
    }

    /// MixColumns Transformation - Multiply a column by a fixed polynomial.
    /// \param c The column to transform.
    /// \return The transformed column.
    /// \remark Section 5.1.3
    [[nodiscard]] constexpr Word mix_column(const Word &c) {
      // 4.2.3 - The MixColumn transformation
      // c(x) = 3 * x^3 + 1 * x^2 + 1 * x + 2 modulo x^4 + 1
      const auto v0{gmul(c[0], 0x02) ^ gmul(c[1], 0x03) ^      c[2]        ^ c[3]};
      const auto v1{     c[0]        ^ gmul(c[1], 0x02) ^ gmul(c[2], 0x03) ^ c[3]};
      const auto v2{     c[0]        ^      c[1]        ^ gmul(c[2], 0x02) ^ gmul(c[3], 0x03)};
      const auto v3{gmul(c[0], 0x03) ^      c[1]        ^      c[2]        ^ gmul(c[3], 0x02)};
      return Word{
        static_cast<Byte>(v0),
        static_cast<Byte>(v1),
        static_cast<Byte>(v2),
        static_cast<Byte>(v3)};
    }

    /// InvMixColumns Transformation - Inverse of MixColumns.
    /// \param c The column to transform.
    /// \return The transformed column.
    /// \remark Section 5.3.3
    [[nodiscard]] constexpr Word inv_mix_column(const Word &c) {
      // 4.2.3 - The MixColumn transformation
      // c(x) = 3 * x^3 + 1 * x^2 + 1 * x + 2 modulo x^4 + 1
      const auto v0{gmul(c[0], 0x0e) ^ gmul(c[1], 0x0b) ^ gmul(c[2], 0x0d) ^ gmul(c[3], 0x09)};
      const auto v1{gmul(c[0], 0x09) ^ gmul(c[1], 0x0e) ^ gmul(c[2], 0x0b) ^ gmul(c[3], 0x0d)};
      const auto v2{gmul(c[0], 0x0d) ^ gmul(c[1], 0x09) ^ gmul(c[2], 0x0e) ^ gmul(c[3], 0x0b)};
      const auto v3{gmul(c[0], 0x0b) ^ gmul(c[1], 0x0d) ^ gmul(c[2], 0x09) ^ gmul(c[3], 0x0e)};
      return Word{
        static_cast<Byte>(v0),
        static_cast<Byte>(v1),
        static_cast<Byte>(v2),
        static_cast<Byte>(v3)};
    }

    /// MixColumns Transformation - Multiply columns by a fixed polynomial.
    /// \param state The state to transform.
    /// \return The transformed state.
    [[nodiscard]] constexpr State mix_columns(const State &state) {
      return State{mix_column(state[0]), mix_column(state[1]), mix_column(state[2]), mix_column(state[3])};
    }

    /// InvMixColumns Transformation - Inverse of MixColumns.
    /// \param state The state to transform.
    /// \return The transformed state.
    [[nodiscard]] constexpr State inv_mix_columns(const State &state) {
      return State{inv_mix_column(state[0]), inv_mix_column(state[1]), inv_mix_column(state[2]), inv_mix_column(state[3])};
    }

    /// AddRoundKey Transformation - Add a Round Key to the State.
    /// \param state The current state.
    /// \param round_key The round key.
    /// \return The transformed state.
    /// \remark Section 5.1.4
    [[nodiscard]] constexpr State add_round_key(const State &state, const EKey &ekey, std::size_t round) {
      State new_state;
      for(std::size_t c = 0; c < 4; ++c)
        for(std::size_t r = 0; r < 4; ++r)
          new_state[c][r] = state[c][r] ^ ekey[round * 4 + c][r];
      return new_state;
    }

    /// RotWord Transformation - Cyclic permutation
    /// \param w The Word to transform.
    /// \returnThe transformed Word.
    [[nodiscard]] constexpr Word rot_word(const Word &w) {
      return Word{w[1], w[2], w[3], w[0]};
    }

    /// Addition (XOR) in GF(2^8) of two words.
    /// \param w0 The left operand.
    /// \param w1 The wight operand.
    /// \return The result of the addition in GF(2^8) of each byte.
    [[nodiscard]] constexpr Word operator^(const Word &w0, const Word &w1) {
      return Word{
        static_cast<Byte>(w0[0] ^ w1[0]),
        static_cast<Byte>(w0[1] ^ w1[1]),
        static_cast<Byte>(w0[2] ^ w1[2]),
        static_cast<Byte>(w0[3] ^ w1[3])
      };
    }

    /// Addition (XOR) in GF(2^8) of a word and a byte.
    /// \param w0 The left operand.
    /// \param w1 The wight operand.
    /// \return The result of the addition in in GF(2^8) of each byte of the left operand with the right operand.
    [[nodiscard]] constexpr Word operator^(const Word &w0, Byte b) {
      return Word{
        static_cast<Byte>(w0[0] ^ b),
        static_cast<Byte>(w0[1] ^ b),
        static_cast<Byte>(w0[2] ^ b),
        static_cast<Byte>(w0[3] ^ b)
      };
    }

    /// Key Expansion - Generate a key schedule.
    /// \param key The key to be expanded.
    /// \return The expanded key.
    /// \remark Section 5.2
    [[nodiscard]] constexpr EKey key_expansion(const Key &key) {
      EKey ekey;
      const auto nk = n_key / 32;

      // First 4 words: copy of the encryption key
      for(std::size_t i = 0; i < nk; ++i)
        ekey[i] = {key[4 * i], key[4 * i + 1], key[4 * i + 2], key[4 * i + 3]};

      const auto n_r = n_rounds();
      for(std::size_t i = nk; i < 4 * (n_r + 1); ++i) {
        Word temp = ekey[i - 1];
        if(i % nk == 0) {
          temp = sub_word(rot_word(temp));
          temp[0] ^= rcon[i / nk - 1];
        }
        else if(nk > 6 and i % nk == 4)
          temp = sub_word(temp);
        ekey[i] = ekey[i - 4] ^ temp;
      }

      return ekey;
    }

    /// Create a State from a Block.
    /// \param block The Block used to create the State.
    /// \return The State created from the Block.
    [[nodiscard]] constexpr State to_state(const Block &block) {
      State state;
      for(std::size_t i = 0; i < 4*4; ++i) state[i / 4][i % 4] = block[i];
      return state;
    }

    /// Create a Block from a State.
    /// \param block The State used to create the Block.
    /// \return The Block created from the State.
    [[nodiscard]] constexpr Block to_block(const State &state) {
      Block block;
      for(std::size_t i = 0; i < 4*4; ++i) block[i] = state[i / 4][i % 4];
      return block;
    }

  }

  // ------------------------------------------------------------------
  // Public functions
  // ------------------------------------------------------------------

  /// Encrypt a block (128-bit) with a key.
  /// \param block Block to be encrypted with AES.
  /// \param key AES key.
  /// \return The encrypted block.
  [[nodiscard]] constexpr Block encrypt(const Block &block, const Key &key) {
    using namespace details;

    const auto ekey = key_expansion(key);
    State state = add_round_key(to_state(block), ekey, 0);
    for(std::size_t round = 1; round < n_rounds(); ++round)
      state = add_round_key(mix_columns(shift_rows(sub_bytes(state))), ekey, round);
    state = add_round_key(shift_rows(sub_bytes(state)), ekey, n_rounds());
    return to_block(state);
  }

  /// Encrypt an array of bytes (128-bit) with a key.
  /// \param block bytes to be encrypted with AES.
  /// \param key AES key.
  /// \return The encrypted block.
  [[nodiscard]] consteval Block encrypt(const Byte (&block)[16], const Byte (&key)[n_key / 8]) {
    return encrypt(std::to_array(block), std::to_array(key));
  }

  /// Decrypt (at runtime) a block of bytes with a key.
  /// \param block bytes to be decrypted with AES.
  /// \param key AES key.
  /// \return The decrypted block.
  [[nodiscard]] inline Block decrypt(const Block &block, const Key &key) {
    using namespace details;
    const auto ekey = key_expansion(key);

    State state = add_round_key(to_state(block), ekey, n_rounds());
    for(std::size_t round = n_rounds() - 1; round >= 1; --round)
      state = inv_mix_columns(add_round_key(inv_sub_bytes(inv_shift_rows(state)), ekey, round));
    state = add_round_key(inv_sub_bytes(inv_shift_rows(state)), ekey, 0);
    return to_block(state);
  }

  /// Encrypt in-place a string with a key using CTR (Counter) code (using a nonce)
  /// \param block bytes to be encrypted with AES. The number of bytes does not need to be a multiple of 128.
  /// \param key AES key.
  /// \param nonce The random nonce to initialize the stream.
  template<std::size_t N>
  [[nodiscard]] consteval std::array<Byte, N> encrypt_ctr(const std::array<Byte, N> &block, const Key &key, const Nonce &nonce) {
    Block ctr{
      nonce[0], nonce[1], nonce[2], nonce[3], nonce[4], nonce[5], nonce[6], nonce[7],
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    std::array<Byte, N> encrypted;
    const auto nb_whole_blocks = N / 16;
    const auto nb_bytes_last_block = N % 16;
    for(std::size_t i = 0; i < nb_whole_blocks; ++i) {
      auto encrypted_ctr = encrypt(ctr, key);
      // Combine the cipher and the plain bytes
      for(std::size_t j = 0; j < 16; ++j) encrypted[i * 16 + j] = block[i * 16 + j] ^ encrypted_ctr[j];
      // Update the counter
      for(std::size_t j = 0; j < 8; ++j) ctr[8 + j] = static_cast<Byte>((i >> j * 8) & 0x00000000000000FF);
    }

    const auto encrypted_ctr = encrypt(ctr, key);
    for(std::size_t j = 0; j < nb_bytes_last_block; ++j)
      encrypted[nb_whole_blocks * 16 + j] = block[nb_whole_blocks * 16 + j] ^ encrypted_ctr[j];

    return encrypted;
  }

  /// Decrypt in-place a string with a key using CTR (Counter) code (using a nonce)
  /// \param block bytes to be decrypted with AES. The number of bytes does not need to be a multiple of 128.
  /// \param key AES key.
  /// \param nonce The random nonce to initialize the stream.
  inline void decrypt_ctr(Byte *data, size_t size, const Key &key, const Nonce &nonce) {
    Block ctr{
        nonce[0], nonce[1], nonce[2], nonce[3], nonce[4], nonce[5], nonce[6], nonce[7],
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    const auto nb_whole_blocks = size / 16;
    const auto nb_bytes_last_block = size % 16;
    for(std::size_t i = 0; i < nb_whole_blocks; ++i) {
      auto encrypted_ctr = encrypt(ctr, key);
      // Combine the cipher and the plain bytes
      for(std::size_t j = 0; j < 16; ++j) data[i * 16 + j] = data[i * 16 + j] ^ encrypted_ctr[j];
      // Update the counter
      for(std::size_t j = 0; j < 8; ++j) ctr[8 + j] = static_cast<Byte>((i >> j * 8) & 0x00000000000000FF);
    }

    const auto encrypted_ctr = encrypt(ctr, key);
    for(std::size_t j = 0; j < nb_bytes_last_block; ++j)
      data[nb_whole_blocks * 16 + j] = data[nb_whole_blocks * 16 + j] ^ encrypted_ctr[j];
  }

  /// Decrypt out-of-place a string with a key using CTR (Counter) code (using a nonce)
  /// \param block bytes to be decrypted with AES. The number of bytes does not need to be a multiple of 128.
  /// \param key AES key.
  /// \param nonce The random nonce to initialize the stream.
  /// \return The decrypted bytes
  template<std::size_t N>
  [[nodiscard]] consteval std::array<Byte, N> encrypt_ctr(const Byte (&data)[N], const Key &key, const Nonce &nonce) {
    std::array<Byte, N> buffer{};
    std::copy(data, data + N, buffer.begin());
    return encrypt_ctr(buffer, key, nonce);
  }
}

#endif
