#pragma once

#include <cstdint>

namespace jpico {
namespace config {

namespace display {
constexpr uint8_t pin_dc = 15;
constexpr uint8_t pin_cs = 13;
constexpr uint8_t pin_rst = 14;
constexpr uint8_t pin_sclk = 6;
constexpr uint8_t pin_mosi = 7;
constexpr uint8_t pin_miso = 4;

constexpr uint16_t width = 320;
constexpr uint16_t height = 240;

constexpr uint32_t spi_baudrate = 62500000;
}  // namespace display

namespace wifi {
constexpr const char* default_ssid = "your_ssid";
constexpr const char* default_password = "your_password";
constexpr uint32_t connect_timeout_ms = 30000;
}  // namespace wifi

namespace colors {
constexpr uint16_t black = 0x0000;
constexpr uint16_t white = 0xFFFF;
constexpr uint16_t red = 0xF800;
constexpr uint16_t green = 0x07E0;
constexpr uint16_t blue = 0x001F;
constexpr uint16_t cyan = 0x07FF;
constexpr uint16_t magenta = 0xF81F;
constexpr uint16_t yellow = 0xFFE0;
constexpr uint16_t orange = 0xFD20;
constexpr uint16_t purple = 0x8010;
constexpr uint16_t gray = 0x8410;
constexpr uint16_t dark_gray = 0x4208;
constexpr uint16_t light_gray = 0xC618;

constexpr uint16_t terminal_bg = 0x0000;
constexpr uint16_t terminal_fg = 0x07E0;
constexpr uint16_t terminal_cursor = 0xFFFF;
}  // namespace colors

namespace tasks {
constexpr uint32_t main_task_stack_size = 4096;
constexpr uint8_t main_task_priority = 1;

constexpr uint32_t network_task_stack_size = 2048;
constexpr uint8_t network_task_priority = 2;
}  // namespace tasks

namespace app {
constexpr uint32_t usb_init_delay_ms = 2000;
constexpr bool enable_debug_output = true;
}  // namespace app

}  // namespace config
}  // namespace jpico
