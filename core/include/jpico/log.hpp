#pragma once

#include <cstdio>
#include <jpico/types.hpp>

namespace jpico::log {

enum class level : u8 {
  trace = 0,
  debug = 1,
  info = 2,
  warn = 3,
  error = 4,
  off = 5,
};

#ifndef JPICO_LOG_LEVEL
#define JPICO_LOG_LEVEL 2  // info
#endif

inline constexpr level min_level = static_cast<level>(JPICO_LOG_LEVEL);

namespace detail {

inline constexpr const char* level_tag(level l) {
  switch (l) {
    case level::trace:
      return "[TRC]";
    case level::debug:
      return "[DBG]";
    case level::info:
      return "[INF]";
    case level::warn:
      return "[WRN]";
    case level::error:
      return "[ERR]";
    default:
      return "[???]";
  }
}

template <level L, typename... Args>
inline void emit(const char* fmt, Args... args) {
  if constexpr (L >= min_level) {
    std::printf("%s ", level_tag(L));
    std::printf(fmt, args...);
    std::printf("\n");
  }
}

}  // namespace detail

template <typename... Args>
inline void trace(const char* fmt, Args... args) {
  detail::emit<level::trace>(fmt, args...);
}

template <typename... Args>
inline void debug(const char* fmt, Args... args) {
  detail::emit<level::debug>(fmt, args...);
}

template <typename... Args>
inline void info(const char* fmt, Args... args) {
  detail::emit<level::info>(fmt, args...);
}

template <typename... Args>
inline void warn(const char* fmt, Args... args) {
  detail::emit<level::warn>(fmt, args...);
}

template <typename... Args>
inline void error(const char* fmt, Args... args) {
  detail::emit<level::error>(fmt, args...);
}

}  // namespace jpico::log
