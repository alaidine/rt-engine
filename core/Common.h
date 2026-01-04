#pragma once

#include "framework.h"

#include <fmt/format.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <utility>

template <typename T> using Ref = std::shared_ptr<T>;
template <typename T, typename... Args> constexpr Ref<T> CreateRef(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#define RO_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define RO_LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define RO_LOG_ERR(...) spdlog::error(__VA_ARGS__)
#define RO_ASSERT(_EXPR) assert(_EXPR)

#ifdef _WIN32
#include <stdio.h>
#define POPEN _popen
#define PCLOSE _pclose

#else
// Linux / Mac
#include <cstdio>
#define POPEN popen
#define PCLOSE pclose
#endif

class ProcessRunner {
  public:
    // Returns a struct containing the Exit Code and the text Output
    struct ProcessResult {
        int ExitCode;
        std::string Output;
    };

    static ProcessResult Execute(const std::string &cmd) {
        std::string command = cmd;

        // redirect stderr to stdout so we capture errors too
        command += " 2>&1";

        char buffer[128];
        std::string result;

        memset(buffer, 0, 128);

        // Open the pipe
        FILE *pipe = POPEN(command.c_str(), "r");
        if (!pipe) {
            return {-1, "Failed to start process."};
        }

        // Read output line by line
        while (fgets(buffer, 128, pipe) != NULL) {
            result += buffer;
        }

        // Close and get exit code
        int exitCode = PCLOSE(pipe);

        return {exitCode, result};
    }
};
