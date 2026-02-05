#pragma once

#include <cstdarg>
#include <cstdint>
#include <memory>

#include "drivers/ili9341.hpp"
#include "graphics/font.hpp"

namespace jpico {
namespace graphics {

constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

class Canvas {
 public:
  Canvas(drivers::ILI9341Display& display);
  ~Canvas();

  Canvas(const Canvas&) = delete;
  Canvas& operator=(const Canvas&) = delete;

  void create_framebuffer();
  void destroy_framebuffer();
  void flush();

  void clear();
  void fill(uint16_t color);
  void set_clear_color(uint16_t color);

  void draw_pixel(int16_t x, int16_t y, uint16_t color);
  void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                 uint16_t color);
  void draw_hline(int16_t x, int16_t y, int16_t length, uint16_t color);
  void draw_vline(int16_t x, int16_t y, int16_t height, uint16_t color);
  void draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void draw_circle(int16_t x, int16_t y, int16_t r, uint16_t color);
  void fill_circle(int16_t x, int16_t y, int16_t r, uint16_t color);

  void set_cursor(int16_t x, int16_t y);
  void set_text_color(uint16_t color);
  void set_text_background(uint16_t color);
  void set_font(const Font* font);
  void draw_char(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg,
                 uint8_t size_x = 1, uint8_t size_y = 1);
  void write_char(char c);
  void print(const char* str);
  void printf(const char* format, ...);

  void scroll_up(int16_t pixels);

  uint16_t get_width() const { return display_.get_width(); }
  uint16_t get_height() const { return display_.get_height(); }
  int16_t get_cursor_x() const { return cursor_x_; }
  int16_t get_cursor_y() const { return cursor_y_; }

 private:
  drivers::ILI9341Display& display_;
  std::unique_ptr<uint16_t[]> framebuffer_;
  bool framebuffer_dirty_ = false;

  int16_t cursor_x_ = 0;
  int16_t cursor_y_ = 0;
  uint8_t text_size_x_ = 1;
  uint8_t text_size_y_ = 1;
  uint16_t text_color_ = 0xFFFF;
  uint16_t text_bg_color_ = 0x0000;
  uint16_t clear_color_ = 0x0000;
  bool text_wrap_ = true;
  const Font* font_ = nullptr;
};

}  // namespace graphics
}  // namespace jpico
