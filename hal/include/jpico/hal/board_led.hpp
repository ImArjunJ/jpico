#pragma once

#include <jpico/result.hpp>
#include <jpico/types.hpp>

#include "pico/stdlib.h"

#if defined(CYW43_WL_GPIO_LED_PIN)
#include "pico/cyw43_arch.h"
#endif

namespace jpico::hal {

class board_led {
 public:
  board_led() = default;

  ~board_led() {
#if defined(CYW43_WL_GPIO_LED_PIN)
    if (initialized_) cyw43_arch_deinit();
#endif
  }

  board_led(const board_led&) = delete;
  board_led& operator=(const board_led&) = delete;

  result<void> init() {
#if defined(CYW43_WL_GPIO_LED_PIN)
    if (cyw43_arch_init() != 0) {
      return fail(error_code::hardware_fault, "cyw43 init failed");
    }
    initialized_ = true;
#elif defined(PICO_DEFAULT_LED_PIN)
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif
    return ok();
  }

  void set(bool on) {
#if defined(CYW43_WL_GPIO_LED_PIN)
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
#elif defined(PICO_DEFAULT_LED_PIN)
    gpio_put(PICO_DEFAULT_LED_PIN, on);
#endif
  }

  void on() { set(true); }
  void off() { set(false); }
  void toggle() {
    state_ = !state_;
    set(state_);
  }

 private:
  bool initialized_ = false;
  bool state_ = false;
};

}  // namespace jpico::hal
