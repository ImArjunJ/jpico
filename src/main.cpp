#include <cstdio>

#include "config.hpp"
#include "drivers/ili9341.hpp"
#include "graphics/canvas.hpp"
#include "network/wifi.hpp"
#include "pico/stdlib.h"

#define LOG(fmt, ...)                              \
  do {                                             \
    if (jpico::config::app::enable_debug_output) { \
      printf("[jpico] " fmt "\n", ##__VA_ARGS__);  \
    }                                              \
  } while (0)

#define LOG_ERROR(fmt, ...)                           \
  do {                                                \
    if (jpico::config::app::enable_debug_output) {    \
      printf("[jpico-err] " fmt "\n", ##__VA_ARGS__); \
    }                                                 \
  } while (0)

static std::unique_ptr<jpico::drivers::ILI9341Display> display;
static std::unique_ptr<jpico::graphics::Canvas> canvas;
static jpico::network::WifiManager wifi;

bool init_display() {
  LOG("initializing display...");

  display = std::make_unique<jpico::drivers::ILI9341Display>(
      jpico::config::display::pin_dc, jpico::config::display::pin_cs,
      jpico::config::display::pin_rst, jpico::config::display::pin_sclk,
      jpico::config::display::pin_mosi);

  if (!display->init()) {
    return false;
  }

  display->set_rotation(4);

  canvas = std::make_unique<jpico::graphics::Canvas>(*display);
  canvas->create_framebuffer();
  canvas->set_clear_color(jpico::config::colors::terminal_bg);
  canvas->set_text_color(jpico::config::colors::terminal_fg);
  canvas->clear();

  LOG("display initialized: %dx%d", display->get_width(),
      display->get_height());
  return true;
}

bool init_wifi() {
  LOG("initializing wifi...");

  wifi.set_status_callback([](jpico::network::WifiStatus status) {
    switch (status) {
      case jpico::network::WifiStatus::connecting:
        wifi.set_led(true);
        break;
      case jpico::network::WifiStatus::connected:
        wifi.blink_led(100, 100, 3);
        break;
      case jpico::network::WifiStatus::disconnected:
      case jpico::network::WifiStatus::connection_failed:
        wifi.set_led(false);
        break;
      default:
        break;
    }
  });

  if (!wifi.init(CYW43_COUNTRY_UK)) {
    return false;
  }

  LOG("wifi initialized, connecting to %s...",
      jpico::config::wifi::default_ssid);

  canvas->printf("connecting to wifi: %s\n", jpico::config::wifi::default_ssid);
  canvas->flush();

  if (!wifi.connect(jpico::config::wifi::default_ssid,
                    jpico::config::wifi::default_password,
                    jpico::config::wifi::connect_timeout_ms)) {
    canvas->set_text_color(jpico::config::colors::red);
    canvas->print("wifi connection failed!\n");
    canvas->flush();
    return false;
  }

  LOG("wifi connected! IP: %s", wifi.get_ip_string());
  return true;
}

int main() {
  stdio_init_all();
  sleep_ms(jpico::config::app::usb_init_delay_ms);

  if (!init_display()) {
    LOG_ERROR("display initialization failed!");
    while (true) {
      sleep_ms(1000);
    }
  }

  if (!init_wifi()) {
    LOG_ERROR("wifi initialization failed!");
  }

  canvas->clear();
  canvas->set_cursor(0, 0);
  canvas->set_text_color(jpico::config::colors::cyan);
  canvas->print("hello, jpico!\n");
  canvas->printf("val: %d\n", 42);
  canvas->flush();

  while (true) {
    cyw43_arch_poll();
    canvas->flush();
    sleep_ms(10);
  }

  return 0;
}
