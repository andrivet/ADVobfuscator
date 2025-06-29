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

#include <iostream>
#include <advobfuscator/obf.h>
#include <advobfuscator/string.h>
#include "hexdump.h"

inline void describe(andrivet::advobfuscator::KeyAlgorithm algo) {
  using namespace andrivet::advobfuscator;
  switch(algo) {
    using enum KeyAlgorithm;
    case IDENTITY:    std::cout << "Identity"; break;
    case INCREMENT:   std::cout << "Increment"; break;
    case INVERT:      std::cout << "Invert"; break;
    case SUBSTITUTE:  std::cout << "Substitute"; break;
    case SWAP:        std::cout << "Swap"; break;
    default:          std::cout << "Unknown"; break;
  }
}

inline void describe(andrivet::advobfuscator::DataAlgorithm algo) {
  using namespace andrivet::advobfuscator;
  switch(algo) {
    using enum DataAlgorithm;
    case IDENTITY:      std::cout << "Identity"; break;
    case CAESAR:        std::cout << "Caesar"; break;
    case XOR:           std::cout << "XOR"; break;
    case ROTATE:        std::cout << "Rotate bits"; break;
    case SUBSTITUTE:    std::cout << "Substitute"; break;
    default:            std::cout << "Unknown"; break;
  }
}

template<std::size_t N>
void describe(const andrivet::advobfuscator::ObfuscatedString<N> &str, bool raw = true) {
  using namespace andrivet::advobfuscator;
  std::cout << "Algorithms: ";
  for(unsigned i = 0; i < details::MAX_NB_ALGORITHMS; ++i) {
    const auto algo = str.algos_[i];
    if(algo.key_algo() == KeyAlgorithm::IDENTITY && algo.data_algo() == DataAlgorithm::IDENTITY) continue;
    std::cout << "(K="   << static_cast<unsigned>(algo.key());
    std::cout << ", KA="; describe(algo.key_algo());
    std::cout << ", DA="; describe(algo.data_algo());
    std::cout << ") ";
  }
  std::cout << '\n';
  if(raw) {
    std::cout << "Raw data:\n";
    std::cout << hexdump::hexdump16(str.raw(), N) << "\n";
  }
}

template<std::size_t N>
void describe(const andrivet::advobfuscator::ObfuscatedBytes<N> &block, bool raw = true) {
  using namespace andrivet::advobfuscator;
  std::cout << "Algorithms: ";
  for(unsigned i = 0; i < details::MAX_NB_ALGORITHMS; ++i) {
    const auto algo = block.algos_[i];
    if(algo.key_algo() == KeyAlgorithm::IDENTITY && algo.data_algo() == DataAlgorithm::IDENTITY) continue;
    std::cout << "(K="   << static_cast<unsigned>(algo.key());
    std::cout << ", KA="; describe(algo.key_algo());
    std::cout << ", DA="; describe(algo.data_algo());
    std::cout << ") ";
  }
  std::cout << '\n';
  if(raw) {
    std::cout << "Raw data:\n";
    std::cout << hexdump::hexdump32(block.raw(), block.size()) << "\n";
  }
}
