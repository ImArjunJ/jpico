#include <jpico/core.hpp>
#include <jpico/hal/gpio.hpp>

int main() {
  stdio_init_all();

  jpico::hal::output_pin led(28);

  while (true) {
    led.toggle();
    sleep_ms(500);
  }
}
