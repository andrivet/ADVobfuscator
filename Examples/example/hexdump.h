// https://github.com/zmb3/hexdump
// (edited)

#ifndef HEXDUMP_HPP
#define HEXDUMP_HPP

#include <cctype>
#include <iomanip>
#include <ostream>

namespace andrivet::advobfuscator {
  template<std::size_t N>
  struct ObfuscatedBytes;
}

namespace hexdump {

  template<std::size_t, bool>
  struct custom_hexdump {
    custom_hexdump(const void *data, std::size_t length)
    : data_{static_cast<const std::uint8_t*>(data)}, length_{length} {}

    template<std::size_t N>
    custom_hexdump(const std::array<std::uint8_t, N> &block)
        : data_(block.data()), length_{block.size()} {}

    template<std::size_t N>
    custom_hexdump(andrivet::advobfuscator::ObfuscatedBytes<N> block)
    : data_(block.data()), length_{block.size()} {}

    const std::uint8_t *data_;
    const std::size_t length_;
  };

  template<std::size_t row_size, bool show_ascii>
  std::basic_ostream<char> &operator<<(std::basic_ostream<char> &out, const custom_hexdump<row_size, show_ascii> &dump) {
    out.fill('0');
    for(std::size_t i = 0; i < dump.length_; i += row_size) {
      out << "0x" << std::setw(6) << std::hex << i << std::dec << ": ";
      for(int j = 0; j < row_size; ++j) {
        if (i + j < dump.length_)
          out << std::hex << std::setw(2) << static_cast<int>(dump.data_[i + j]) << std::dec << " ";
        else
          out << "   ";
      }

      out << " ";
      if(show_ascii) {
        for(std::size_t j = 0; j < row_size; ++j) {
          if(i + j < dump.length_) {
            if(std::isprint(dump.data_[i + j]))
              out << static_cast<char>(dump.data_[i + j]);
            else
              out << ".";
          }
        }
      }
      out << '\n';
    }

    return out;
  }

  using hexdump8 = custom_hexdump<8, true>;
  using hexdump16 = custom_hexdump<16, true>;
  using hexdump32 = custom_hexdump<32, true>;
}

#endif // HEXDUMP_HPP
