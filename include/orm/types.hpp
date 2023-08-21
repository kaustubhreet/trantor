/**
 * @file types.hpp
 * @brief Contains utility functions and type traits related to SQL operations.
 *
 * This file defines enums, functions, and template structures that are useful
 * for working with SQL types, ordering, and type conversions.
 */

#pragma once

namespace trantor{

    /**
     * @enum sql_type_t
     * @brief Enumerates various SQL data types.
     *
     * This enumeration provides a set of SQL data types that can be used for
     * defining columns in database tables.
     */
    enum class sql_type_t{
        INTEGER,     /**< Integer data type */
        TEXT,        /**< Text data type */
        BLOB,        /**< Binary large object data type */
        NUMERIC,     /**< Numeric data type */
        REAL         /**< Real (floating-point) data type */
    };

    /**
     * @brief Converts an sql_type_t value to its corresponding string representation.
     * @param t     The sql_type_t value to be converted.
     * @return The string representation of the sql_type_t value.
     */
    static consteval const char* sqlTypesStr(sql_type_t t) {
        switch (t) {
                case sql_type_t::INTEGER:{
                    return "INTEGER";
                }
                case sql_type_t::TEXT:{
                        return "TEXT";
                }
                case sql_type_t::BLOB:{
                        return "BLOB";
                }
                case sql_type_t::NUMERIC:{
                        return "NUMERIC";
                }
                case sql_type_t::REAL:{
                    return "REAL";
                }

                default:
                    return "not a supported type";
        }
    }

    /**
     * @enum order_t
     * @brief Enumerates various SQL ordering options.
     *
     * This enumeration provides options for specifying the ordering of
     * database query results.
     */
    enum class order_t{
        NONE,
        ASC,
        DESC
    };

    /**
     * @brief Converts an order_t value to its corresponding string representation.
     * @param t     The order_t value to be converted.
     * @return The string representation of the order_t value.
     */
    static constexpr const char* orderStr(order_t t){
        switch (t) {
            case order_t::NONE: {
                return "";
            }
            case order_t::ASC: {
                return "ASC";
            }
            case order_t::DESC: {
                 return "DESC";
            }
            default:
                return "order not supported";

        }
    }

    /**
     * @struct MemberTypeToSqlType
     * @brief Maps C++ member types to their corresponding SQL data types.
     * @tparam T    The C++ type to be mapped to an SQL data type.
     *
     * It uses type traits and conditional
     * compilation to determine the appropriate SQL data type.
     */
    template<typename T>
    struct MemberTypeToSqlType {
    private:
        static constexpr sql_type_t findType() {
            using type = typename remove_optional<T>::type;
            if constexpr (IsArithmetic<type>()) {
                if (std::is_floating_point_v<type>) {
                    return sql_type_t::REAL;
                } else {
                    return sql_type_t::INTEGER;
                }
            } else if constexpr (IsString<type>()) {
                return sql_type_t::TEXT;
            } else if constexpr (IsContinuousContainer<type>()) {
                return sql_type_t::BLOB;
            } else {
                static_assert(std::is_same_v<T, std::false_type>,
                              "Member type is not convertable to an sql type. "
                              "Please use only arithmetic types or containers with continuous memory, "
                              "or std::optional containing either type");
            }
        }
    public:
        /** The SQL data type value corresponding to the C++ type. */
        static constexpr sql_type_t value = findType();
    };

}// namespace trantor
