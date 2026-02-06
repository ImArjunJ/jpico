#include <jpico/drivers/ili9341.hpp>
#include <jpico/log.hpp>

#include "pico/stdlib.h"

namespace jpico::drivers {

static const u8 init_commands[] = {24,
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

result<void> ili9341::init() {
  spi_.set_format(16, SPI_CPOL_1, SPI_CPHA_1);
  cs_.low();

  hw_reset();

  const u8* addr = init_commands;
  u8 num_commands = *(addr++);

  while (num_commands--) {
    u8 cmd = *(addr++);
    u8 x = *(addr++);
    u8 num_args = x & 0x7F;
    send_command(cmd, addr, num_args);
    addr += num_args;
    if (x & 0x80) sleep_ms(150);
  }

  width_ = NATIVE_WIDTH;
  height_ = NATIVE_HEIGHT;

  log::info("ili9341 initialized (%dx%d)", width_, height_);
  return ok();
}

void ili9341::hw_reset() {
  rst_.low();
  sleep_ms(5);
  rst_.high();
  sleep_ms(150);
}

void ili9341::set_rotation(u8 rotation) {
  rotation_ = rotation % 4;
  u8 m;

  switch (rotation_) {
    case 0:
      m = madctl::MX | madctl::BGR;
      width_ = NATIVE_WIDTH;
      height_ = NATIVE_HEIGHT;
      break;
    case 1:
      m = madctl::MV | madctl::BGR;
      width_ = NATIVE_HEIGHT;
      height_ = NATIVE_WIDTH;
      break;
    case 2:
      m = madctl::MY | madctl::BGR;
      width_ = NATIVE_WIDTH;
      height_ = NATIVE_HEIGHT;
      break;
    case 3:
      m = madctl::MX | madctl::MY | madctl::MV | madctl::BGR;
      width_ = NATIVE_HEIGHT;
      height_ = NATIVE_WIDTH;
      break;
    default:
      m = madctl::MX | madctl::BGR;
      break;
  }

  send_command(ili9341_cmd::MADCTL, &m, 1);
}

void ili9341::fill(u16 color) {
  cs_.low();
  set_addr_window(0, 0, width_, height_);
  dc_.high();
  spi_.set_format(16, SPI_CPOL_1, SPI_CPHA_1);

  constexpr usize row_size = 320;
  u16 row_buffer[row_size];
  for (usize i = 0; i < row_size; i++) row_buffer[i] = color;

  u32 total = static_cast<u32>(width_) * height_;
  while (total > 0) {
    usize chunk = (total > row_size) ? row_size : total;
    spi_write16_blocking(spi_.instance(), row_buffer, chunk);
    total -= chunk;
  }
  cs_.high();
}

void ili9341::pixel(u16 x, u16 y, u16 color) {
  cs_.low();
  set_addr_window(x, y, 1, 1);
  dc_.high();
  spi_.set_format(16, SPI_CPOL_1, SPI_CPHA_1);
  spi_write16_blocking(spi_.instance(), &color, 1);
  cs_.high();
}

void ili9341::blit(u16 x, u16 y, u16 w, u16 h, const u16* data) {
  cs_.low();
  set_addr_window(x, y, w, h);
  dc_.high();
  spi_.set_format(16, SPI_CPOL_1, SPI_CPHA_1);
  spi_write16_blocking(spi_.instance(), data, static_cast<usize>(w) * h);
  cs_.high();
}

void ili9341::write_command(u8 cmd) {
  dc_.low();
  spi_.set_format(8, SPI_CPOL_1, SPI_CPHA_1);
  spi_write_blocking(spi_.instance(), &cmd, 1);
}

void ili9341::write_data(const u8* data, usize len) {
  dc_.high();
  spi_.set_format(8, SPI_CPOL_1, SPI_CPHA_1);
  spi_write_blocking(spi_.instance(), data, len);
}

void ili9341::send_command(u8 cmd, const u8* data, u8 len) {
  cs_.low();
  write_command(cmd);
  write_data(data, len);
  cs_.high();
}

void ili9341::set_addr_window(u16 x, u16 y, u16 w, u16 h) {
  u32 xa = ((u32)x << 16) | (x + w - 1);
  u32 ya = ((u32)y << 16) | (y + h - 1);
  xa = __builtin_bswap32(xa);
  ya = __builtin_bswap32(ya);

  write_command(ili9341_cmd::CASET);
  write_data(reinterpret_cast<const u8*>(&xa), sizeof(xa));

  write_command(ili9341_cmd::PASET);
  write_data(reinterpret_cast<const u8*>(&ya), sizeof(ya));

  write_command(ili9341_cmd::RAMWR);
}

}  // namespace jpico::drivers
