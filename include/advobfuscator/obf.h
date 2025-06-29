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

#ifndef ADVOBFUSCATOR_OBF_H
#define ADVOBFUSCATOR_OBF_H

#include <array>
#include <bit>
#include "random.h"

namespace andrivet::advobfuscator {

  /// Algorithms to encode data
  enum class DataAlgorithm {
    IDENTITY,   //< Identity function, i.e. no change.
    CAESAR,     //< Caesar algorithm, key is the displacement.
    XOR,        //< XOR with the key.
    ROTATE,     //< Bits rotation, key is the displacement.
    SUBSTITUTE, //< Substitute bits, key % 8 is the displacement.
    NB_VALUES   //< Number of values in this enum.
  };

  /// Algorithms to encode a key from a previous one
  enum class KeyAlgorithm {
    IDENTITY,   //< Identity function, i.e. no change.
    INCREMENT,  //< Key is incremented at each step.
    INVERT,     //< Key is inverted at each step.
    SUBSTITUTE, //< Substitute bits (0 becomes 7, 7 becomes 0, ...) at each step.
    SWAP,       //< Swap high and low nibbles at each step.
    NB_VALUES   //< Number of values in this enum.
  };

  /// Parameters of an obfuscation algorithm.
  struct Parameters {
    std::uint8_t key = 0; ///< Key to be used.
    KeyAlgorithm key_algo = KeyAlgorithm::IDENTITY; ///< Algorithm to compute the next key.
    DataAlgorithm data_algo = DataAlgorithm::IDENTITY; ///< Algorithm to encode data.
  };

  // ------------------------------------------------------------------
  // Internal details
  // ------------------------------------------------------------------

  namespace details {
    /// Minimal number of algorithms
    static const std::size_t MIN_NB_ALGORITHMS = 2;
    /// Maximal number of algorithms
    static const std::size_t MAX_NB_ALGORITHMS = 4;

    /// Substitute bits in a byte.
    /// \param b Input byte.
    /// \param d Number of bits for the substitution.
    /// \remark If d = 7, bits 0 and 7 are exchanged, bits 1 and 6 are exchanged, etc.
    /// If d = 6, bits 0 and 6 are exchanged, bits 1 and 5 are exchanged, etc.
    /// \result The result of the substitution.
    constexpr uint8_t substitute(uint8_t b, uint8_t d) {
      d %= 8;
      uint8_t result = 0;
      for(uint8_t i = 0; i < 8; ++i) {
        auto bit = (b >> i) & 0x01;
        result |= bit << (i <= d ? d - i : 8 - i + d);
      }
      return result;
    }

    /// Generalized Caesar cypher (ROT).
    /// \param b Input byte.
    /// \param d Displacement for the rotation.
    /// \result The result of the rotation.
    constexpr uint8_t caesar(uint8_t b, uint8_t d) {
      return static_cast<std::uint8_t>((d >= 128 ? b + d - 256 : b + d) % 256);
    }

    /// Generalized Inverted Caesar cypher.
    /// \param b Input byte.
    /// \param d Displacement for the inverted rotation.
    /// \result The result of the inverted rotation.
    constexpr uint8_t caesar_inverted(uint8_t b, uint8_t d) {
      return static_cast<std::uint8_t>((d >= 128 ? b - d + 256 : b - d) % 256);
    }

    /// XOR between a byte and a key.
    /// \param b Input byte.
    /// \param key Key to be used to XOR the byte.
    /// \result The result of the XOR operation.
    constexpr uint8_t x0r(uint8_t b, uint8_t key) {
      return static_cast<std::uint8_t>(b ^ key);
    }

    /// Rotate bits in a byte left (positive d) or right (negative d).
    /// \param b Input byte.
    /// \param d Number of bits for the rotation.
    /// \remark If d = 1, bits 0 becomes bit 1, bits 1 become bit 2, etc.
    /// \result The result of the rotation.
    constexpr uint8_t rotate(uint8_t b, uint8_t d) {
      return static_cast<std::uint8_t>(
          d >= 128
           ? std::rotr(static_cast<std::uint8_t>(b), (256 - d) % 8)
           : std::rotl(static_cast<std::uint8_t>(b), d % 8));
    }

    /// Rotate bits in a byte left (negative d) or right (positive d).
    /// \param b Input byte.
    /// \param d Number of bits for the rotation.
    /// \remark If d = 1, bits 0 becomes bit 1, bits 1 become bit 2, etc.
    /// \result The result of the rotation.
    constexpr uint8_t rotate_inverted(uint8_t b, uint8_t d) {
      return static_cast<std::uint8_t>(
          d >= 128
          ? std::rotl(static_cast<std::uint8_t>(b), (256 - d) % 8)
          : std::rotr(static_cast<std::uint8_t>(b), d % 8));
    }

