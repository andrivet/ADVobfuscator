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

#include <cassert>
#include <advobfuscator/string.h>
#include <advobfuscator/bytes.h>
#include <advobfuscator/aes.h>
#include <advobfuscator/aes_string.h>

using namespace andrivet::advobfuscator;

void test_strings_obfuscation() {
  auto s0 = "abc"_obf;

  assert(
    s0.raw()[0] != 'a' ||
    s0.raw()[1] != 'b' ||
    s0.raw()[2] != 'c' ||
    s0.raw()[3] != 0
  );
  assert(s0[0] == 'a');
  assert(s0[1] == 'b');
  assert(s0[2] == 'c');
  assert(s0[3] == 0);

  auto s1 = "ABCDEFGHIJKLMNOP"_obf;
  assert(
    s1.raw()[0] != 'A' ||
    s1.raw()[1] != 'B' ||
    s1.raw()[2] != 'C' ||
    s1.raw()[3] != 'D' ||
    s1.raw()[4] != 'E' ||
    s1.raw()[5] != 'F' ||
    s1.raw()[6] != 'G' ||
    s1.raw()[7] != 'H' ||
    s1.raw()[8] != 'I' ||
    s1.raw()[9] != 'J' ||
    s1.raw()[10] != 'K' ||
    s1.raw()[11] != 'L' ||
    s1.raw()[12] != 'M' ||
    s1.raw()[13] != 'N' ||
    s1.raw()[14] != 'O' ||
    s1.raw()[15] != 'P' ||
    s1.raw()[16] != 0
  );
  assert(s1[0] == 'A');
  assert(s1[1] == 'B');
  assert(s1[2] == 'C');
  assert(s1[3] == 'D');
  assert(s1[4] == 'E');
  assert(s1[5] == 'F');
  assert(s1[6] == 'G');
  assert(s1[7] == 'H');
  assert(s1[8] == 'I');
  assert(s1[9] == 'J');
  assert(s1[10] == 'K');
  assert(s1[11] == 'L');
  assert(s1[12] == 'M');
  assert(s1[13] == 'N');
  assert(s1[14] == 'O');
  assert(s1[15] == 'P');
  assert(s1[16] == 0);

  auto s2{"0123456789"_obf};
  assert(
    s2.raw()[0] != '0' ||
    s2.raw()[1] != '1' ||
    s2.raw()[2] != '2' ||
    s2.raw()[3] != '3' ||
    s2.raw()[4] != '4' ||
    s2.raw()[5] != '5' ||
    s2.raw()[6] != '6' ||
    s2.raw()[7] != '7' ||
    s2.raw()[8] != '8' ||
    s2.raw()[9] != '9' ||
    s2.raw()[10] != 0
  );
  assert(s2[0] == '0');
  assert(s2[1] == '1');
  assert(s2[2] == '2');
  assert(s2[3] == '3');
  assert(s2[4] == '4');
  assert(s2[5] == '5');
  assert(s2[6] == '6');
  assert(s2[7] == '7');
  assert(s2[8] == '8');
  assert(s2[9] == '9');
  assert(s2[10] == 0);

  auto s3 = ObfuscatedString("abcd", {1, KeyAlgorithm::IDENTITY, DataAlgorithm::XOR});
  assert(
    s3.raw()[0] != 'a' ||
    s3.raw()[1] != 'b' ||
    s3.raw()[2] != 'c' ||
    s3.raw()[3] != 'd' ||
    s3.raw()[4] != 0
  );
  assert(s3[0] == 'a');
  assert(s3[1] == 'b');
  assert(s3[2] == 'c');
  assert(s3[3] == 'd');
  assert(s3[4] == 0);

  static constexpr Parameters params[] = {
      {.key=1, .key_algo=KeyAlgorithm::IDENTITY, .data_algo=DataAlgorithm::XOR},
      {.key=2, .key_algo=KeyAlgorithm::IDENTITY, .data_algo=DataAlgorithm::XOR}
  };
  auto s4 = ObfuscatedString("abcde", params);
  assert(
    s4.raw()[0] != 'a' ||
    s4.raw()[1] != 'b' ||
    s4.raw()[2] != 'c' ||
    s4.raw()[3] != 'd' ||
    s4.raw()[4] != 'e' ||
    s4.raw()[5] != 0
  );
  assert(s4[0] == 'a');
  assert(s4[1] == 'b');
  assert(s4[2] == 'c');
  assert(s4[3] == 'd');
  assert(s4[4] == 'e');
  assert(s4[5] == 0);

  static constexpr auto s5 = "An immutable compile-time string"_obf;
  assert(s5.decode() == "An immutable compile-time string");
}

