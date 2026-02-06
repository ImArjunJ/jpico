#pragma once

#include <jpico/result.hpp>
#include <jpico/types.hpp>
#include <span>

#include "hardware/i2c.h"
#include "pico/stdlib.h"

namespace jpico::hal {

struct i2c_config {
  u32 baudrate = 400'000;  // < 400 kHz standard fast-mode
  u8 pin_sda = 0;
  u8 pin_scl = 0;
};

class i2c_bus {
 public:
  i2c_bus(i2c_inst_t* inst, i2c_config cfg) : inst_{inst}, config_{cfg} {
    i2c_init(inst_, config_.baudrate);

    gpio_set_function(config_.pin_sda, GPIO_FUNC_I2C);
    gpio_set_function(config_.pin_scl, GPIO_FUNC_I2C);
    gpio_pull_up(config_.pin_sda);
    gpio_pull_up(config_.pin_scl);
  }

  ~i2c_bus() { i2c_deinit(inst_); }

  i2c_bus(i2c_bus&& other) noexcept
      : inst_{other.inst_}, config_{other.config_} {
    other.inst_ = nullptr;
  }

  i2c_bus& operator=(i2c_bus&& other) noexcept {
    if (this != &other) {
      if (inst_) i2c_deinit(inst_);
      inst_ = other.inst_;
      config_ = other.config_;
      other.inst_ = nullptr;
    }
    return *this;
  }

  i2c_bus(const i2c_bus&) = delete;
  i2c_bus& operator=(const i2c_bus&) = delete;

  result<usize> write(u8 addr, std::span<const u8> data, bool nostop = false) {
    auto n = i2c_write_blocking(inst_, addr, data.data(), data.size(), nostop);
    if (n < 0) {
      return fail(error_code::io_error, "i2c write failed");
    }
    return ok(static_cast<usize>(n));
  }

  result<usize> read(u8 addr, std::span<u8> buf, bool nostop = false) {
    auto n = i2c_read_blocking(inst_, addr, buf.data(), buf.size(), nostop);
    if (n < 0) {
      return fail(error_code::io_error, "i2c read failed");
    }
    return ok(static_cast<usize>(n));
  }

  result<usize> write_read(u8 addr, std::span<const u8> tx, std::span<u8> rx) {
    auto w = write(addr, tx, /*nostop=*/true);
    if (!w) return std::unexpected(w.error());
    return read(addr, rx);
  }

  i2c_inst_t* instance() const { return inst_; }
  const i2c_config& config() const { return config_; }

 private:
  i2c_inst_t* inst_;
  i2c_config config_;
};

}  // namespace jpico::hal
