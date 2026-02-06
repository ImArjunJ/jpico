#include <jpico/core.hpp>
#include <jpico/drivers/ili9341.hpp>
#include <jpico/graphics/canvas.hpp>
#include <jpico/hal/hal.hpp>

#include "pico/stdlib.h"

using namespace jpico;

int main() {
  stdio_init_all();
  sleep_ms(2000);

  hal::spi_bus spi(spi0, {
                             .baudrate = 40'000'000,
                             .pin_sck = 6,
                             .pin_tx = 7,
                             .cpol = SPI_CPOL_1,
                             .cpha = SPI_CPHA_1,
                         });

  hal::output_pin cs(13, true);
  hal::output_pin dc(15);
  hal::output_pin rst(14, true);

  drivers::ili9341 display(spi, cs, dc, rst);

  auto r = display.init();
  if (!r) {
    log::error("display init failed: %s", r.error().message);
    while (true) sleep_ms(1000);
  }

  display.set_rotation(1);

  graphics::canvas canvas(display);
  canvas.create_framebuffer();
  canvas.set_clear_color(colors::black.raw);
  canvas.clear();

  canvas.set_text_color(colors::cyan.raw);
  canvas.print("hello, jpico!\n");
  canvas.set_text_color(colors::green.raw);
  canvas.printf("display: %dx%d\n", display.width(), display.height());
  canvas.flush();

  while (true) {
    sleep_ms(100);
  }
}
