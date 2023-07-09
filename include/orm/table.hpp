#pragma once

#include "../common.hpp"
#include <optional>
#include <algorithm>
#include <iostream>


namespace trantor{

    template <typename T, typename... Column>
    class Table{
    public:
        void printColumns(T &obj) {
            ([&]
            {
                std::cout << "column: " << Column::getter(obj) << std::endl;
            }(), ...);
        }

        std::optional<Error> create(){
            return std::nullopt;
        }
    };
}

template <size_t N>
struct ColumnName{
    constexpr ColumnName(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    char value[N];
};

template <ColumnName columnName, auto Getter, auto Setter>
class ColumnPrivate{
public:
    static constexpr const char* name(){
        return columnName.value;
    }
    static auto getter(auto obj){
        return (obj.*Getter)();
    };

    static void setter(auto obj, auto arg){
        obj.*Setter(arg);
    };
};


template <ColumnName columnName, auto M>
class Column{
public:
    static constexpr const char* name(){
        return columnName.value;
    }
    static auto getter(auto obj){
        return obj.*M;
    };

    static void setter(auto obj, auto arg){
        obj.*M = arg;
    };
};