    /// Swap nibbles in a byte.
    /// \result The result of the swapping.
    constexpr uint8_t swap(uint8_t b) {
        return (b & 0xF0 >> 4) | (b & 0x0F << 4);
    }
  }

  // ------------------------------------------------------------------
  // Public interface
  // ------------------------------------------------------------------

  /// An obfuscation algorithm
  struct Obfuscation {
    /// Construct an obfuscation with identity algorithms.
    consteval Obfuscation() = default;

    /// Construct an obfuscation with on the fly algorithms.
    /// \param counter Randomization counter.
    consteval explicit Obfuscation(std::size_t counter) noexcept
    : parameters_{
     .key = generate_random_not_0<std::uint8_t>(counter, 0x7F),
     .key_algo = generate_random(counter + 2, KeyAlgorithm::NB_VALUES), // Identity is acceptable here
     .data_algo = generate_random_not_0(counter + 1, DataAlgorithm::NB_VALUES)
    } {}

    /// Construct an obfuscation with explicit algorithms.
    /// \param params Parameters for the obfuscation (key and algorithms).
    consteval explicit Obfuscation(const Parameters &params) noexcept : parameters_{params} {}

    /// Encode a byte.
    /// \param key Key to be used for the encoding.
    /// \return The encoded byte.
    [[nodiscard]] consteval std::uint8_t encode(std::uint8_t c, std::uint8_t key) const {
      switch(parameters_.data_algo) {
        using enum DataAlgorithm;
        case IDENTITY: break;
        case CAESAR: return details::caesar(c, key);
        case XOR: return details::x0r(c, key);
        case ROTATE: return details::rotate(c, key);
        case SUBSTITUTE: return details::substitute(c, key);
        case NB_VALUES: throw std::exception(); // Invalid data encoding
      }
      return c;
    }

    /// Decode a byte.
    /// \param key Key to be used for the decoding.
    /// \return The decoded byte.
    [[nodiscard]] constexpr std::uint8_t decode(std::uint8_t c, std::uint8_t key) const {
      switch(parameters_.data_algo) {
        using enum DataAlgorithm;
        case IDENTITY: break;
        case CAESAR: return details::caesar_inverted(c, key);
        case XOR: return static_cast<std::uint8_t>(c ^ key);
        case ROTATE: return details::rotate_inverted(c, key);
        case SUBSTITUTE: return details::substitute(c, key);
        case NB_VALUES: throw std::exception(); // Invalid data encoding
      }
      return c;
    }

    /// Compute the next key from the current one.
    /// \param key The current key.
    /// \return The new key computed from the given key.
    [[nodiscard]] constexpr std::uint8_t next_key(std::uint8_t key) const {
      switch(parameters_.key_algo) {
        using enum KeyAlgorithm;
        case IDENTITY: break; // This is acceptable here
        case INCREMENT: return static_cast<std::uint8_t>((key + 1) % 256);
        case INVERT: return details::x0r(key, 0xFF);
        case SUBSTITUTE: return details::substitute(key, 7);
        case SWAP: return details::swap(key);
        default: throw std::exception(); // Invalid key encoding;
      }
      return key;
    }

    /// Encode a range of data.
    /// \param begin_pos Relative position of the beginning of the range from the whole data.
    /// \param begin Pointer to the first byte to encode.
    /// \param end Pointer past the last byte to encode.
    template<typename It>
    consteval void encode(std::size_t begin_pos, It begin, It end) const noexcept {
      auto key = parameters_.key;
      while(begin_pos-- > 0) key = next_key(key);
      for(auto current = begin; current < end; key = next_key(key), ++current)
        *current = encode(*current, key);
    }

    /// Decode a range of data.
    /// \param begin_pos Relative position of the beginning of the range from the whole data.
    /// \param begin Pointer to the first byte to decode.
    /// \param end Pointer past the last byte to decode.
    template<typename It>
    constexpr void decode(std::size_t begin_pos, It begin, It end) const noexcept {
      auto key = parameters_.key;
      while(begin_pos-- > 0) key = next_key(key);
      for(auto current = begin; current < end; key = next_key(key), ++current)
        *current = decode(*current, key);
    }

    /// Get the current key for the obfuscation
    [[nodiscard]] constexpr std::uint8_t key() const noexcept { return parameters_.key; }

    /// Get the algorithm to compute the next key
    [[nodiscard]] constexpr KeyAlgorithm key_algo() const noexcept { return parameters_.key_algo; }

