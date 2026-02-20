#pragma once

#include <functional>
#include <jpico/core.hpp>

extern "C" {
#include "lwip/netif.h"
#include "pico/cyw43_arch.h"
}

namespace jpico::network {

enum class wifi_status : u8 {
  disconnected,
  connecting,
  connected,
  connection_failed,
  no_ssid_available,
};

struct wifi_config {
  const char* ssid;
  const char* password;
  u32 auth_mode = CYW43_AUTH_WPA2_AES_PSK;
  u32 country_code = CYW43_COUNTRY_UK;
  u32 timeout_ms = 30000;
};

class wifi_manager {
 public:
  wifi_manager() = default;
  ~wifi_manager();

  wifi_manager(const wifi_manager&) = delete;
  wifi_manager& operator=(const wifi_manager&) = delete;

  result<void> init(u32 country_code = CYW43_COUNTRY_UK);
  void deinit();

  result<void> connect(const wifi_config& config);
  result<void> connect(const char* ssid, const char* password,
                       u32 timeout_ms = 30000);
  void disconnect();

  wifi_status status() const { return status_; }
  bool is_connected() const { return status_ == wifi_status::connected; }
  bool is_initialized() const { return initialized_; }

  u32 ip_address() const;
  const char* ip_string() const;
  i32 rssi() const;
  const char* mac_address() const;

  void set_led(bool on);
  void blink_led(u32 on_ms, u32 off_ms, u32 count);

  void poll();

  using status_callback = std::function<void(wifi_status)>;
  void set_status_callback(status_callback cb) { status_callback_ = cb; }

 private:
  void update_status(wifi_status new_status);

  bool initialized_ = false;
  wifi_status status_ = wifi_status::disconnected;
  status_callback status_callback_;
  mutable char ip_str_buffer_[16] = {0};
  mutable char mac_str_buffer_[18] = {0};
};

}  // namespace jpico::network
