#pragma once

#include "../common.hpp"
#include <optional>
#include <algorithm>
#include <iostream>


namespace trantor{

    template <FixedLengthString tableName, typename T, typename... Column>
    class Table{
    public:
        void printColumns(T &obj) {
            int i = 0;
            ((
                    std::cout << "column[" << i << "] " << Column::getter(obj) << std::endl, i++
            ), ...);
        }

       static constexpr std::string columnName(int index){
            int i = 0;
            std::string name;
           ((name = i == index ? Column::name(): name, i++), ...);
           return name;
        }

        std::optional<Error> create() { return std::nullopt; }
    };
}


template <trantor::FixedLengthString columnName, auto Getter, auto Setter>
class ColumnPrivate{
public:
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
public:
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
