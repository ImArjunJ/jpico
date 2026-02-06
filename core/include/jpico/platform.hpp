#pragma once

namespace jpico::platform {

#if defined(PICO_RP2350)
inline constexpr bool is_rp2350 = true;
#else
inline constexpr bool is_rp2350 = false;
#endif

#if defined(PICO_RP2040)
inline constexpr bool is_rp2040 = true;
#else
inline constexpr bool is_rp2040 = false;
#endif

#if defined(CYW43_WL_GPIO_LED_PIN) || defined(PICO_CYW43_SUPPORTED)
inline constexpr bool has_wifi = true;
#else
inline constexpr bool has_wifi = false;
#endif

#if defined(PICO_RP2350)
inline constexpr int core_count = 2;  // RP2350: dual-core ARM or RISC-V
#elif defined(PICO_RP2040)
inline constexpr int core_count = 2;  // RP2040: dual-core ARM
#else
inline constexpr int core_count = 1;
#endif

}  // namespace jpico::platform
