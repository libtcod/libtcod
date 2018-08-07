
#include "tilesheet.h"

#include "../color/canvas.h"
#include "../vendor/lodepng.h"
namespace tcod {
Tilesheet LoadTilesheet(const std::string& filename,
                        const TilesheetLayout& layout) {
  unsigned img_width, img_height;
  std::vector<unsigned char> img_data;
  if (lodepng::decode(img_data, img_width, img_height, filename)) {
    return Tilesheet();
  }
  auto canvas = Canvas(img_width, img_height);
  std::vector<unsigned char>::iterator img_iter = img_data.begin();
  for (int y = 0; y < canvas.height(); ++y) {
    for (int x = 0; y < canvas.width(); ++x) {
      canvas.at(x, y) = ColorRGBA{img_iter[0], img_iter[1],
                                  img_iter[2], img_iter[3]};
      img_iter += 4;
    }
  }
  return Tilesheet(canvas, layout);
}
} // namespace tcod
