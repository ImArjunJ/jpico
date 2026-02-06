#pragma once

#include <jpico/types.hpp>

#include "hardware/dma.h"

namespace jpico::hal {

class dma_channel {
 public:
  dma_channel() : channel_{dma_claim_unused_channel(true)} {}

  ~dma_channel() {
    if (channel_ >= 0) {
      dma_channel_unclaim(static_cast<u32>(channel_));
    }
  }

  dma_channel(dma_channel&& other) noexcept : channel_{other.channel_} {
    other.channel_ = -1;
  }

  dma_channel& operator=(dma_channel&& other) noexcept {
    if (this != &other) {
      if (channel_ >= 0) {
        dma_channel_unclaim(static_cast<u32>(channel_));
      }
      channel_ = other.channel_;
      other.channel_ = -1;
    }
    return *this;
  }

  dma_channel(const dma_channel&) = delete;
  dma_channel& operator=(const dma_channel&) = delete;

  int channel() const { return channel_; }

  dma_channel_config default_config() const {
    return dma_channel_get_default_config(static_cast<u32>(channel_));
  }

  auto transfer(const volatile void* read_addr, volatile void* write_addr,
                u32 count, const dma_channel_config& cfg, bool trigger = true)
      -> void {
    dma_channel_configure(static_cast<u32>(channel_), &cfg, write_addr,
                          read_addr, count, trigger);
  }

  void wait() {
    dma_channel_wait_for_finish_blocking(static_cast<u32>(channel_));
  }

  bool busy() const { return dma_channel_is_busy(static_cast<u32>(channel_)); }

 private:
  int channel_;
};

}  // namespace jpico::hal
