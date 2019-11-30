#include <cstring>
#include <iostream>

struct Big5Char {
  unsigned char value[4]; // use additional char to store \0

  bool isZhuYin() const {
    // ZhuYin range: [0xA374, 0xA37E] union [0xA3A1, 0xA3BA]
    return value[0] == 0xa3 && value[1] > 0x73 && value[1] < 0xbb &&
           (value[1] < 0x7f || value[1] > 0xa0);
  }

  bool operator==(const Big5Char &other) const {
    return value[0] == other.value[0] && value[1] == other.value[1];
  }

  void setChar(const char *src) {
    strncpy(reinterpret_cast<char *>(value), src, 2);
    value[2] = 0;
  }

  void setValue(const char *src) {
    strncpy(reinterpret_cast<char *>(value), src, 4);
  }

  operator const char *() const {
    return reinterpret_cast<const char *>(value);
  }

  friend std::istream &operator>>(std::istream &is, Big5Char &c) {
    is >> c.value[0] >> c.value[1];
    c.value[2] = 0;
    return is;
  }
};

namespace std {
template <> struct hash<Big5Char> {
  size_t operator()(const Big5Char &k) const {
    size_t h = k.value[0];
    return (h << 8) + k.value[1];
  }
};
} // namespace std