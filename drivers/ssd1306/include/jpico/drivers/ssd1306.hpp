#pragma once
#include <jpico/core.hpp>
#include <jpico/hal/i2c_bus.hpp>

namespace jpico::drivers {

namespace ssd1306_cmd {
inline constexpr u8 SET_CONTRAST = 0x81;
inline constexpr u8 DISPLAY_ALL_ON_RESUME = 0xA4;
inline constexpr u8 DISPLAY_ALL_ON = 0xA5;
inline constexpr u8 NORMAL_DISPLAY = 0xA6;
inline constexpr u8 INVERT_DISPLAY = 0xA7;
inline constexpr u8 DISPLAY_OFF = 0xAE;
inline constexpr u8 DISPLAY_ON = 0xAF;
inline constexpr u8 SET_DISPLAY_OFFSET = 0xD3;
inline constexpr u8 SET_COM_PINS = 0xDA;
inline constexpr u8 SET_VCOM_DETECT = 0xDB;
inline constexpr u8 SET_DISPLAY_CLOCK_DIV = 0xD5;
inline constexpr u8 SET_PRECHARGE = 0xD9;
inline constexpr u8 SET_MULTIPLEX = 0xA8;
inline constexpr u8 SET_LOW_COLUMN = 0x00;
inline constexpr u8 SET_HIGH_COLUMN = 0x10;
inline constexpr u8 SET_START_LINE = 0x40;
inline constexpr u8 MEMORY_MODE = 0x20;
inline constexpr u8 COLUMN_ADDR = 0x21;
inline constexpr u8 PAGE_ADDR = 0x22;
inline constexpr u8 COM_SCAN_INC = 0xC0;
inline constexpr u8 COM_SCAN_DEC = 0xC8;
inline constexpr u8 SEG_REMAP = 0xA0;
inline constexpr u8 CHARGE_PUMP = 0x8D;
inline constexpr u8 DEACTIVATE_SCROLL = 0x2E;
}  // namespace ssd1306_cmd

class ssd1306 {
 public:
  static constexpr u8 DEFAULT_ADDR = 0x3C;
  static constexpr u16 WIDTH_128 = 128;
  static constexpr u16 HEIGHT_64 = 64;
  static constexpr u16 HEIGHT_32 = 32;

  ssd1306(hal::i2c_bus& i2c, u16 w = WIDTH_128, u16 h = HEIGHT_64,
          u8 addr = DEFAULT_ADDR)
      : i2c_{i2c}, addr_{addr}, width_{w}, height_{h} {}

  ~ssd1306() = default;

  ssd1306(const ssd1306&) = delete;
  ssd1306& operator=(const ssd1306&) = delete;

  result<void> init();

  u16 width() const { return width_; }
  u16 height() const { return height_; }

  void fill(u16 color);
  void pixel(u16 x, u16 y, u16 color);
  void blit(u16 x, u16 y, u16 w, u16 h, const u16* data);

  void flush();
  void set_contrast(u8 contrast);
  void invert(bool inv);

 private:
  void send_cmd(u8 cmd);
  void send_cmd_list(const u8* cmds, usize len);
  void send_buf(const u8* data, usize len);

  hal::i2c_bus& i2c_;
  u8 addr_;
  u16 width_;
  u16 height_;

  u8 buffer_[WIDTH_128 * HEIGHT_64 / 8] = {};
};

static_assert(display<ssd1306>);

}  // namespace jpico::drivers
