#pragma once

#include "../common.hpp"
#include "column.hpp"
#include "constraint.hpp"
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
       static constexpr std::string columnName(int index){
            int i = 0;
            std::string name;
           ((name = i == index
                   ? Column::name()
                   : name, i++), ...);

           return name;
        }

        static constexpr auto numberOfColumns = sizeof...(Column);

        std::optional<Error> create() { return std::nullopt; }

        static std::string createTableQuery(bool ifNotExist) {
            std::ostringstream query;

            query<<"CREATE TABLE ";
            if(ifNotExist)
                    query << "IF NOT EXIST ";
            query<<tableName.value<<" (" <<std::endl;
            ([&]{
                query << '\t'
                << Column::name()<< " "
                << sqlTypesStr(Column::SQLMemberType);

                auto constraints = Column::creationConstraints();
                if(!constraints.empty()){
                    query << " " << constraints;
                }
                query << "," <<std::endl;
            }(), ...);

            std::string qstr = query.str();
            qstr.erase(qstr.end() -2);

            return qstr + " );";
        }

        static std::string findQuery(){
            std::ostringstream ss;
            ss << "SELECT * FROM `" << tableName.value << "` "
            << "WHERE `" << column_constraint::PrimaryKey<>::to_string()
            << "` = ?;";

            return ss.str();
        }
    };
}


public:

    using MemberType = find_column_type<decltype(M)>::type;

    static_assert(!std::is_same<MemberType, std::false_type>::value,
            "Column template argument should be a pointer to a class member");

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



