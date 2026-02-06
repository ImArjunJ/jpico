#pragma once

#include <jpico/types.hpp>

#include "pico/stdlib.h"

namespace jpico::hal {

class output_pin {
 public:
  explicit output_pin(u8 pin, bool initial = false) : pin_{pin} {
    gpio_init(pin_);
    gpio_set_dir(pin_, GPIO_OUT);
    gpio_put(pin_, initial);
  }

  void high() { gpio_put(pin_, true); }
  void low() { gpio_put(pin_, false); }
  void set(bool v) { gpio_put(pin_, v); }
  void toggle() { gpio_xor_mask(1u << pin_); }

  u8 pin() const { return pin_; }

  output_pin(const output_pin&) = delete;
  output_pin& operator=(const output_pin&) = delete;

 private:
  u8 pin_;
};

enum class pull : u8 { none, up, down };

class input_pin {
 public:
  explicit input_pin(u8 pin, pull p = pull::none) : pin_{pin} {
    gpio_init(pin_);
    gpio_set_dir(pin_, GPIO_IN);
    switch (p) {
      case pull::up:
        gpio_pull_up(pin_);
        break;
      case pull::down:
        gpio_pull_down(pin_);
        break;
      case pull::none:
        gpio_disable_pulls(pin_);
        break;
    }
  }

  bool read() const { return gpio_get(pin_); }

  u8 pin() const { return pin_; }

  input_pin(const input_pin&) = delete;
  input_pin& operator=(const input_pin&) = delete;

 private:
  u8 pin_;
};

class cs_guard {
 public:
  explicit cs_guard(output_pin& cs) : cs_{cs} { cs_.low(); }
  ~cs_guard() { cs_.high(); }

  cs_guard(const cs_guard&) = delete;
  cs_guard& operator=(const cs_guard&) = delete;

 private:
  output_pin& cs_;
};

}  // namespace jpico::hal
