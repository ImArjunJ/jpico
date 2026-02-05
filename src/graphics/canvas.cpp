#include "graphics/canvas.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace jpico {
namespace graphics {

Canvas::Canvas(drivers::ILI9341Display& display) : display_(display) {}

Canvas::~Canvas() { destroy_framebuffer(); }

void Canvas::create_framebuffer() {
  if (!framebuffer_) {
    size_t size = static_cast<size_t>(get_width()) * get_height();
    framebuffer_ = std::make_unique<uint16_t[]>(size);
    clear();
  }
}

void Canvas::destroy_framebuffer() { framebuffer_.reset(); }

void Canvas::flush() {
  if (framebuffer_ && framebuffer_dirty_) {
    display_.write_bitmap(0, 0, get_width(), get_height(), framebuffer_.get());
    framebuffer_dirty_ = false;
  }
}

void Canvas::clear() { fill(clear_color_); }

void Canvas::fill(uint16_t color) {
  if (framebuffer_) {
    size_t size = static_cast<size_t>(get_width()) * get_height();
    for (size_t i = 0; i < size; ++i) {
      framebuffer_[i] = color;
    }
    framebuffer_dirty_ = true;
  } else {
    display_.fill_screen(color);
  }
}

void Canvas::set_clear_color(uint16_t color) { clear_color_ = color; }

void Canvas::draw_pixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || x >= get_width() || y < 0 || y >= get_height()) {
    return;
  }

  if (framebuffer_) {
    framebuffer_[y * get_width() + x] = color;
    framebuffer_dirty_ = true;
  } else {
    display_.write_pixel(x, y, color);
  }
}

void Canvas::draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                       uint16_t color) {
  bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);

  if (steep) {
    std::swap(x0, y0);
    std::swap(x1, y1);
  }

  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  int16_t dx = x1 - x0;
  int16_t dy = std::abs(y1 - y0);
  int16_t err = dx / 2;
  int16_t ystep = (y0 < y1) ? 1 : -1;

  for (; x0 <= x1; x0++) {
    if (steep) {
      draw_pixel(y0, x0, color);
    } else {
      draw_pixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void Canvas::draw_hline(int16_t x, int16_t y, int16_t length, uint16_t color) {
  if (y < 0 || y >= get_height()) return;

  int16_t x_start = std::max<int16_t>(0, x);
  int16_t x_end = std::min<int16_t>(get_width() - 1, x + length - 1);

  if (x_start > x_end) return;

  if (framebuffer_) {
    for (int16_t i = x_start; i <= x_end; ++i) {
      framebuffer_[y * get_width() + i] = color;
    }
    framebuffer_dirty_ = true;
  } else {
    for (int16_t i = x_start; i <= x_end; ++i) {
      display_.write_pixel(i, y, color);
    }
  }
}

void Canvas::draw_vline(int16_t x, int16_t y, int16_t height, uint16_t color) {
  if (x < 0 || x >= get_width()) return;

  int16_t y_start = std::max<int16_t>(0, y);
  int16_t y_end = std::min<int16_t>(get_height() - 1, y + height - 1);

  if (y_start > y_end) return;

  if (framebuffer_) {
    for (int16_t i = y_start; i <= y_end; ++i) {
      framebuffer_[i * get_width() + x] = color;
    }
    framebuffer_dirty_ = true;
  } else {
    for (int16_t i = y_start; i <= y_end; ++i) {
      display_.write_pixel(x, i, color);
    }
  }
}

void Canvas::draw_rect(int16_t x, int16_t y, int16_t w, int16_t h,
                       uint16_t color) {
  draw_hline(x, y, w, color);
  draw_hline(x, y + h - 1, w, color);
  draw_vline(x, y, h, color);
  draw_vline(x + w - 1, y, h, color);
}

void Canvas::fill_rect(int16_t x, int16_t y, int16_t w, int16_t h,
                       uint16_t color) {
  int16_t x_start = std::max<int16_t>(0, x);
  int16_t y_start = std::max<int16_t>(0, y);
  int16_t x_end = std::min<int16_t>(get_width() - 1, x + w - 1);
  int16_t y_end = std::min<int16_t>(get_height() - 1, y + h - 1);

  if (x_start > x_end || y_start > y_end) return;

  if (framebuffer_) {
    for (int16_t j = y_start; j <= y_end; ++j) {
      for (int16_t i = x_start; i <= x_end; ++i) {
        framebuffer_[j * get_width() + i] = color;
      }
    }
    framebuffer_dirty_ = true;
  } else {
    for (int16_t j = y_start; j <= y_end; ++j) {
      for (int16_t i = x_start; i <= x_end; ++i) {
        display_.write_pixel(i, j, color);
      }
    }
  }
}

void Canvas::draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  draw_pixel(x0, y0 + r, color);
  draw_pixel(x0, y0 - r, color);
  draw_pixel(x0 + r, y0, color);
  draw_pixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    draw_pixel(x0 + x, y0 + y, color);
    draw_pixel(x0 - x, y0 + y, color);
    draw_pixel(x0 + x, y0 - y, color);
    draw_pixel(x0 - x, y0 - y, color);
    draw_pixel(x0 + y, y0 + x, color);
    draw_pixel(x0 - y, y0 + x, color);
    draw_pixel(x0 + y, y0 - x, color);
    draw_pixel(x0 - y, y0 - x, color);
  }
}

