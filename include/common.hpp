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

    /**
     * @brief Defines a type alias for a function pointer that takes a @ref trantor::LogLevel enumeration value
     * and a C-string as arguments and returns void
     */

    using Logger = std::function<void(LogLevel, const char*)>;

    /**
     * @struct Error
     *
     * @brief Represents an error with associated error message and SQLite result code.
     */

    struct Error {
        /**
         * @brief Constructor for Error.
         * @param err               The error message.
         * @param sqlite_result     The SQLite result code (default is SQLITE_OK).
         */
        Error(const char *const err, int sqlite_result=SQLITE_OK) : err(err), sqlite_result(sqlite_result) {}

        const char *err;        ///< The error message.
        int sqlite_result;      ///< The SQLite result code.

        /**
        * @brief Stream insertion operator to output an Error.
        * @param out    The output stream.
        * @param e      The Error to be output.
        * @return The output stream after writing the Error.
        */

        friend std::ostream& operator<< (std::ostream &out, const Error& e){
            out << std::string(e.err);
            if (e.sqlite_result != SQLITE_OK) {
                const char* sqlErr = sqlite3_errstr(e.sqlite_result);
                out <<": " + std::string(sqlErr);
            }

            return out;
        }

        /**
         * @brief Parenthesis Operator Overload to output an elegant error string
         * @return std::string error
         */
        operator std::string() const {
            std::ostringstream out;
            out << *this;
            return out.str();
        }
    };

    template<typename T>
    using Maybe = std::variant<Error, T>;

    /**
     * @brief A templated structure to store a fixed-length character array.
     *
     * This structure provides a way to store a fixed-length character array of size N.
     * It is particularly useful for storing the names of tables and columns.
     *
     * @tparam N The size of the fixed-length character array.
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

    /**
    * @brief Appends string representations of variadic elements to an output stream,
    * separated by specified delimiter.
    *
    * @tparam T         Pack of types representing elements whose string representations are to be appended.
    * @param ss         The output string stream to which the string representations will be appended.
    * @param delim      The delimiter used to separate the string representations.
    */
    template<typename... T>
    auto appendToStringStream(std::ostringstream &ss, const char *delim) {
        std::array<std::string, sizeof...(T)> strings = {T::to_string()...};
        std::copy(strings.begin(), strings.end(),
                  std::ostream_iterator<std::string>(ss, delim));
    }

    /**
     * @brief Appends fixed-length string values to an output stream,
     * separated by a specified delimiter.
     *
     * @tparam string       Pack of `FixedLengthString` objects representing values to be appended.
     * @param ss            The output string stream to which the string values will be appended.
     * @param delim         The delimiter used to separate the string values.
     */
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

    /**
     * @brief Checks whether a given type is a continuous container type.
     * A continuous container is one that can store a sequence of
     * elements and provides a continuous memory layout.
     *
     * @tparam T    The type to be checked.
     * @return `true` if the type is a continuous container type, `false` otherwise.
     */
    template<typename T>
    static constexpr bool IsContinuousContainer() {
        using plain = remove_optional<std::remove_cvref_t<T>>::type;
        return is_vector<plain>::value || is_basic_string<plain>::value || is_array<plain>::value;
    }

    /**
     * @concept{ContinuousContainer}
     *
     * @brief Concept for checking if a type is a continuous container.
     *
     * This concept provides a more intuitive way to check if a type is a
     * continuous container using the `ContinuousContainer` template function.
     *
     * @tparam T The type to be checked.
     */
    template<typename T>
    concept ContinuousContainer = IsContinuousContainer<T>();

    /**
     * @brief Checks whether a given type is an arithmetic type.
     * An arithmetic type is one that represents a numeric value
     * and supports arithmetic operations.
     *
     * @tparam T The type to be checked.
     * @return `true` if the type is an arithmetic type, `false` otherwise.
     */
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