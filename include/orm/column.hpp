#pragma once

#include <common.hpp>
#include "constraint.hpp"
#include <sstream>
#include "types.hpp"
#include <type_traits>

namespace trantor{

    namespace __constraint_t_detail{
        template<typename MemberT, typename... C>
        struct constraints_t {
            using type = unique_tuple<C...>;
        };

        template <typename MemberT, typename... C>
        requires (not is_optional<MemberT>::value)
        struct constraints_t<MemberT, C...> {
            using type = unique_tuple<column_constraint::NotNull<>, C...>;
        };

        static inline std::string constraintCreationQuery(auto constraints) {
            std::stringstream ss;
            std::apply([&] (const auto&... c) {
                ([&](){
                    ss << c << " ";
                }(), ...);
            }, constraints);

            std::string qstr = ss.str();
            // erase trailing comma
            if (qstr.size() >= 2) {
                qstr.erase(qstr.end() - 1);
            }

            return qstr;
        }
    }
    template <trantor::FixedLengthString columnName, auto Getter, auto Setter= nullptr, class... Constraint>
    class ColumnP {
    private:
        template<typename T>
        struct resolve_function_ptr_types : std::false_type {
            using argType = std::false_type;
            using returnType = void;
            using klass = std::false_type;
        };

        //pointer to setter
        template<typename C, class A>
        struct resolve_function_ptr_types<void (C::*)(A)> {
            using argType = A;
            using returnType = void;
            using klass = C;
        };

        //pointer to getter
        template<typename R, typename C>
        struct resolve_function_ptr_types<R (C::*)()> {
            using argType = std::false_type;
            using returnType = R;
            using klass = C;
        };

        //pointer to member
        template<typename R, typename C>
        struct resolve_function_ptr_types<R C::*> {
            using argType = std::false_type;
            using returnType = void;
            using klass = std::false_type;
        };

        using SetterResolved = resolve_function_ptr_types<decltype(Setter)>;
        using GetterResolved = resolve_function_ptr_types<decltype(Getter)>;

        static_assert(!std::is_same<typename SetterResolved::argType, std::false_type>::value,
                      "Column template argument should be a pointer to a class method that sets the column data");
        static_assert(std::is_same<typename SetterResolved::returnType, void>::value,
                      "Column template argument should be a pointer to a class method that sets the column data. The return type should be `void`");

        static_assert(std::is_same<typename GetterResolved::argType, std::false_type>::value,
                      "Column template argument should be a pointer to a class method that gets the column data");
        static_assert(!std::is_same<typename GetterResolved::returnType, void>::value,
                      "Column template argument should be a pointer to a class method that gets the column data. The return type should not be `void`");

        static_assert(std::is_same<typename GetterResolved::returnType, typename SetterResolved::argType>::value,
                      "Column template arguments should be a pointers to class methods that get and set the column data");

    public:

        static constexpr bool publicColumn = false;
        using MemberType = SetterResolved::argType;
        using ObjectClass = SetterResolved::klass;
        static constexpr bool isPrimaryKey = AnyOf<column_constraint::ConstraintIsPrimaryKey<Constraint>::value...>;
        using constraints_t = typename __constraint_t_detail::constraints_t<MemberType, Constraint...>::type;


        static constexpr const char *name() {
            return columnName.value;
        }

        static constexpr trantor::sql_type_t SQLMemberType = trantor::MemberTypeToSqlType<MemberType>::value;
        static constexpr bool isAutoIncColumn =
                AnyOf<column_constraint::ConstraintIsPrimaryKey<Constraint>::value...> &&
                SQLMemberType == sql_type_t::INTEGER;

        static auto &getter(auto &obj) {
            return (obj.*Getter)();
        }

        static void setter(auto &obj, auto arg) {
            obj.*Setter(arg);
        }

        static std::string constraintCreationQuery() {
            return __constraint_t_detail::constraintCreationQuery(constraints_t{});
        }

    };


    template <trantor::FixedLengthString columnName, auto M, class... Constraint>
    class Column{
    private:
        template<typename T>
        struct find_column_type : std::false_type {
            using type = std::false_type;
        };

        template<typename R, typename C, class A>
        struct find_column_type<R (C::*)(A)>
        {
            using type = std::false_type;
        };

        template<typename R, typename C>
        struct find_column_type<R C::*>
        {
            using type = R;
            using klass = C;
        };

    public:
        static constexpr bool publicColumn = true;
        using MemberType = find_column_type<decltype(M)>::type;

        static_assert(!std::is_same<MemberType, std::false_type>::value,
                      "Column template argument should be a pointer to a class member");

        using ObjectClass = find_column_type<decltype(M)>::klass;
        using constraints_t = typename __constraint_t_detail::constraints_t<MemberType, Constraint...>::type;
        static constexpr sql_type_t SQLMemberType = MemberTypeToSqlType<MemberType>::value;
        static constexpr bool isPrimaryKey = AnyOf<column_constraint::ConstraintIsPrimaryKey<Constraint>::value...>;
        static constexpr bool isAutoIncColumn = AnyOf<column_constraint::ConstraintIsPrimaryKey<Constraint>::value...> && SQLMemberType == sql_type_t::INTEGER;


        static constexpr const char* name(){
            return columnName.value;
        }

        static auto& getter(auto& obj){
            return obj.*M;
        };

        static void setter(auto& obj, auto arg){
            obj.*M = arg;
        };

        static std::string constraintCreationQuery() {
            return __constraint_t_detail::constraintCreationQuery(constraints_t{});
        }
    };
}
