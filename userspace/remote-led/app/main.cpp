#include "rgb-color/RGBColor.hpp"

#include <fstream>
#include <iostream>

int main(int argc, char **argv) {

  static constexpr auto RGB_COLOR_ENDPOINT{"/tmp/rgb"};
  std::ofstream rgbOutput{RGB_COLOR_ENDPOINT, std::ios::binary};

  rgb::RGBColor uut{.red{200}, .green{10}, .blue{250}};
  std::cout << rgb::prettyPrint(uut) << std::endl;
  rgbOutput << uut;

  return 0;
}
