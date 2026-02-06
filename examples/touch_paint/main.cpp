#include <jpico/core.hpp>
#include <jpico/drivers/ili9341.hpp>
#include <jpico/drivers/xpt2046.hpp>
#include <jpico/graphics/canvas.hpp>
#include <jpico/hal/hal.hpp>

using namespace jpico;

int main() {
  stdio_init_all();
  sleep_ms(2000);

  // display and touch share the same spi bus
  hal::spi_bus spi(spi0, {
                             .baudrate = 40'000'000,
                             .pin_sck = 6,
                             .pin_tx = 7,
                             .pin_rx = 4,
                             .cpol = SPI_CPOL_1,
                             .cpha = SPI_CPHA_1,
                         });

  hal::output_pin lcd_cs(13, true);
  hal::output_pin dc(15);
  hal::output_pin rst(14, true);

  hal::output_pin touch_cs(12, true);

  drivers::ili9341 display(spi, lcd_cs, dc, rst);
  drivers::xpt2046 touch(spi, touch_cs);

  auto r = display.init();
  if (!r) {
    log::error("display init failed: %s", r.error().message);
    while (true) sleep_ms(1000);
  }

  display.set_rotation(1);

  auto t = touch.init();
  if (!t) {
    log::error("touch init failed: %s", t.error().message);
    while (true) sleep_ms(1000);
  }

  touch.set_screen_size(display.width(), display.height());

  graphics::canvas canvas(display);

  canvas.set_clear_color(colors::black.raw);
  canvas.clear();

  canvas.set_text_color(colors::white.raw);
  canvas.set_text_size(2);
  canvas.set_cursor(5, 5);
  canvas.print("touch to draw");

  constexpr u16 palette[] = {
      colors::white.raw,  colors::red.raw,    colors::green.raw,
      colors::blue.raw,   colors::cyan.raw,   colors::magenta.raw,
      colors::yellow.raw, colors::orange.raw,
  };
  constexpr u16 palette_count = sizeof(palette) / sizeof(palette[0]);
  u16 bar_w = display.width() / (palette_count + 1);  // +1 for CLR button
  u16 bar_y = display.height() - 20;

  auto draw_toolbar = [&]() {
    for (u16 i = 0; i < palette_count; ++i) {
      canvas.fill_rect(i * bar_w, bar_y, bar_w, 20, palette[i]);
    }
    canvas.fill_rect(palette_count * bar_w, bar_y,
                     display.width() - palette_count * bar_w, 20,
                     colors::dark_gray.raw);
    canvas.set_text_color(colors::white.raw);
    canvas.set_text_size(1);
    canvas.set_cursor(palette_count * bar_w + 4, bar_y + 6);
    canvas.print("CLR");
  };

  draw_toolbar();

  u16 brush_color = colors::white.raw;
  constexpr i16 brush_size = 5;

  while (true) {
    u16 z = touch.pressure();

    if (z > 30) {
      auto p = touch.read();

      if (p.x >= 0 && p.x < display.width() && p.y >= 0 &&
          p.y < display.height()) {
        if (p.y >= bar_y) {
          u16 idx = p.x / bar_w;
          if (idx < palette_count) {
            brush_color = palette[idx];
          } else {
            canvas.clear();
            draw_toolbar();
          }
        } else {
          canvas.fill_rect(p.x - brush_size / 2, p.y - brush_size / 2,
                           brush_size, brush_size, brush_color);
        }
      }
    }

    sleep_ms(10);
  }
}
