#include "rgb-color/RGBColor.hpp"

#include <sstream>

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

} // namespace rgb
