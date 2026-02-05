#pragma once

#include <cstdint>
#include <functional>
#include <string_view>

extern "C" {
#include "lwip/netif.h"
#include "pico/cyw43_arch.h"
}

namespace jpico {
namespace network {

enum class WifiStatus {
  disconnected,
  connecting,
  connected,
  connection_failed,
  no_ssid_available
};

struct WifiConfig {
  const char* ssid;
  const char* password;
  uint32_t auth_mode = CYW43_AUTH_WPA2_AES_PSK;
  uint32_t country_code = CYW43_COUNTRY_UK;
  uint32_t timeout_ms = 30000;
};

class WifiManager {
 public:
  WifiManager() = default;
  ~WifiManager();

  WifiManager(const WifiManager&) = delete;
  WifiManager& operator=(const WifiManager&) = delete;

  bool init(uint32_t country_code = CYW43_COUNTRY_UK);
  void deinit();

  bool connect(const WifiConfig& config);
  bool connect(const char* ssid, const char* password,
               uint32_t timeout_ms = 30000);
  void disconnect();

  WifiStatus get_status() const { return status_; }
  bool is_connected() const { return status_ == WifiStatus::connected; }
  bool is_initialized() const { return initialized_; }

  uint32_t get_ip_address() const;
  const char* get_ip_string() const;
  int32_t get_rssi() const;
  const char* get_mac_address() const;

  void set_led(bool on);
  void blink_led(uint32_t on_ms, uint32_t off_ms, uint32_t count);

  using StatusCallback = std::function<void(WifiStatus)>;
  void set_status_callback(StatusCallback callback) {
    status_callback_ = callback;
  }

 private:
  void update_status(WifiStatus new_status);

  bool initialized_ = false;
  WifiStatus status_ = WifiStatus::disconnected;
  StatusCallback status_callback_;
  mutable char ip_str_buffer_[16] = {0};
  mutable char mac_str_buffer_[18] = {0};
};

}  // namespace network
}  // namespace jpico
