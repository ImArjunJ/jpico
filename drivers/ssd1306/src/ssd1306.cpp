#include <cstring>
#include <jpico/drivers/ssd1306.hpp>
#include <jpico/log.hpp>

namespace jpico::drivers {

// i2c helpers — mirrors the rpi reference implementation:
// one command byte per i2c transaction, Co=1 D/C#=0 control byte

void ssd1306::send_cmd(u8 cmd) {
  u8 buf[2] = {0x80, cmd};  // Co=1, D/C#=0
  i2c_.write(addr_, buf);
}

void ssd1306::send_cmd_list(const u8* cmds, usize len) {
  for (usize i = 0; i < len; ++i) {
    send_cmd(cmds[i]);
  }
}

void ssd1306::send_buf(const u8* data, usize len) {
  // send entire framebuffer in one transaction — control byte 0x40 (Co=0,
  // D/C#=1 = data) followed by all pixel bytes, matching the reference impl
  u8 tmp[1 + WIDTH_128 * HEIGHT_64 / 8];  // 1025 max
  tmp[0] = 0x40;
  std::memcpy(&tmp[1], data, len);
  i2c_.write(addr_, std::span<const u8>(tmp, 1 + len));
}

// init — register values and order taken from the rpi reference that
// the user confirmed working on their 128x32 panel

result<void> ssd1306::init() {
  u8 com_pin_cfg = (height_ == HEIGHT_64) ? 0x12 : 0x02;

  u8 cmds[] = {
      0xAE,                                       // display off
      0x20,        0x00,                          // horizontal addressing mode
      0x40,                                       // start line 0
      0xA0 | 0x01,                                // seg remap, col 127 → SEG0
      0xA8,        static_cast<u8>(height_ - 1),  // mux ratio
      0xC0 | 0x08,                                // COM scan dec
      0xD3,        0x00,                          // display offset 0
      0xDA,        com_pin_cfg,                   // COM pins
      0xD5,        0x80,                          // clock div
      0xD9,        0xF1,                          // precharge
      0xDB,        0x30,                          // VCOM deselect
      0x81,        0xFF,                          // contrast
      0xA4,                                       // follow RAM content
      0xA6,                                       // normal (not inverted)
      0x8D,        0x14,                          // charge pump on
      0x2E,                                       // deactivate scroll
  };

  send_cmd_list(cmds, sizeof(cmds));

  // clear and push before turning on so there's no garbage flash
  fill(0);
  flush();

  send_cmd(0xAF);  // display on

  log::info("ssd1306 initialized (%dx%d)", width_, height_);
  return ok();
}

// display concept

void ssd1306::fill(u16 color) {
  usize buflen = static_cast<usize>(width_) * (height_ / 8);
  std::memset(buffer_, color ? 0xFF : 0x00, buflen);
}

void ssd1306::pixel(u16 x, u16 y, u16 color) {
  if (x >= width_ || y >= height_) return;

  usize idx = x + (y / 8) * width_;
  u8 bit = 1 << (y & 7);

  if (color)
    buffer_[idx] |= bit;
  else
    buffer_[idx] &= ~bit;
}

void ssd1306::blit(u16 x, u16 y, u16 w, u16 h, const u16* data) {
  for (u16 dy = 0; dy < h; ++dy)
    for (u16 dx = 0; dx < w; ++dx) pixel(x + dx, y + dy, data[dy * w + dx]);
}

// oled-specific

void ssd1306::flush() {
  usize buflen = static_cast<usize>(width_) * (height_ / 8);

  u8 cmds[] = {
      0x21, 0x00, static_cast<u8>(width_ - 1),         // column range
      0x22, 0x00, static_cast<u8>((height_ / 8) - 1),  // page range
  };
  send_cmd_list(cmds, sizeof(cmds));
  send_buf(buffer_, buflen);
}

void ssd1306::set_contrast(u8 contrast) {
  send_cmd(0x81);
  send_cmd(contrast);
}

void ssd1306::invert(bool inv) { send_cmd(inv ? 0xA7 : 0xA6); }

}  // namespace jpico::drivers
