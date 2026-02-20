#include <jpico/core.hpp>
#include <jpico/hal/board_led.hpp>
#include <jpico/hal/time.hpp>

using namespace jpico;

int main() {
  hal::init_stdio();
  hal::board_led led;
  auto r = led.init();
  if (!r) {
    log::error("led init failed: %s", r.error().message);
    return 1;
  }

  while (true) {
    led.toggle();
    hal::sleep(500);
  }
}
