#include <cstdio>
#include <jpico/log.hpp>
#include <jpico/network/wifi.hpp>

#include "pico/stdlib.h"

namespace jpico::network {

wifi_manager::~wifi_manager() {
  if (initialized_) deinit();
}

result<void> wifi_manager::init(u32 country_code) {
  if (initialized_) return ok();

  if (cyw43_arch_init_with_country(country_code) != 0) {
    update_status(wifi_status::connection_failed);
    return fail(error_code::hardware_fault, "cyw43 init failed");
  }

  cyw43_arch_enable_sta_mode();
  initialized_ = true;
  update_status(wifi_status::disconnected);
  log::info("wifi initialized");
  return ok();
}

void wifi_manager::deinit() {
  if (initialized_) {
    disconnect();
    cyw43_arch_deinit();
    initialized_ = false;
  }
}

result<void> wifi_manager::connect(const wifi_config& config) {
  if (!initialized_) {
    auto r = init(config.country_code);
    if (!r) return r;
  }

  update_status(wifi_status::connecting);

  int result = cyw43_arch_wifi_connect_timeout_ms(
      config.ssid, config.password, config.auth_mode, config.timeout_ms);

  if (result != 0) {
    update_status(wifi_status::connection_failed);
    return fail(error_code::connection_failed, "wifi connect failed");
  }

  update_status(wifi_status::connected);
  log::info("wifi connected: %s", ip_string());
  return ok();
}

result<void> wifi_manager::connect(const char* ssid, const char* password,
                                   u32 timeout_ms) {
  wifi_config config{
      .ssid = ssid,
      .password = password,
      .auth_mode = CYW43_AUTH_WPA2_AES_PSK,
      .country_code = CYW43_COUNTRY_UK,
      .timeout_ms = timeout_ms,
  };
  return connect(config);
}

void wifi_manager::disconnect() {
  if (initialized_ && status_ == wifi_status::connected) {
    cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
    update_status(wifi_status::disconnected);
  }
}

u32 wifi_manager::ip_address() const {
  if (!is_connected()) return 0;
  struct netif* n = netif_default;
  return n ? ip4_addr_get_u32(netif_ip4_addr(n)) : 0;
}

const char* wifi_manager::ip_string() const {
  u32 ip = ip_address();
  if (ip == 0) return "0.0.0.0";
  snprintf(ip_str_buffer_, sizeof(ip_str_buffer_), "%u.%u.%u.%u", ip & 0xFF,
           (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
  return ip_str_buffer_;
}

i32 wifi_manager::rssi() const {
  if (!is_connected()) return 0;
  i32 r = 0;
  cyw43_wifi_get_rssi(&cyw43_state, &r);
  return r;
}

const char* wifi_manager::mac_address() const {
  if (!initialized_) return "00:00:00:00:00:00";
  u8 mac[6];
  cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, mac);
  snprintf(mac_str_buffer_, sizeof(mac_str_buffer_),
           "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3],
           mac[4], mac[5]);
  return mac_str_buffer_;
}

void wifi_manager::set_led(bool on) {
  if (initialized_) cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on ? 1 : 0);
}

void wifi_manager::blink_led(u32 on_ms, u32 off_ms, u32 count) {
  for (u32 i = 0; i < count; ++i) {
    set_led(true);
    sleep_ms(on_ms);
    set_led(false);
    sleep_ms(off_ms);
  }
}

void wifi_manager::poll() {
  if (initialized_) cyw43_arch_poll();
}

void wifi_manager::update_status(wifi_status new_status) {
  if (status_ != new_status) {
    status_ = new_status;
    if (status_callback_) status_callback_(new_status);
  }
}

}  // namespace jpico::network
