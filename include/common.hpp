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
        Error(const char *const err, int sqlite_result=SQLITE_OK) : err(err), sqlite_result(sqlite_result) {}

        const char *err;
        int sqlite_result;

        /**
         * @brief Parenthesis Operator Overload to output an elegant error string
         * @return std::string error
         */
        operator std::string() const {
            std::ostringstream out;
            out << std::string(err);
            if (sqlite_result != SQLITE_OK) {
                const char* sqlErr = sqlite3_errstr(sqlite_result);
                out <<": " + std::string(sqlErr);
            }
            return out.str();
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

    template<bool... T>
    static constexpr bool AnyOf = (... || T);

    template<bool... T>
    static constexpr bool AllOf = (... && T);

    template<bool... T>
    struct IndexOfFirst {
    private:
        static constexpr int _impl() {
            constexpr std::array<bool, sizeof...(T)> a{T...};
            const auto it = std::find(a.begin(), a.end(), true);

            // As we are in constant expression, we will have compilation error.
            if (it == a.end()) return -1;

            return std::distance(a.begin(), it);
        }
    public:
        static constexpr int value = _impl();
    };

    template<typename T>
    struct is_vector : std::false_type  { };

    template<typename T, typename A>
    struct is_vector<std::vector<T, A>> : std::true_type {};

    template<typename T>
    struct is_basic_string : std::false_type  { };

    template<typename CharT, typename Traits, typename Allocator>
    struct is_basic_string<std::basic_string<CharT, Traits, Allocator>> : std::true_type {};

    template<typename T>
    struct remove_optional : std::type_identity<T> {};

    template<typename T>
    struct remove_optional<std::optional<T>> : std::type_identity<T> {};

    template<typename T>
    struct is_array : std::false_type  { };

    template<typename T, auto s>
    struct is_array<std::array<T, s>> : std::true_type {};

    template<typename T>
    static constexpr bool IsContinuousContainer() {
        using plain = remove_optional<std::remove_cvref_t<T>>::type;
        return is_vector<plain>::value || is_basic_string<plain>::value || is_array<plain>::value;
    }

    template<typename T>
    concept ContinuousContainer = IsContinuousContainer<T>();
    template<typename T>
    static constexpr bool IsArithmetic() {
        using plain = remove_optional<std::remove_cvref_t<T>>::type;
        return std::is_arithmetic_v<plain>;
    }

    template<typename T>
    concept ArithmeticT = IsArithmetic<T>();

    template<typename T>
    struct is_optional : std::false_type  { };

    template<typename T>
    struct is_optional<std::optional<T>> : std::true_type {};

    template<typename T>
    static constexpr bool IsOptional() {
        using plain = std::remove_cvref_t<T>;
        return is_optional<plain>::value;
    }

    template<typename T>
    concept OptionalT = IsOptional<T>();

    template<typename T>
    struct is_string : std::false_type  { };

    template<typename CharT, typename Traits, typename Allocator>
    struct is_string<std::basic_string<CharT, Traits, Allocator>> : std::true_type {};

    template <typename T>
    static constexpr bool IsString() {
        using plain = remove_optional<std::remove_cvref_t<T>>::type;
        return is_string<plain>::value;
    }

    // unique tuple https://stackoverflow.com/a/57528226
    namespace __unique_tuple_detail {
        template <typename T, typename... Ts>
        struct unique : std::type_identity<T> {};

        template <typename... Ts, typename U, typename... Us>
        struct unique<std::tuple<Ts...>, U, Us...>
                : std::conditional_t<(std::is_same_v<U, Ts> || ...)
                        , unique<std::tuple<Ts...>, Us...>
                        , unique<std::tuple<Ts..., U>, Us...>> {};
    };

    template <typename... Ts>
    using unique_tuple = typename __unique_tuple_detail::unique<std::tuple<>, Ts...>::type;
}