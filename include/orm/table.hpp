#pragma once

#include "../common.hpp"
#include "types.hpp"
#include <optional>
#include <algorithm>
#include <iostream>


namespace trantor{

    template <typename Column, typename Table>
    concept ColumnBelongingToClass = std::is_same<typename Column::ObjectClass, Table>::value;

    template <FixedLengthString tableName, typename T, ColumnBelongingToClass<T>... Column>
    class Table{
    public:
        static constexpr void printTable() {
            int i = 0;
            std::cout << "Table: " << type_name<Table<tableName, T, Column...>>() << std::endl;

            std::cout << "name: " << tableName.value << std::endl;

            ([&]{
                std::cout
                << "column[" << i << "]:"
                << Column::name() << " | "
                << type_name<typename Column::MemberType>() << " | "
                << MemberTypeToSqlType<typename Column::MemberType>::value << " | "
                << std::endl;

                i++;
            }(), ...);

        }

       static constexpr std::string columnName(int index){
            int i = 0;
            std::string name;
           ((name = i == index
                   ? Column::name()
                   : name, i++), ...);

           return name;
        }

        static constexpr int numberOfColumns(){
            return std::tuple_size(std::tuple<Column...>());
        }

        std::optional<Error> create() { return std::nullopt; }

        static std::string createTableQuery() {
            std::ostringstream query;

            query<<"CREATE TABLE "<<tableName.value<<" (" <<std::endl;
            ([&]{
                query << '\t'
                << Column::name()<< " "
                << sqlTypesStr(Column::SQLMemberType) << "," <<std::endl;
            }(), ...);

            std::string qstr = query.str();
            qstr.erase(qstr.end() -2);

            return qstr + " );";
        }
    };
}


template <trantor::FixedLengthString columnName, auto Getter, auto Setter= nullptr>
class Column{
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
    struct resolve_function_ptr_types<R (C::*)()>
    {
        using argType = std::false_type;
        using returnType = R;
        using klass = C;
    };

    //pointer to member
    template<typename R, typename C>
    struct resolve_function_ptr_types<R C::*>
    {
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

    using MemberType = SetterResolved::argType;
    using ObjectClass = SetterResolved::klass;

    static constexpr const char* name(){
        return columnName.value;
    }

    static constexpr trantor::sql_type_t SQLMemberType = trantor::MemberTypeToSqlType<MemberType>::value;

    static auto getter(auto obj){
        return (obj.*Getter)();
    };

    static void setter(auto obj, auto arg){
        obj.*Setter(arg);
    };
};


template <trantor::FixedLengthString columnName, auto M>
class Column<columnName, M, nullptr>{
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

    using MemberType = find_column_type<decltype(M)>::type;

    static_assert(!std::is_same<MemberType, std::false_type>::value,
            "Column template argument should be a pointer to a class memebr");

    using ObjectClass = find_column_type<decltype(M)>::klass;

    static constexpr const char* name(){
        return columnName.value;
    }

    static constexpr const char* type() {
        return "INT";
    }

    static auto getter(auto obj){
        return obj.*M;
    };

    static void setter(auto obj, auto arg){
        obj.*M = arg;
    };

    static constexpr trantor::sql_type_t SQLMemberType = trantor::MemberTypeToSqlType<MemberType>::value;
};
