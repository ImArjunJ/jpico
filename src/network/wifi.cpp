#include "network/wifi.hpp"

#include <cstdio>

#include "pico/stdlib.h"

namespace jpico {
namespace network {

WifiManager::~WifiManager() {
  if (initialized_) {
    deinit();
  }
}

bool WifiManager::init(uint32_t country_code) {
  if (initialized_) {
    return true;
  }

  if (cyw43_arch_init_with_country(country_code) != 0) {
    update_status(WifiStatus::connection_failed);
    return false;
  }

  cyw43_arch_enable_sta_mode();
  initialized_ = true;
  update_status(WifiStatus::disconnected);
  return true;
}

void WifiManager::deinit() {
  if (initialized_) {
    disconnect();
    cyw43_arch_deinit();
    initialized_ = false;
  }
}

bool WifiManager::connect(const WifiConfig& config) {
  if (!initialized_) {
    if (!init(config.country_code)) {
      return false;
    }
  }

  update_status(WifiStatus::connecting);

  int result = cyw43_arch_wifi_connect_timeout_ms(
      config.ssid, config.password, config.auth_mode, config.timeout_ms);

  if (result != 0) {
    update_status(WifiStatus::connection_failed);
    return false;
  }

  update_status(WifiStatus::connected);
  return true;
}

bool WifiManager::connect(const char* ssid, const char* password,
                          uint32_t timeout_ms) {
  WifiConfig config{.ssid = ssid,
                    .password = password,
                    .auth_mode = CYW43_AUTH_WPA2_AES_PSK,
                    .country_code = CYW43_COUNTRY_UK,
                    .timeout_ms = timeout_ms};
  return connect(config);
}

void WifiManager::disconnect() {
  if (initialized_ && status_ == WifiStatus::connected) {
    cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
    update_status(WifiStatus::disconnected);
  }
}

uint32_t WifiManager::get_ip_address() const {
  if (!is_connected()) {
    return 0;
  }

  struct netif* netif = netif_default;
  if (netif) {
    return ip4_addr_get_u32(netif_ip4_addr(netif));
  }
  return 0;
}

const char* WifiManager::get_ip_string() const {
  uint32_t ip = get_ip_address();
  if (ip == 0) {
    return "0.0.0.0";
  }

  snprintf(ip_str_buffer_, sizeof(ip_str_buffer_), "%lu.%lu.%lu.%lu", ip & 0xFF,
           (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
  return ip_str_buffer_;
}

int32_t WifiManager::get_rssi() const {
  if (!is_connected()) {
    return 0;
  }

  int32_t rssi = 0;
  cyw43_wifi_get_rssi(&cyw43_state, &rssi);
  return rssi;
}

const char* WifiManager::get_mac_address() const {
  if (!initialized_) {
    return "00:00:00:00:00:00";
  }

  uint8_t mac[6];
  cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, mac);

  snprintf(mac_str_buffer_, sizeof(mac_str_buffer_),
           "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3],
           mac[4], mac[5]);
  return mac_str_buffer_;
}

void WifiManager::set_led(bool on) {
  if (initialized_) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on ? 1 : 0);
  }
}

void WifiManager::blink_led(uint32_t on_ms, uint32_t off_ms, uint32_t count) {
  for (uint32_t i = 0; i < count; ++i) {
    set_led(true);
    sleep_ms(on_ms);
    set_led(false);
    sleep_ms(off_ms);
  }
}

void WifiManager::update_status(WifiStatus new_status) {
  if (status_ != new_status) {
    status_ = new_status;
    if (status_callback_) {
      status_callback_(new_status);
    }
  }
}

}  // namespace network
}  // namespace jpico
