#include "drivers/ili9341.hpp"

#include <cstring>

#include "hardware/dma.h"

namespace jpico {
namespace drivers {

static const uint8_t init_commands[] = {24,
                                        0xEF,
                                        3,
                                        0x03,
                                        0x80,
                                        0x02,
                                        0xCF,
                                        3,
                                        0x00,
                                        0xC1,
                                        0x30,
                                        0xED,
                                        4,
                                        0x64,
                                        0x03,
                                        0x12,
                                        0x81,
                                        0xE8,
                                        3,
                                        0x85,
                                        0x00,
                                        0x78,
                                        0xCB,
                                        5,
                                        0x39,
                                        0x2C,
                                        0x00,
                                        0x34,
                                        0x02,
                                        0xF7,
                                        1,
                                        0x20,
                                        0xEA,
                                        2,
                                        0x00,
                                        0x00,
                                        ili9341_cmd::PWCTR1,
                                        1,
                                        0x23,
                                        ili9341_cmd::PWCTR2,
                                        1,
                                        0x10,
                                        ili9341_cmd::VMCTR1,
                                        2,
                                        0x3e,
                                        0x28,
                                        ili9341_cmd::VMCTR2,
                                        1,
                                        0x86,
                                        ili9341_cmd::MADCTL,
                                        1,
                                        0x48,
                                        ili9341_cmd::VSCRSADD,
                                        1,
                                        0x00,
                                        ili9341_cmd::PIXFMT,
                                        1,
                                        0x55,
                                        ili9341_cmd::FRMCTR1,
                                        2,
                                        0x00,
                                        0x18,
                                        ili9341_cmd::DFUNCTR,
                                        3,
                                        0x08,
                                        0x82,
                                        0x27,
                                        0xF2,
                                        1,
                                        0x00,
                                        ili9341_cmd::GAMMASET,
                                        1,
                                        0x01,
                                        ili9341_cmd::GMCTRP1,
                                        15,
                                        0x0F,
                                        0x31,
                                        0x2B,
                                        0x0C,
                                        0x0E,
                                        0x08,
                                        0x4E,
                                        0xF1,
                                        0x37,
                                        0x07,
                                        0x10,
                                        0x03,
                                        0x0E,
                                        0x09,
                                        0x00,
                                        ili9341_cmd::GMCTRN1,
                                        15,
                                        0x00,
                                        0x0E,
                                        0x14,
                                        0x03,
                                        0x11,
                                        0x07,
                                        0x31,
                                        0xC1,
                                        0x48,
                                        0x08,
                                        0x0F,
                                        0x0C,
                                        0x31,
                                        0x36,
                                        0x0F,
                                        ili9341_cmd::SLPOUT,
                                        0x80,
                                        ili9341_cmd::DISPON,
                                        0x80,
                                        0x00};

ILI9341Display::ILI9341Display(uint8_t dc, uint8_t cs, int8_t rst, uint8_t sck,
                               uint8_t mosi, spi_inst_t* spi) {
  config_.pin_dc = dc;
  config_.pin_cs = cs;
  config_.pin_rst = rst;
  config_.pin_sck = sck;
  config_.pin_tx = mosi;
  config_.spi = spi;
}

bool ILI9341Display::init() {
  init_spi();
  select();

  if (config_.pin_rst < 0) {
    write_command(ili9341_cmd::SWRESET);
    sleep_ms(150);
  } else {
    reset();
  }

  const uint8_t* addr = init_commands;
  uint8_t num_commands = *(addr++);

  while (num_commands--) {
    uint8_t cmd = *(addr++);
    uint8_t x = *(addr++);
    uint8_t num_args = x & 0x7F;
    send_command(cmd, addr, num_args);
    addr += num_args;
    if (x & 0x80) {
      sleep_ms(150);
    }
  }

  width_ = WIDTH;
  height_ = HEIGHT;
  initialized_ = true;
  return true;
}

void ILI9341Display::init(const DisplayConfig& config) {
  config_ = config;
  init_spi();
  select();

  if (config_.pin_rst < 0) {
    write_command(ili9341_cmd::SWRESET);
    sleep_ms(150);
  } else {
    reset();
  }

  const uint8_t* addr = init_commands;
  uint8_t num_commands = *(addr++);

  while (num_commands--) {
    uint8_t cmd = *(addr++);
    uint8_t x = *(addr++);
    uint8_t num_args = x & 0x7F;
    send_command(cmd, addr, num_args);
    addr += num_args;
    if (x & 0x80) {
      sleep_ms(150);
    }
  }

  width_ = WIDTH;
  height_ = HEIGHT;
  initialized_ = true;
}

void ILI9341Display::init_spi() {
  spi_init(config_.spi, 40 * 1000 * 1000);
  spi_set_format(config_.spi, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

  gpio_set_function(config_.pin_sck, GPIO_FUNC_SPI);
  gpio_set_function(config_.pin_tx, GPIO_FUNC_SPI);

  gpio_init(config_.pin_cs);
  gpio_set_dir(config_.pin_cs, GPIO_OUT);
  gpio_put(config_.pin_cs, 1);

  gpio_init(config_.pin_dc);
  gpio_set_dir(config_.pin_dc, GPIO_OUT);
  gpio_put(config_.pin_dc, 1);

  if (config_.pin_rst >= 0) {
    gpio_init(config_.pin_rst);
    gpio_set_dir(config_.pin_rst, GPIO_OUT);
    gpio_put(config_.pin_rst, 1);
  }
}

void ILI9341Display::reset() {
  if (config_.pin_rst >= 0) {
    gpio_put(config_.pin_rst, 0);
    sleep_ms(5);
    gpio_put(config_.pin_rst, 1);
    sleep_ms(150);
  }
}

void ILI9341Display::select() { gpio_put(config_.pin_cs, 0); }

void ILI9341Display::deselect() { gpio_put(config_.pin_cs, 1); }

void ILI9341Display::set_command_mode() { gpio_put(config_.pin_dc, 0); }

void ILI9341Display::set_data_mode() { gpio_put(config_.pin_dc, 1); }

void ILI9341Display::write_command(uint8_t cmd) {
  set_command_mode();
  spi_set_format(config_.spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
  spi_write_blocking(config_.spi, &cmd, 1);
}

void ILI9341Display::write_data(const uint8_t* data, size_t len) {
  set_data_mode();
  spi_set_format(config_.spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
  spi_write_blocking(config_.spi, data, len);
}

void ILI9341Display::send_command(uint8_t cmd, const uint8_t* data,
                                  uint8_t len) {
  select();
  write_command(cmd);
  write_data(data, len);
  deselect();
}

void ILI9341Display::set_rotation(uint8_t rotation) {
  rotation_ = rotation % 4;
  uint8_t m;

  switch (rotation_) {
    case 0:
      m = madctl::MX | madctl::BGR;
      width_ = WIDTH;
      height_ = HEIGHT;
      break;
    case 1:
      m = madctl::MV | madctl::BGR;
      width_ = HEIGHT;
      height_ = WIDTH;
      break;
    case 2:
      m = madctl::MY | madctl::BGR;
      width_ = WIDTH;
      height_ = HEIGHT;
      break;
    case 3:
      m = madctl::MX | madctl::MY | madctl::MV | madctl::BGR;
      width_ = HEIGHT;
      height_ = WIDTH;
      break;
    default:
      m = madctl::MX | madctl::BGR;
      break;
  }

  send_command(ili9341_cmd::MADCTL, &m, 1);
}

void ILI9341Display::set_addr_window(uint16_t x, uint16_t y, uint16_t w,
                                     uint16_t h) {
  uint32_t xa = ((uint32_t)x << 16) | (x + w - 1);
  uint32_t ya = ((uint32_t)y << 16) | (y + h - 1);

  xa = __builtin_bswap32(xa);
  ya = __builtin_bswap32(ya);

  write_command(ili9341_cmd::CASET);
  write_data(reinterpret_cast<const uint8_t*>(&xa), sizeof(xa));

  write_command(ili9341_cmd::PASET);
  write_data(reinterpret_cast<const uint8_t*>(&ya), sizeof(ya));

  write_command(ili9341_cmd::RAMWR);
}

void ILI9341Display::write_pixel(int16_t x, int16_t y, uint16_t color) {
  select();
  set_addr_window(x, y, 1, 1);
  set_data_mode();
  spi_set_format(config_.spi, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
  spi_write16_blocking(config_.spi, &color, 1);
  deselect();
}

void ILI9341Display::fill_screen(uint16_t color) {
  select();
  set_addr_window(0, 0, width_, height_);
  set_data_mode();
  spi_set_format(config_.spi, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

  constexpr size_t row_size = 320;
  uint16_t row_buffer[row_size];
  for (size_t i = 0; i < row_size; i++) {
    row_buffer[i] = color;
  }

  uint32_t total_pixels = static_cast<uint32_t>(width_) * height_;
  while (total_pixels > 0) {
    size_t chunk = (total_pixels > row_size) ? row_size : total_pixels;
    spi_write16_blocking(config_.spi, row_buffer, chunk);
    total_pixels -= chunk;
  }

  deselect();
}

void ILI9341Display::write_bitmap(uint16_t x, uint16_t y, uint16_t w,
                                  uint16_t h, const uint16_t* data) {
  select();
  set_addr_window(x, y, w, h);
  set_data_mode();
  spi_set_format(config_.spi, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
  spi_write16_blocking(config_.spi, data, w * h);
  deselect();
}

}  // namespace drivers
}  // namespace jpico
