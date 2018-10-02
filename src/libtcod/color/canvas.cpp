
#include "canvas.h"

#include <stdexcept>

#include "../../vendor/lodepng.h"

namespace tcod {
namespace image {
Image load(const std::string& filename)
{
  unsigned img_width, img_height;
  std::vector<unsigned char> img_data;
  if (lodepng::decode(img_data, img_width, img_height, filename)) {
    throw std::runtime_error("File not found: " + filename);
  }
  Image image(img_width, img_height);
  std::vector<unsigned char>::iterator img_iter = img_data.begin();
  for (int y = 0; y < image.height(); ++y) {
    for (int x = 0; x < image.width(); ++x) {
      image.at(x, y) = ColorRGBA{
          img_iter[0], img_iter[1], img_iter[2], img_iter[3]};
      img_iter += 4;
    }
  }
  return image;
}
} // namespace image
} // namespace tcod