void test_block_obfuscation() {
  static constexpr auto rcon = "01 02 04 08 10 20 40 80 1b 36"_obf_bytes;
  auto decoded = rcon.decode();

  assert(
      rcon.raw()[0] != 0x01 ||
      rcon.raw()[1] != 0x02 ||
      rcon.raw()[2] != 0x04 ||
      rcon.raw()[3] != 0x08 ||
      rcon.raw()[4] != 0x10 ||
      rcon.raw()[5] != 0x10 ||
      rcon.raw()[6] != 0x40 ||
      rcon.raw()[7] != 0x80 ||
      rcon.raw()[8] != 0x1b ||
      rcon.raw()[9] != 0x36
  );

  assert(decoded[0] == 0x01);
  assert(decoded[1] == 0x02);
  assert(decoded[2] == 0x04);
  assert(decoded[3] == 0x08);
  assert(decoded[4] == 0x10);
  assert(decoded[5] == 0x20);
  assert(decoded[6] == 0x40);
  assert(decoded[7] == 0x80);
  assert(decoded[8] == 0x1b);
  assert(decoded[9] == 0x36);

  assert(rcon[0] == 0x01);
  assert(rcon[1] == 0x02);
  assert(rcon[2] == 0x04);
  assert(rcon[3] == 0x08);
  assert(rcon[4] == 0x10);
  assert(rcon[5] == 0x20);
  assert(rcon[6] == 0x40);
  assert(rcon[7] == 0x80);
  assert(rcon[8] == 0x1b);
  assert(rcon[9] == 0x36);

}

