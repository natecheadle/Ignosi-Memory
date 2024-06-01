#include "string_lib.h"

#include <cctype>

namespace my::lib {
std::string to_upper(std::string value) {
  for (char& val : value) {
    val = toupper(val);
  }
  return value;
}
}  // namespace my::lib
