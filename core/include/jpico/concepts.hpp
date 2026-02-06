#pragma once

#include <concepts>
#include <jpico/types.hpp>

namespace jpico {

// any device that can receive pixel data.
// drivers for ili9341, ST7789, SSD1306, etc. should satisfy this.
template <typename T>
concept display =
    requires(T d, u16 x, u16 y, u16 w, u16 h, u16 color, const u16* data) {
      { d.width() } -> std::convertible_to<u16>;
      { d.height() } -> std::convertible_to<u16>;
      { d.fill(color) } -> std::same_as<void>;
      { d.pixel(x, y, color) } -> std::same_as<void>;
      { d.blit(x, y, w, h, data) } -> std::same_as<void>;
    };

// any chip that communicates over SPI with chip-select semantics.
template <typename T>
concept spi_device = requires(T d) {
  { d.select() } -> std::same_as<void>;
  { d.deselect() } -> std::same_as<void>;
};

// any chip that communicates over I2C.
template <typename T>
concept i2c_device = requires(T d) {
  { d.address() } -> std::convertible_to<u8>;
};

// any source of user input (button, encoder, touchscreen, keypad...).
template <typename T>
concept input_source = requires(T s) {
  { s.poll() } -> std::same_as<void>;
  { s.pressed() } -> std::convertible_to<bool>;
};

// any touch input that gives screen coordinates.
template <typename T>
concept touch_source = requires(T t) {
  { t.touched() } -> std::convertible_to<bool>;
  { t.read() } -> std::convertible_to<point>;
};

}  // namespace jpico
