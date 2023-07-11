#pragma once

#include <variant>
#include <functional>
#include <string_view>

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

    template<size_t N>
    struct FixedLengthString{
        constexpr FixedLengthString(const char (&str)[N]){
            std::copy_n(str, N, value);
        }

        char value[N];
    };

    template <typename T>
    consteval auto type_name() {
        std::string_view name, prefix, suffix;
#ifdef defined(__clang__)
        name = __PRETTY_FUNCTION__;
        prefix = "auto type_name() [T = ";
        suffix = "]";
#elif defined(__GNUC__)
        name = __PRETTY_FUNCTION__;
        prefix = "constexpr auto type_name() [with T = ";
        suffix = "]";
#elif defined(_MSC_VER)
        name = __FUNCSIG__;
        prefix = "auto __cdecl type_name<";
        suffix = ">(void)";
#endif
        name.remove_prefix(prefix.size());
        name.remove_suffix(suffix.size());
        return name;
    }
}