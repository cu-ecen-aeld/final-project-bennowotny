#include "rgb-color/RGBColor.hpp"

#include <sstream>
#include <stdexcept>

namespace rgb {

std::ostream &operator<<(std::ostream &stream, const rgb::RGBColor &color) {
  stream << color.red << color.green << color.blue;
  return stream;
}

std::string prettyPrint(const rgb::RGBColor &color) {
  std::stringstream stream;
  stream << std::hex << std::showbase << "{ red: " << +(color.red) << ", green: " << +(color.green) << ", blue: " << +(color.blue) << " }";
  return stream.str();
}

RGBColor fromBytes(const std::vector<std::byte> &bytes) {
  if (bytes.size() != 3)
    throw std::invalid_argument{"RGB colors should be only 3 bytes"};

  return {
      .red{static_cast<uint8_t>(bytes.at(0))}, .green{static_cast<uint8_t>(bytes.at(1))}, .blue{static_cast<uint8_t>(bytes.at(2))}};
}

} // namespace rgb
