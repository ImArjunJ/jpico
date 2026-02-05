#pragma once

#include <cstdint>

namespace jpico {
namespace graphics {

struct Glyph {
  uint16_t bitmap_offset;
  uint8_t width;
  uint8_t height;
  uint8_t x_advance;
  int8_t x_offset;
  int8_t y_offset;
};

struct Font {
  const uint8_t* bitmap;
  const Glyph* glyph;  // Changed from glyphs to glyph
  uint8_t first;       // Changed from first_char to first
  uint8_t last;        // Changed from last_char to last
  uint8_t y_advance;
};

}  // namespace graphics
}  // namespace jpico
