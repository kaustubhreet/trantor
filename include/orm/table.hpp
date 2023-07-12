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
       static constexpr std::string columnName(int index){
            int i = 0;
            std::string name;
           ((name = i == index
                   ? Column::name()
                   : name, i++), ...);

           return name;
        }

        static constexpr int numberOfColumns = std::tuple_size(std::tuple<Column...>());

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
                << sqlTypesStr(Column::SQLMemberType) << "," <<std::endl;
            }(), ...);

            std::string qstr = query.str();
            qstr.erase(qstr.end() -2);

            return qstr + " );";
        }
    };
}



