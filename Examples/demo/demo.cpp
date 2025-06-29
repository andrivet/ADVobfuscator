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
#include <iomanip>
#include <advobfuscator/string.h>
#include <advobfuscator/bytes.h>
#include <advobfuscator/aes.h>
#include <advobfuscator/aes_string.h>

using namespace andrivet::advobfuscator;


void strings_obfuscation() {
  // Obfuscate a string literal
  std::cout << "abc"_obf << '\n';

  // Obfuscate a string literal and describe it after deobfuscation
  auto s1{"0123456789"_obf};
  std::cout << s1 << "\n";

  // Construct explicitly an ObfuscatedString
  auto s2 = ObfuscatedString("abcd", {1, KeyAlgorithm::IDENTITY, DataAlgorithm::XOR});
  std::cout << s2 << '\n';

  // Construct explicitly an ObfuscatedString with precise obfuscation parameters
  static constexpr Parameters params[] = {
    {.key=1, .key_algo=KeyAlgorithm::IDENTITY, .data_algo=DataAlgorithm::XOR},
    {.key=2, .key_algo=KeyAlgorithm::IDENTITY, .data_algo=DataAlgorithm::XOR}
  };
  auto s3 = ObfuscatedString("abcde", params);
  std::cout << s3 << '\n';

  static constexpr auto s4 = "An immutable compile-time string"_obf;
  // It is not possible to use directly s4 since it is immutable:
  // std::cout << s4 << '\n'; // Compilation failure
  // So use decode() instead:
  std::cout << s4.decode() << '\n';
}

void blocks_obfuscation() {

  static constexpr auto rcon = "01 02 04 08 10 20 40 80 1b 36"_obf_bytes;

  for(std::size_t i = 0; i < rcon.size(); ++i)
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)rcon[i] << ' ';
  std::cout << std::dec << '\n';

  // Note that it is a static constexpr
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

  for(const auto &box : sbox) {
    // This is not permitted since data() modifies the object (non-const)
    // std::cout << hexdump32(i.data(), 16);
    // So use decode() instead:
    const auto &decoded = box.decode();
    for(const auto &i : decoded)
      std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)i << ' ';
    std::cout << std::dec << '\n';
  }
}

void aes_encryption_certificate() {
  auto s1 = R"(-----BEGIN CERTIFICATE-----
MIICUTCCAfugAwIBAgIBADANBgkqhkiG9w0BAQQFADBXMQswCQYDVQQGEwJDTjEL
MAkGA1UECBMCUE4xCzAJBgNVBAcTAkNOMQswCQYDVQQKEwJPTjELMAkGA1UECxMC
VU4xFDASBgNVBAMTC0hlcm9uZyBZYW5nMB4XDTA1MDcxNTIxMTk0N1oXDTA1MDgx
NDIxMTk0N1owVzELMAkGA1UEBhMCQ04xCzAJBgNVBAgTAlBOMQswCQYDVQQHEwJD
TjELMAkGA1UEChMCT04xCzAJBgNVBAsTAlVOMRQwEgYDVQQDEwtIZXJvbmcgWWFu
ZzBcMA0GCSqGSIb3DQEBAQUAA0sAMEgCQQCp5hnG7ogBhtlynpOS21cBewKE/B7j
V14qeyslnr26xZUsSVko36ZnhiaO/zbMOoRcKK9vEcgMtcLFuQTWDl3RAgMBAAGj
gbEwga4wHQYDVR0OBBYEFFXI70krXeQDxZgbaCQoR4jUDncEMH8GA1UdIwR4MHaA
FFXI70krXeQDxZgbaCQoR4jUDncEoVukWTBXMQswCQYDVQQGEwJDTjELMAkGA1UE
CBMCUE4xCzAJBgNVBAcTAkNOMQswCQYDVQQKEwJPTjELMAkGA1UECxMCVU4xFDAS
BgNVBAMTC0hlcm9uZyBZYW5nggEAMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEE
BQADQQA/ugzBrjjK9jcWnDVfGHlk3icNRq0oV7Ri32z/+HQX67aRfgZu7KWdI+Ju
Wm7DCfrPNGVwFWUQOmsPue9rZBgO
-----END CERTIFICATE-----)"_obf;

  std::cout << s1 << '\n';
}

void aes_encryption_strings() {
  std::cout << "This is a string containing a secret that has to be hidden with AES"_aes << "\n";
}

int main() {
  strings_obfuscation();
  blocks_obfuscation();
  aes_encryption_certificate();
  aes_encryption_strings();
  return 0;
};