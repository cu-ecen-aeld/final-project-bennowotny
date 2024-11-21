#ifndef RGB_COLOR_HPP
#define RGB_COLOR_HPP

#include <cstdint>
#include <ostream>

namespace rgb {

struct RGBColor {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

std::ostream &operator<<(std::ostream &stream, const RGBColor &color);
std::string prettyPrint(const RGBColor &color);

} // namespace rgb

#endif
