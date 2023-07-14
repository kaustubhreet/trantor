#pragma once

#include <variant>
#include <functional>
#include <string_view>
#include <sqlite3.h>

/**
 * @dir include
 * @brief Namespace @ref trantor
 */

/**
    * @file
    * @brief struct @ref Error
    */

namespace trantor{
    enum class LogLevel{
        Error = 0,
        Warning = 1,
        Info = 2,
        Debug = 3,
    };

    using Logger  = std::function<void(LogLevel, const char*)>;

    /**
     * @brief structure to maintain Errors
     */

    struct Error{
        /**
         * @brief Constructor
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param flags             Image layout flags
         *
         * The @p data array is expected to be of proper size for given
         * parameters. For a 3D image, if @p flags contain
         * @ref ImageFlag3D::CubeMap, the @p size is expected to match its
         * restrictions.
         */
        Error(const char* const err, int sqlite_result) : err(err), sqlite_result(sqlite_result) { }

        const char* err;
        int sqlite_result;

        operator std::string () const{
            const char* sqlError = sqlite3_errstr(sqlite_result);
            std::string out = std::string(err) + ":" + std::string(sqlError);

            return out;
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