void test_aes_key_expansion() {
  // https://csrc.nist.gov/files/pubs/fips/197/final/docs/fips-197.pdf
  // Appendix A - Key Expansion Examples

  static constexpr Key key = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

  const auto expanded = details::key_expansion(key);

  uint32_t w00 = expanded[ 0][0] << 24 | expanded[ 0][1] << 16 | expanded[ 0][2] << 8 | expanded[ 0][3];
  uint32_t w01 = expanded[ 1][0] << 24 | expanded[ 1][1] << 16 | expanded[ 1][2] << 8 | expanded[ 1][3];
  uint32_t w02 = expanded[ 2][0] << 24 | expanded[ 2][1] << 16 | expanded[ 2][2] << 8 | expanded[ 2][3];
  uint32_t w03 = expanded[ 3][0] << 24 | expanded[ 3][1] << 16 | expanded[ 3][2] << 8 | expanded[ 3][3];
  uint32_t w04 = expanded[ 4][0] << 24 | expanded[ 4][1] << 16 | expanded[ 4][2] << 8 | expanded[ 4][3];
  uint32_t w05 = expanded[ 5][0] << 24 | expanded[ 5][1] << 16 | expanded[ 5][2] << 8 | expanded[ 5][3];
  uint32_t w06 = expanded[ 6][0] << 24 | expanded[ 6][1] << 16 | expanded[ 6][2] << 8 | expanded[ 6][3];
  uint32_t w07 = expanded[ 7][0] << 24 | expanded[ 7][1] << 16 | expanded[ 7][2] << 8 | expanded[ 7][3];
  uint32_t w08 = expanded[ 8][0] << 24 | expanded[ 8][1] << 16 | expanded[ 8][2] << 8 | expanded[ 8][3];
  uint32_t w09 = expanded[ 9][0] << 24 | expanded[ 9][1] << 16 | expanded[ 9][2] << 8 | expanded[ 9][3];
  uint32_t w10 = expanded[10][0] << 24 | expanded[10][1] << 16 | expanded[10][2] << 8 | expanded[10][3];
  uint32_t w11 = expanded[11][0] << 24 | expanded[11][1] << 16 | expanded[11][2] << 8 | expanded[11][3];
  uint32_t w12 = expanded[12][0] << 24 | expanded[12][1] << 16 | expanded[12][2] << 8 | expanded[12][3];
  uint32_t w13 = expanded[13][0] << 24 | expanded[13][1] << 16 | expanded[13][2] << 8 | expanded[13][3];
  uint32_t w14 = expanded[14][0] << 24 | expanded[14][1] << 16 | expanded[14][2] << 8 | expanded[14][3];
  uint32_t w15 = expanded[15][0] << 24 | expanded[15][1] << 16 | expanded[15][2] << 8 | expanded[15][3];
  uint32_t w16 = expanded[16][0] << 24 | expanded[16][1] << 16 | expanded[16][2] << 8 | expanded[16][3];
  uint32_t w17 = expanded[17][0] << 24 | expanded[17][1] << 16 | expanded[17][2] << 8 | expanded[17][3];
  uint32_t w18 = expanded[18][0] << 24 | expanded[18][1] << 16 | expanded[18][2] << 8 | expanded[18][3];
  uint32_t w19 = expanded[19][0] << 24 | expanded[19][1] << 16 | expanded[19][2] << 8 | expanded[19][3];
  uint32_t w20 = expanded[20][0] << 24 | expanded[20][1] << 16 | expanded[20][2] << 8 | expanded[20][3];
  uint32_t w21 = expanded[21][0] << 24 | expanded[21][1] << 16 | expanded[21][2] << 8 | expanded[21][3];
  uint32_t w22 = expanded[22][0] << 24 | expanded[22][1] << 16 | expanded[22][2] << 8 | expanded[22][3];
  uint32_t w23 = expanded[23][0] << 24 | expanded[23][1] << 16 | expanded[23][2] << 8 | expanded[23][3];
  uint32_t w24 = expanded[24][0] << 24 | expanded[24][1] << 16 | expanded[24][2] << 8 | expanded[24][3];
  uint32_t w25 = expanded[25][0] << 24 | expanded[25][1] << 16 | expanded[25][2] << 8 | expanded[25][3];
  uint32_t w26 = expanded[26][0] << 24 | expanded[26][1] << 16 | expanded[26][2] << 8 | expanded[26][3];
  uint32_t w27 = expanded[27][0] << 24 | expanded[27][1] << 16 | expanded[27][2] << 8 | expanded[27][3];
  uint32_t w28 = expanded[28][0] << 24 | expanded[28][1] << 16 | expanded[28][2] << 8 | expanded[28][3];
  uint32_t w29 = expanded[29][0] << 24 | expanded[29][1] << 16 | expanded[29][2] << 8 | expanded[29][3];
  uint32_t w30 = expanded[30][0] << 24 | expanded[30][1] << 16 | expanded[30][2] << 8 | expanded[30][3];
  uint32_t w31 = expanded[31][0] << 24 | expanded[31][1] << 16 | expanded[31][2] << 8 | expanded[31][3];
  uint32_t w32 = expanded[32][0] << 24 | expanded[32][1] << 16 | expanded[32][2] << 8 | expanded[32][3];
  uint32_t w33 = expanded[33][0] << 24 | expanded[33][1] << 16 | expanded[33][2] << 8 | expanded[33][3];
  uint32_t w34 = expanded[34][0] << 24 | expanded[34][1] << 16 | expanded[34][2] << 8 | expanded[34][3];
  uint32_t w35 = expanded[35][0] << 24 | expanded[35][1] << 16 | expanded[35][2] << 8 | expanded[35][3];
  uint32_t w36 = expanded[36][0] << 24 | expanded[36][1] << 16 | expanded[36][2] << 8 | expanded[36][3];
  uint32_t w37 = expanded[37][0] << 24 | expanded[37][1] << 16 | expanded[37][2] << 8 | expanded[37][3];
  uint32_t w38 = expanded[38][0] << 24 | expanded[38][1] << 16 | expanded[38][2] << 8 | expanded[38][3];
  uint32_t w39 = expanded[39][0] << 24 | expanded[39][1] << 16 | expanded[39][2] << 8 | expanded[39][3];
  uint32_t w40 = expanded[40][0] << 24 | expanded[40][1] << 16 | expanded[40][2] << 8 | expanded[40][3];
  uint32_t w41 = expanded[41][0] << 24 | expanded[41][1] << 16 | expanded[41][2] << 8 | expanded[41][3];
  uint32_t w42 = expanded[42][0] << 24 | expanded[42][1] << 16 | expanded[42][2] << 8 | expanded[42][3];
  uint32_t w43 = expanded[43][0] << 24 | expanded[43][1] << 16 | expanded[43][2] << 8 | expanded[43][3];

  assert(w00 == 0x2b7e1516);
  assert(w01 == 0x28aed2a6);
  assert(w02 == 0xabf71588);
  assert(w03 == 0x09cf4f3c);

  assert(w04 == 0xa0fafe17);
  assert(w05 == 0x88542cb1);
  assert(w06 == 0x23a33939);
  assert(w07 == 0x2a6c7605);

  assert(w08 == 0xf2c295f2);
  assert(w09 == 0x7a96b943);
  assert(w10 == 0x5935807a);
  assert(w11 == 0x7359f67f);

  assert(w12 == 0x3d80477d);
  assert(w13 == 0x4716fe3e);
  assert(w14 == 0x1e237e44);
  assert(w15 == 0x6d7a883b);

  assert(w16 == 0xef44a541);
  assert(w17 == 0xa8525b7f);
  assert(w18 == 0xb671253b);
  assert(w19 == 0xdb0bad00);

  assert(w20 == 0xd4d1c6f8);
  assert(w21 == 0x7c839d87);
  assert(w22 == 0xcaf2b8bc);
  assert(w23 == 0x11f915bc);

  assert(w24 == 0x6d88a37a);
  assert(w25 == 0x110b3efd);
  assert(w26 == 0xdbf98641);
  assert(w27 == 0xca0093fd);

  assert(w28 == 0x4e54f70e);
  assert(w29 == 0x5f5fc9f3);
  assert(w30 == 0x84a64fb2);
  assert(w31 == 0x4ea6dc4f);

  assert(w32 == 0xead27321);
  assert(w33 == 0xb58dbad2);
  assert(w34 == 0x312bf560);
  assert(w35 == 0x7f8d292f);

  assert(w36 == 0xac7766f3);
  assert(w37 == 0x19fadc21);
  assert(w38 == 0x28d12941);
  assert(w39 == 0x575c006e);

  assert(w40 == 0xd014f9a8);
  assert(w41 == 0xc9ee2589);
  assert(w42 == 0xe13f0cc8);
  assert(w43 == 0xb6630ca6);
}

