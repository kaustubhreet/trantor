#pragma once

#include "../common.hpp"
#include "column.hpp"
#include "constraint.hpp"
#include "types.hpp"
#include <optional>
#include <algorithm>
#include <iostream>
#include <string>


namespace trantor{

    template <typename Column, typename Table>
    concept ColumnBelongingToClass = std::is_same<typename Column::ObjectClass, Table>::value;

    template <FixedLengthString tableName, typename T, ColumnBelongingToClass<T>... Column>
    class Table{
    private:
        template<typename... C>
        struct FindPrimaryKey : std::false_type {};

        template<typename... C>
        requires AnyOf<Column::isPrimaryKey...>
        struct FindPrimaryKey<C...> : std::false_type {
            static constexpr int primaryKeyIndex = IndexOfFirst<C::isPrimaryKey...>::value;
            using type = typename std::tuple_element<primaryKeyIndex, std::tuple<Column...>>::type;
        };

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

        static constexpr bool hasPrimaryKey = AnyOf<Column::isPrimaryKey...>;
        using PrimaryKey = typename FindPrimaryKey<Column...>::type;
        using ObjectClass = T;
        using columns_t = std::tuple<Column...>;

        //std::optional<Error> create() { return std::nullopt; }

        static std::string createTableQuery(bool ifNotExist) {
            std::ostringstream query;

            query<<"CREATE TABLE ";
            if(ifNotExist)
                    query << "IF NOT EXISTS ";
            query<<tableName.value<<" (" <<std::endl;
            ([&]{
                query << '\t'
                << Column::name()<< " "
                << sqlTypesStr(Column::SQLMemberType);

                auto constraints = Column::constraintCreationQuery();
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
            << "WHERE `" << PrimaryKey::name
            << "` = ?;";

            return ss.str();
        }

        static std::string insertQuery() {
            std::ostringstream ss;
            ss << "INSERT INTO `" << tableName.value << "` (";

            size_t queryColumns = numberOfColumns;
            std::apply([&](const auto&... a) {
                ([&]() {
                    using column_t = std::remove_reference_t<decltype(a)>;
                    if constexpr (column_t::isAutoIncColumn) {
                        queryColumns--;
                    } else {
                        ss << "`" << column_t::name() << "`, ";
                    }
                }(), ...);
            }, columns_t{});

            std::string str = ss.str();
            str.erase(str.end() - 2, str.end());

            std::ostringstream ss2;
            ss2 << ") VALUES (";
            for (size_t i = 0; i < queryColumns; i++) {
                if (i == queryColumns - 1) {
                    ss2 << "?);";
                } else  {
                    ss2 << "?, ";
                }
            }

            return str + ss2.str();
        }
    };
}



