#pragma once

#include <cstdint>

#include "hardware/spi.h"
#include "pico/stdlib.h"

namespace jpico {
namespace drivers {

namespace ili9341_cmd {
constexpr uint8_t NOP = 0x00;
constexpr uint8_t SWRESET = 0x01;
constexpr uint8_t SLPOUT = 0x11;
constexpr uint8_t DISPON = 0x29;
constexpr uint8_t CASET = 0x2A;
constexpr uint8_t PASET = 0x2B;
constexpr uint8_t RAMWR = 0x2C;
constexpr uint8_t MADCTL = 0x36;
constexpr uint8_t VSCRSADD = 0x37;
constexpr uint8_t PIXFMT = 0x3A;
constexpr uint8_t FRMCTR1 = 0xB1;
constexpr uint8_t DFUNCTR = 0xB6;
constexpr uint8_t PWCTR1 = 0xC0;
constexpr uint8_t PWCTR2 = 0xC1;
constexpr uint8_t VMCTR1 = 0xC5;
constexpr uint8_t VMCTR2 = 0xC7;
constexpr uint8_t GMCTRP1 = 0xE0;
constexpr uint8_t GMCTRN1 = 0xE1;
constexpr uint8_t GAMMASET = 0x26;
}  // namespace ili9341_cmd

namespace madctl {
constexpr uint8_t MY = 0x80;
constexpr uint8_t MX = 0x40;
constexpr uint8_t MV = 0x20;
constexpr uint8_t ML = 0x10;
constexpr uint8_t BGR = 0x08;
constexpr uint8_t MH = 0x04;
}  // namespace madctl

namespace color {
constexpr uint16_t BLACK = 0x0000;
constexpr uint16_t WHITE = 0xFFFF;
constexpr uint16_t RED = 0xF800;
constexpr uint16_t GREEN = 0x07E0;
constexpr uint16_t BLUE = 0x001F;
constexpr uint16_t CYAN = 0x07FF;
constexpr uint16_t MAGENTA = 0xF81F;
constexpr uint16_t YELLOW = 0xFFE0;
constexpr uint16_t ORANGE = 0xFC00;

constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
}  // namespace color

struct DisplayConfig {
  uint16_t pin_dc;
  uint16_t pin_cs;
  int16_t pin_rst;
  uint16_t pin_sck;
  uint16_t pin_tx;
  spi_inst_t* spi = spi_default;
};

class ILI9341Display {
 public:
  static constexpr uint16_t WIDTH = 240;
  static constexpr uint16_t HEIGHT = 320;

  ILI9341Display() = default;

  ILI9341Display(uint8_t dc, uint8_t cs, int8_t rst, uint8_t sck, uint8_t mosi,
                 spi_inst_t* spi = spi_default);

  ~ILI9341Display() = default;

  ILI9341Display(const ILI9341Display&) = delete;
  ILI9341Display& operator=(const ILI9341Display&) = delete;

  bool init();
  void init(const DisplayConfig& config);

  void set_rotation(uint8_t rotation);

  void fill_screen(uint16_t color);

  void write_pixel(int16_t x, int16_t y, uint16_t color);

  void write_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                    const uint16_t* data);

  uint16_t get_width() const { return width_; }
  uint16_t get_height() const { return height_; }

 private:
  void init_spi();
  void reset();
  void select();
  void deselect();
  void set_command_mode();
  void set_data_mode();
  void write_command(uint8_t cmd);
  void write_data(const uint8_t* data, size_t len);
  void send_command(uint8_t cmd, const uint8_t* data, uint8_t len);
  void set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

  DisplayConfig config_{};
  uint16_t width_ = WIDTH;
  uint16_t height_ = HEIGHT;
  uint8_t rotation_ = 0;
  bool initialized_ = false;
};

}  // namespace drivers
}  // namespace jpico