void test_aes_cipher() {
  // https://csrc.nist.gov/files/pubs/fips/197/final/docs/fips-197.pdf
  // Appendix B - Cipher Example

  static constexpr Block input = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
  static constexpr Key key = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

  const auto encrypted = encrypt(input, key);
  assert(encrypted[ 0] == 0x39); assert(encrypted[ 1] == 0x25); assert(encrypted[ 2] == 0x84); assert(encrypted[ 3] == 0x1d);
  assert(encrypted[ 4] == 0x02); assert(encrypted[ 5] == 0xdc); assert(encrypted[ 6] == 0x09); assert(encrypted[ 7] == 0xfb);
  assert(encrypted[ 8] == 0xdc); assert(encrypted[ 9] == 0x11); assert(encrypted[10] == 0x85); assert(encrypted[11] == 0x97);
  assert(encrypted[12] == 0x19); assert(encrypted[13] == 0x6a); assert(encrypted[14] == 0x0b); assert(encrypted[15] == 0x32);

  const auto decrypted = decrypt(encrypted, key);
  assert(decrypted[ 0] == 0x32); assert(decrypted[ 1] == 0x43); assert(decrypted[ 2] == 0xf6); assert(decrypted[ 3] == 0xa8);
  assert(decrypted[ 4] == 0x88); assert(decrypted[ 5] == 0x5a); assert(decrypted[ 6] == 0x30); assert(decrypted[ 7] == 0x8d);
  assert(decrypted[ 8] == 0x31); assert(decrypted[ 9] == 0x31); assert(decrypted[10] == 0x98); assert(decrypted[11] == 0xa2);
  assert(decrypted[12] == 0xe0); assert(decrypted[13] == 0x37); assert(decrypted[14] == 0x07); assert(decrypted[15] == 0x34);
}

void test_aes_ctr_cipher() {
  static constexpr Byte input[] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34, 0x69, 0x65, 0xfa, 0x98, 0x18, 0xad, 0x58};
  static constexpr Key key = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
  static constexpr Nonce nonce = {0xb2, 0x96, 0x75, 0x8f, 0x1b, 0x06, 0x5d, 0x3e};

  auto encrypted = encrypt_ctr(input, key, nonce);
  decrypt_ctr(encrypted.data(), encrypted.size(), key, nonce);

  auto decrypted = encrypted.data();
  assert(decrypted[ 0] == input[ 0]); assert(decrypted[ 1] == input[ 1]); assert(decrypted[ 2] == input[ 2]); assert(decrypted[ 3] == input[ 3]);
  assert(decrypted[ 4] == input[ 4]); assert(decrypted[ 5] == input[ 5]); assert(decrypted[ 6] == input[ 6]); assert(decrypted[ 7] == input[ 7]);
  assert(decrypted[ 8] == input[ 8]); assert(decrypted[ 9] == input[ 9]); assert(decrypted[10] == input[10]); assert(decrypted[11] == input[11]);
  assert(decrypted[12] == input[12]); assert(decrypted[13] == input[13]); assert(decrypted[14] == input[14]); assert(decrypted[15] == input[15]);
  assert(decrypted[16] == input[16]); assert(decrypted[17] == input[17]); assert(decrypted[18] == input[18]); assert(decrypted[19] == input[19]);
  assert(decrypted[20] == input[20]); assert(decrypted[21] == input[21]); assert(decrypted[22] == input[22]);
}


int main() {
  test_strings_obfuscation();
  test_block_obfuscation();
  test_aes_key_expansion();
  test_aes_cipher();
  test_aes_ctr_cipher();
  return 0;
}
