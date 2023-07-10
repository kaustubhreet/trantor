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
                <<MemberTypeToSqlType<typename Column::MemberType>::value << "," <<std::endl;
            }(), ...);

            std::string qstr = query.str();
            qstr.erase(qstr.end() -2);

            return qstr + " );";
        }
    };
}


template <trantor::FixedLengthString columnName, auto Getter, auto Setter>
class ColumnPrivate{
private:
    template<typename T>
    struct find_column_type : std::false_type {};

    template<typename R, typename C, class A>
    struct find_column_type<R (C::*)(A)>
    {
        typedef A type;
        typedef C klass;
    };

public:

    using MemberType = find_column_type<decltype(Setter)>::type;
    using ObjectClass = find_column_type<decltype(Setter)>::klass;

    static constexpr const char* name(){
        return columnName.value;
    }

    static constexpr const char* type() {
        return "INT";
    }

    static auto getter(auto obj){
        return (obj.*Getter)();
    };

    static void setter(auto obj, auto arg){
        obj.*Setter(arg);
    };
};


template <trantor::FixedLengthString columnName, auto M>
class Column{
private:
    template<typename T>
    struct find_column_type : std::false_type {};

    template<typename R, typename C>
    struct find_column_type<R C::*>
    {
        typedef R type;
        typedef C klass;
    };
public:

    using MemberType = find_column_type<decltype(M)>::type;
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
};
