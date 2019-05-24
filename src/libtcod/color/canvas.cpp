/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "canvas.h"

#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>

#include "../../vendor/lodepng.h"

namespace tcod {
namespace image {
/**
 *  Return true if the file at `path` exists.
 */
static bool file_exists(const std::string& path)
{
  struct stat buffer;
  return stat(path.c_str(), &buffer) == 0;
}
Image load(const std::string& filename)
{
  if (!file_exists(filename)) {
    throw std::runtime_error("File does not exist: " + filename);
  }
  unsigned img_width, img_height;
  std::vector<unsigned char> img_data;
  if (auto err=lodepng::decode(img_data, img_width, img_height, filename)) {
    throw std::runtime_error(lodepng_error_text(err));
  }
  Image image(img_width, img_height);
  std::vector<unsigned char>::iterator img_iter = img_data.begin();
  for (int y = 0; y < image.height(); ++y) {
    for (int x = 0; x < image.width(); ++x) {
      image.atf(x, y) = ColorRGBA{
          img_iter[0], img_iter[1], img_iter[2], img_iter[3]};
      img_iter += 4;
    }
  }
  return image;
}
void save(const Image& image, const std::string& filename)
{
  const unsigned char* buffer =
      reinterpret_cast<const unsigned char*>(image.data());
  if (auto err=lodepng::encode(filename, buffer,
                               image.width(), image.height())) {
    throw std::runtime_error(lodepng_error_text(err));
  }
}
} // namespace image
} // namespace tcod