    /// Get the algorithm to encode data
    [[nodiscard]] constexpr DataAlgorithm data_algo() const noexcept { return parameters_.data_algo; }

    /// Parameters for the obfuscation (key and algorithms).
    Parameters parameters_;
  };

  // ------------------------------------------------------------------
  // Internal details
  // ------------------------------------------------------------------

  namespace details {

    /// Construct an Identity obfuscation (i.e. no obfuscation)
    consteval Obfuscation
    make_algorithm() { return Obfuscation{}; }

    /// Construct an random generated obfuscation.
    /// \param counter Randomization counter.
    consteval Obfuscation
    make_algorithm(std::size_t counter) { return Obfuscation{counter}; }

    /// Construct an obfuscation with explicit algorithms.
    /// \param params Parameters for the obfuscation (key and algorithms).
    consteval Obfuscation
    make_algorithm(const Parameters &params) { return Obfuscation{params}; }

    /// Construct a set of obfuscations.
    /// \param params I Number of actual obfuscations to generate (others will be identity).
    /// \param counter Randomization counter.
    /// \param nb_algorithms Number of obfuscations to generate.
    template<std::size_t... I>
    consteval std::array<Obfuscation, sizeof...(I)>
    make_algorithms(std::size_t counter, std::size_t nb_algorithms, std::index_sequence<I...>) {
      return {I >= nb_algorithms ? make_algorithm() : make_algorithm(counter + 3 * I)...};
    }

    /// Construct a set of obfuscations.
    /// \param params I Number of actual obfuscations to generate (others will be identity).
    /// \param A Size of the array of parameters.
    /// \param params Array of parameters for the obfuscation (key and algorithms).
    template<std::size_t A, std::size_t... I>
    consteval std::array<Obfuscation, sizeof...(I)>
    make_algorithms(const Parameters (&params)[A], std::index_sequence<I...>) {
      return {I >= A ? make_algorithm() : make_algorithm(params[I])...};
    }
  }

  /// A set of obfuscations
  struct Obfuscations {
    /// Construct a set of random generated obfuscations.
    /// \param counter Randomization counter.
    consteval explicit Obfuscations(std::size_t counter) noexcept
    : algos_{details::make_algorithms(
        counter,
        generate_random(counter, details::MIN_NB_ALGORITHMS, details::MAX_NB_ALGORITHMS),
        std::make_index_sequence<details::MAX_NB_ALGORITHMS>{}
    )} {}

    /// Construct a set of one obfuscations with explicit parameters.
    /// \param params Parameters for the obfuscation (key and algorithms).
    consteval explicit Obfuscations(const Parameters &params) noexcept
    : algos_{details::make_algorithm(params)} {}

    /// Construct a set of obfuscations with explicit parameters.
    /// \param A Size of the array of parameters.
    /// \param params Array of parameters for the obfuscation (key and algorithms).
    template<std::size_t A>
    consteval explicit Obfuscations(const Parameters (&params)[A]) noexcept
    : algos_{details::make_algorithms<A>(
      params,
      std::make_index_sequence<details::MAX_NB_ALGORITHMS>{})} {}

    /// Encode a range of data.
    /// \param begin_pos Relative position of the beginning of the range from the whole data.
    /// \param begin Pointer to the first byte to encode.
    /// \param end Pointer past the last byte to encode.
    template<typename It>
    consteval void encode(std::size_t begin_pos, It begin, It end) const {
      for(std::size_t i = 0; i < details::MAX_NB_ALGORITHMS; ++i)
        algos_[i].encode(begin_pos, begin, end);
    }

    /// Decode a range of data.
    /// \param begin_pos Relative position of the beginning of the range from the whole data.
    /// \param begin Pointer to the first byte to decode.
    /// \param end Pointer past the last byte to decode.
    template<typename It>
    constexpr void decode(std::size_t begin_pos, It begin, It end) const noexcept {
      for(std::size_t i = 0; i < details::MAX_NB_ALGORITHMS; ++i)
        algos_[details::MAX_NB_ALGORITHMS - i - 1].decode(begin_pos, begin, end);
    }

    /// Get a decoded element.
    /// \param index Position of the element to decode and return.
    constexpr Obfuscation &operator[](std::size_t index) noexcept { return algos_[index]; }

    /// Get a decoded element.
    /// \param index Position of the element to decode and return.
    constexpr const Obfuscation &operator[](std::size_t index) const noexcept { return algos_[index]; }

    /// A set of obfuscations
    std::array<Obfuscation, details::MAX_NB_ALGORITHMS> algos_;
  };
}

#endif