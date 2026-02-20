#include <jpico/core.hpp>
#include <jpico/hal/time.hpp>
#include <jpico/network/wifi.hpp>

using namespace jpico;

int main() {
  hal::init_stdio();
  hal::sleep(2000);

  network::wifi_manager wifi;

  wifi.set_status_callback([&](network::wifi_status status) {
    switch (status) {
      case network::wifi_status::connecting:
        wifi.set_led(true);
        break;
      case network::wifi_status::connected:
        wifi.blink_led(100, 100, 3);
        break;
      default:
        wifi.set_led(false);
        break;
    }
  });

  auto init = wifi.init();
  if (!init) {
    log::error("wifi init failed: %s", init.error().message);
    while (true) hal::sleep(1000);
  }

  log::info("connecting...");

  auto conn = wifi.connect("YOUR_SSID", "YOUR_PASSWORD", 30000);
  if (!conn) {
    log::error("connect failed: %s", conn.error().message);
  } else {
    log::info("connected! ip: %s", wifi.ip_string());
    log::info("rssi: %ld dBm", wifi.rssi());
  }

  while (true) {
    wifi.poll();
    hal::sleep(10);
  }
}
