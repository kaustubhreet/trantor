#pragma once

#include <variant>
#include <functional>

namespace trantor{
    enum class LogLevel{
        Error = 0,
        Warning = 1,
        Info = 2,
        Debug = 3,
    };

    using Logger  = std::function<void(LogLevel, const char*)>;

    struct Error{
        Error(const char* err, int sqlite_result) : err(err), sqlite_result(sqlite_result) { }

        const char* const err = nullptr;
        int sqlite_result;

        operator const char* () {
            return err;
        }
    };

    template<typename T>
    using Maybe = std::variant<Error, T>;
}