#pragma once

#include <jpico/types.hpp>

#include "pico/stdlib.h"

namespace jpico::hal {

inline void init_stdio() { stdio_init_all(); }
inline void sleep(u32 ms) { sleep_ms(ms); }

}  // namespace jpico::hal
