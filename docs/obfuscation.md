# How to obfuscate or encrypt data and code

## Obfuscation of strings

Strings can be obfuscated using ADVobfuscator UDL (user-defined literal) `_obf`:

```c++
#include <advobfuscator/string.h>

std::cout << "abc"_obf << '\n';
```

The string is obfuscated at compile time.
The UDL constructs (at compile-time) an instance of `ObfuscatedString`.
At run-time, there is an implicit cast operator to `const char*` so the deobfuscated string can be converted.
This code is thus (almost) equivalent to:

```c++
std::cout << ObfuscatedString{"abc"}.decode() << '\n';
```

It is also possible to use `std::format`:

```c++
#include <advobfuscator/format.h>

std::cout << std::format("{}\n", "abc"_obf);
```

Instances of obfuscated strings can be manipulated like any object.
The implicit cast operator to `const char*` does modify the instance however (to decode the string).
If the instance is immutable, you have to call explicitly `decode()` that returns a `std::string` and does not modify the instance:

```c++
static constexpr auto s4 = "An immutable compile-time string"_obf;
std::cout << s4.decode() << '\n';
```

## Obfuscation of data

Blocks of data (`uint8_t`) can be obfuscated at compile-time using `_obf_bytes`:

```c++
#include <advobfuscator/bytes.h>

static constexpr auto data = "01 02 04 08 10 20 40 80 1b 36"_obf_bytes;
```

The format has to follow these rules:

* Each byte is represented by two hexadecimal digits.
* These hexadecimal digits can be in lower or upper case.
* Bytes have to be separated by space.

At compile-time, an instance of `ObfuscatedBytes` is created.
This class provides a subscript operator that decodes, at run-time, the obfuscated data:

```c++
auto d = data[0];  // d is a uin8_t
```

It is also possible to decode the whole data with `decode()`:

```c++
auto decoded = data.decode(); // decoded is an std::array<uint8_t, N>
```

There is also a `data()` member function that decodes the data in-place:

```c++
auto data = "01 02 04 08 10 20 40 80 1b 36"_obf_bytes;
auto decoded = data.data(); // decoded is a const std::uint8_t
```


## Encryption of strings with AES

In this version, it is also possible to encrypt the strings at compile-time using AES.
The usage is however limited because of limitation of compilers (compile-time AES is quite complex for them).
In practice, you can also encrypt strings that are not too long with the `_aes` UDL.
The behaviour is similar to obfuscated strings:

```c++
#include <advobfuscator/aes_string.h>

std::cout << "This is a string containing a secret that has to be hidden with AES"_aes << "\n";
```

> Note: The S-box and other well-known data used by AES are obfuscated.


## Obfuscation of function calls

