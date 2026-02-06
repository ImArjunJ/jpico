#include <jpico/core.hpp>
#include <jpico/drivers/ssd1306.hpp>
#include <jpico/graphics/canvas.hpp>
#include <jpico/hal/hal.hpp>

#include "pico/stdlib.h"

using namespace jpico;

int main() {
  stdio_init_all();
  sleep_ms(2000);

  hal::i2c_bus i2c(i2c0, {
                             .baudrate = 400'000,
                             .pin_sda = 4,
                             .pin_scl = 5,
                         });

  drivers::ssd1306 oled(i2c, drivers::ssd1306::WIDTH_128,
                        drivers::ssd1306::HEIGHT_32);

  auto r = oled.init();
  if (!r) {
    log::error("oled init failed: %s", r.error().message);
    while (true) sleep_ms(1000);
  }

  graphics::canvas canvas(oled);
  canvas.set_clear_color(0x0000);
  canvas.clear();

  canvas.set_text_color(0xFFFF);
  canvas.set_text_size(2);
  canvas.print("hello\njpico!");
  oled.flush();

  while (true) {
    sleep_ms(100);
  }
}
