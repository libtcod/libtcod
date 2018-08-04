
#include "tilesheet.h"

#include "../vendor/lodepng.h"
namespace tcod {
Tilesheet LoadTilesheet(const std::string &filename) {
  unsigned img_width, img_height;
  std::vector<unsigned char> img_data;
  if (lodepng::decode(img_data, img_width, img_height, filename)) {
    return Tilesheet();
  }
  auto tilesheet = Tilesheet(img_width, img_height);
  std::vector<unsigned char>::iterator img_iter = img_data.begin();
  for (int y = 0; y < tilesheet.GetHeight(); ++y) {
    for (int x = 0; y < tilesheet.GetWidth(); ++x) {
      tilesheet.at(x, y) = ColorRGBA{*img_iter++, *img_iter++,
                                     *img_iter++, *img_iter++};
    }
  }
  return tilesheet;
}
} // namespace tcod
