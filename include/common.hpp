#pragma once

/**
    * @file common.hpp
    * @brief Contains routines for common usage by other classes.
    * @brief enum @ref trantor::LogLevel, struct @ref trantor::Error, struct @ref trantor::FixedLengthString, function @ref trantor::type_name()
    *
    */

#include <variant>
#include <functional>
#include <string_view>
#include <sqlite3.h>

namespace trantor {

    /**
     * @enum LogLevel
     *
     * @brief Describes various levels of Log types
     *
     */
    enum class LogLevel {
        Error = 0,
        Warning = 1,
        Info = 2,
        Debug = 3,
    };

    using Logger = std::function<void(LogLevel, const char *)>;

    /**
     * @struct Error
     *
     * @brief Provides error logging and maintaining methods
     */

    struct Error {
        /**
         * @brief Constructor
         * @param err               error message
         * @param sqlite_result     Code from the SQLite descriptor
         *
         */
        Error(const char *const err, int sqlite_result) : err(err), sqlite_result(sqlite_result) {}

        const char *err;
        int sqlite_result;

        /**
         * @brief Parenthesis Operator Overload to output an elegant error string
         * @return std::string error
         */
        operator std::string() const {
            const char *sqlError = sqlite3_errstr(sqlite_result);
            std::string out = std::string(err) + ":" + std::string(sqlError);

            return out;
        }
    };

    template<typename T>
    using Maybe = std::variant<Error, T>;

    /**
     * @brief Abstraction to store names of tables and columns
     * @tparam N denotes the size of input size
     */
    template<size_t N>
    struct FixedLengthString {
        constexpr FixedLengthString(const char (&str)[N]) {
            std::copy_n(str, N, value);
        }

        char value[N];
    };

    template<typename T>
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

    template<typename... T>
    auto appendToStringStream(std::ostringstream &ss, const char *delim) {
        std::array<std::string, sizeof...(T)> strings = {T::to_string()...};
        std::copy(strings.begin(), strings.end(),
                  std::ostream_iterator<std::string>(ss, delim));
    }

    template<FixedLengthString...string>
    requires requires { (sizeof(string.value) + ...); }
    void appendToStringStream(std::ostringstream &ss, const char *delim) {
        std::array<std::string, sizeof...(string)> strings = {string.value...};
        std::copy(strings.begin(), strings.end(),
                  std::ostream_iterator<std::string>(ss, delim));
    }
}