void Canvas::fill_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  draw_vline(x0, y0 - r, 2 * r + 1, color);

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    draw_vline(x0 + x, y0 - y, 2 * y + 1, color);
    draw_vline(x0 - x, y0 - y, 2 * y + 1, color);
    draw_vline(x0 + y, y0 - x, 2 * x + 1, color);
    draw_vline(x0 - y, y0 - x, 2 * x + 1, color);
  }
}

void Canvas::set_cursor(int16_t x, int16_t y) {
  cursor_x_ = x;
  cursor_y_ = y;
}

void Canvas::set_text_color(uint16_t color) { text_color_ = color; }

void Canvas::set_text_background(uint16_t color) { text_bg_color_ = color; }

void Canvas::set_font(const Font* font) { font_ = font; }

void Canvas::draw_char(int16_t x, int16_t y, char c, uint16_t color,
                       uint16_t bg, uint8_t size_x, uint8_t size_y) {
  if (font_) {
    uint8_t first = font_->first;
    uint8_t last = font_->last;

    if ((c < first) || (c > last)) {
      return;
    }

    const Glyph* glyph = &font_->glyph[c - first];
    const uint8_t* bitmap = font_->bitmap;

    uint16_t bo = glyph->bitmap_offset;
    uint8_t w = glyph->width;
    uint8_t h = glyph->height;
    int8_t xo = glyph->x_offset;
    int8_t yo = glyph->y_offset;
    uint8_t bit = 0;
    uint8_t bits = 0;

    for (uint8_t yy = 0; yy < h; yy++) {
      for (uint8_t xx = 0; xx < w; xx++) {
        if (!(bit++ & 7)) {
          bits = bitmap[bo++];
        }
        if (bits & 0x80) {
          if (size_x == 1 && size_y == 1) {
            draw_pixel(x + xo + xx, y + yo + yy, color);
          } else {
            fill_rect(x + (xo + xx) * size_x, y + (yo + yy) * size_y, size_x,
                      size_y, color);
          }
        }
        bits <<= 1;
      }
    }
  } else {
    static const uint8_t font5x7[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x07,
        0x00, 0x07, 0x00, 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x24, 0x2A, 0x7F, 0x2A,
        0x12, 0x23, 0x13, 0x08, 0x64, 0x62, 0x36, 0x49, 0x55, 0x22, 0x50, 0x00,
        0x05, 0x03, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, 0x41, 0x22,
        0x1C, 0x00, 0x08, 0x2A, 0x1C, 0x2A, 0x08, 0x08, 0x08, 0x3E, 0x08, 0x08,
        0x00, 0x50, 0x30, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x60,
        0x60, 0x00, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x3E, 0x51, 0x49, 0x45,
        0x3E, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x42, 0x61, 0x51, 0x49, 0x46, 0x21,
        0x41, 0x45, 0x4B, 0x31, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x27, 0x45, 0x45,
        0x45, 0x39, 0x3C, 0x4A, 0x49, 0x49, 0x30, 0x01, 0x71, 0x09, 0x05, 0x03,
        0x36, 0x49, 0x49, 0x49, 0x36, 0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x36,
        0x36, 0x00, 0x00, 0x00, 0x56, 0x36, 0x00, 0x00, 0x00, 0x08, 0x14, 0x22,
        0x41, 0x14, 0x14, 0x14, 0x14, 0x14, 0x41, 0x22, 0x14, 0x08, 0x00, 0x02,
        0x01, 0x51, 0x09, 0x06, 0x32, 0x49, 0x79, 0x41, 0x3E, 0x7E, 0x11, 0x11,
        0x11, 0x7E, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x3E, 0x41, 0x41, 0x41, 0x22,
        0x7F, 0x41, 0x41, 0x22, 0x1C, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x7F, 0x09,
        0x09, 0x01, 0x01, 0x3E, 0x41, 0x41, 0x51, 0x32, 0x7F, 0x08, 0x08, 0x08,
        0x7F, 0x00, 0x41, 0x7F, 0x41, 0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, 0x7F,
        0x08, 0x14, 0x22, 0x41, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x7F, 0x02, 0x04,
        0x02, 0x7F, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x3E, 0x41, 0x41, 0x41, 0x3E,
        0x7F, 0x09, 0x09, 0x09, 0x06, 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x7F, 0x09,
        0x19, 0x29, 0x46, 0x46, 0x49, 0x49, 0x49, 0x31, 0x01, 0x01, 0x7F, 0x01,
        0x01, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x7F,
        0x20, 0x18, 0x20, 0x7F, 0x63, 0x14, 0x08, 0x14, 0x63, 0x03, 0x04, 0x78,
        0x04, 0x03, 0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x00, 0x7F, 0x41, 0x41,
        0x02, 0x04, 0x08, 0x10, 0x20, 0x41, 0x41, 0x7F, 0x00, 0x00, 0x04, 0x02,
        0x01, 0x02, 0x04, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x01, 0x02, 0x04,
        0x00, 0x20, 0x54, 0x54, 0x54, 0x78, 0x7F, 0x48, 0x44, 0x44, 0x38, 0x38,
        0x44, 0x44, 0x44, 0x20, 0x38, 0x44, 0x44, 0x48, 0x7F, 0x38, 0x54, 0x54,
        0x54, 0x18, 0x08, 0x7E, 0x09, 0x01, 0x02, 0x08, 0x14, 0x54, 0x54, 0x3C,
        0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x44, 0x7D, 0x40, 0x00, 0x20, 0x40,
        0x44, 0x3D, 0x00, 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x41, 0x7F, 0x40,
        0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, 0x7C, 0x08, 0x04, 0x04, 0x78, 0x38,
        0x44, 0x44, 0x44, 0x38, 0x7C, 0x14, 0x14, 0x14, 0x08, 0x08, 0x14, 0x14,
        0x18, 0x7C, 0x7C, 0x08, 0x04, 0x04, 0x08, 0x48, 0x54, 0x54, 0x54, 0x20,
        0x04, 0x3F, 0x44, 0x40, 0x20, 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x1C, 0x20,
        0x40, 0x20, 0x1C, 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x44, 0x28, 0x10, 0x28,
        0x44, 0x0C, 0x50, 0x50, 0x50, 0x3C, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00,
        0x08, 0x36, 0x41, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x41, 0x36,
        0x08, 0x00, 0x08, 0x08, 0x2A, 0x1C, 0x08,
    };

    if (c < 32 || c > 126) return;

    for (uint8_t i = 0; i < 5; i++) {
      uint8_t line = font5x7[(c - 32) * 5 + i];
      for (uint8_t j = 0; j < 8; j++) {
        if (line & 0x01) {
          if (size_x == 1 && size_y == 1) {
            draw_pixel(x + i, y + j, color);
          } else {
            fill_rect(x + i * size_x, y + j * size_y, size_x, size_y, color);
          }
        } else if (bg != color) {
          if (size_x == 1 && size_y == 1) {
            draw_pixel(x + i, y + j, bg);
          } else {
            fill_rect(x + i * size_x, y + j * size_y, size_x, size_y, bg);
          }
        }
        line >>= 1;
      }
    }
  }
}

