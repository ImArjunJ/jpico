#pragma once

#include <jpico/result.hpp>
#include <jpico/types.hpp>
#include <span>

#include "hardware/spi.h"
#include "pico/stdlib.h"

namespace jpico::hal {

struct spi_config {
  u32 baudrate = 10'000'000;
  u8 pin_sck = 0;
  u8 pin_tx = 0;
  u8 pin_rx = 0xFF;  ///< 0xFF = unused / not connected
  spi_cpol_t cpol = SPI_CPOL_0;
  spi_cpha_t cpha = SPI_CPHA_0;
};

class spi_bus {
 public:
  spi_bus(spi_inst_t* inst, spi_config cfg) : inst_{inst}, config_{cfg} {
    spi_init(inst_, config_.baudrate);
    spi_set_format(inst_, 8, config_.cpol, config_.cpha, SPI_MSB_FIRST);

    gpio_set_function(config_.pin_sck, GPIO_FUNC_SPI);
    gpio_set_function(config_.pin_tx, GPIO_FUNC_SPI);
    if (config_.pin_rx != 0xFF) {
      gpio_set_function(config_.pin_rx, GPIO_FUNC_SPI);
    }
  }

  ~spi_bus() { spi_deinit(inst_); }

  spi_bus(spi_bus&& other) noexcept
      : inst_{other.inst_}, config_{other.config_} {
    other.inst_ = nullptr;
  }

  spi_bus& operator=(spi_bus&& other) noexcept {
    if (this != &other) {
      if (inst_) spi_deinit(inst_);
      inst_ = other.inst_;
      config_ = other.config_;
      other.inst_ = nullptr;
    }
    return *this;
  }

  spi_bus(const spi_bus&) = delete;
  spi_bus& operator=(const spi_bus&) = delete;

  result<usize> write(std::span<const u8> data) {
    set_format(8, config_.cpol, config_.cpha);
    auto n =
        spi_write_blocking(inst_, data.data(), static_cast<usize>(data.size()));
    return ok(static_cast<usize>(n));
  }

  result<usize> write16(std::span<const u16> data) {
    set_format(16, config_.cpol, config_.cpha);
    auto n = spi_write16_blocking(inst_, data.data(),
                                  static_cast<usize>(data.size()));
    return ok(static_cast<usize>(n));
  }

  result<usize> transfer(std::span<const u8> tx, std::span<u8> rx) {
    set_format(8, config_.cpol, config_.cpha);
    auto len = std::min(tx.size(), rx.size());
    auto n = spi_write_read_blocking(inst_, tx.data(), rx.data(),
                                     static_cast<usize>(len));
    return ok(static_cast<usize>(n));
  }

  void set_format(u8 bits, spi_cpol_t cpol, spi_cpha_t cpha) {
    spi_set_format(inst_, bits, cpol, cpha, SPI_MSB_FIRST);
  }

  u32 set_baudrate(u32 baud) { return spi_set_baudrate(inst_, baud); }

  spi_inst_t* instance() const { return inst_; }
  const spi_config& config() const { return config_; }

 private:
  spi_inst_t* inst_;
  spi_config config_;
};

}  // namespace jpico::hal
