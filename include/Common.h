#pragma once

#include "framework.h"

#include <memory>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <utility>

template <typename T> using Ref = std::shared_ptr<T>;
template <typename T, typename... Args> constexpr Ref<T> CreateRef(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#ifdef _WIN32
#define RO_DEBGBRK() __debugbreak()
#endif

#define RO_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define RO_LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define RO_LOG_ERR(...) spdlog::error(__VA_ARGS__)
#define RO_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define RO_ASSERT(_EXPR) assert(_EXPR)