void Canvas::write_char(char c) {
  if (c == '\n') {
    cursor_x_ = 0;
    cursor_y_ += (font_ ? font_->y_advance : 8) * text_size_y_;
  } else if (c == '\r') {
    cursor_x_ = 0;
  } else {
    uint8_t char_width = font_ ? font_->glyph[c - font_->first].x_advance : 6;

    if (text_wrap_ && (cursor_x_ + char_width * text_size_x_ > get_width())) {
      cursor_x_ = 0;
      cursor_y_ += (font_ ? font_->y_advance : 8) * text_size_y_;
    }

    draw_char(cursor_x_, cursor_y_, c, text_color_, text_bg_color_,
              text_size_x_, text_size_y_);
    cursor_x_ += char_width * text_size_x_;
  }
}

void Canvas::print(const char* str) {
  while (*str) {
    write_char(*str++);
  }
}

void Canvas::printf(const char* format, ...) {
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  print(buffer);
}

void Canvas::scroll_up(int16_t pixels) {
  if (!framebuffer_) return;

  uint16_t width = get_width();
  uint16_t height = get_height();

  for (int16_t y = 0; y < height - pixels; y++) {
    memcpy(&framebuffer_[y * width], &framebuffer_[(y + pixels) * width],
           width * sizeof(uint16_t));
  }

  for (int16_t y = height - pixels; y < height; y++) {
    for (int16_t x = 0; x < width; x++) {
      framebuffer_[y * width + x] = clear_color_;
    }
  }

  framebuffer_dirty_ = true;
}

}  // namespace graphics
}  // namespace jpico
