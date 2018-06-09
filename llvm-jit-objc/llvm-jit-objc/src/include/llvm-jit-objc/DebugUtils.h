#include <iomanip>
#include <sstream>

template<typename T>
std::string to_hex16(T val) {
  constexpr int HEX_DIGIT_BITS = 4; // One hex digit = 4 bits.
  constexpr int HEX_BASE_CHARS = 2; // For the "0x".

  std::stringstream sformatter;
  sformatter << std::hex
  << std::internal
  << std::showbase
  << std::setfill('0')
  << std::setw((sizeof(T) * CHAR_BIT / HEX_DIGIT_BITS) + HEX_BASE_CHARS)
  << (uintptr_t)val;

  return sformatter.str();
}
