#pragma once

#include "common.hpp"
#include <optional>
#include <algorithm>


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

template <ColumnName columnName, typename C, typename T, T (C::*Getter)(), void (C::*Setter)(T)>
class ColumnPrivate{
public:
    static constexpr const char* name(){
        return columnName.value;
    }
    static auto getter(C obj){
        return (obj.*Getter)();
    };

    static void setter(C obj, T arg){
        obj.*Setter(arg);
    };
};


template <ColumnName columnName, typename C, typename T, T (C::*value)>
class Column{
public:
    static constexpr const char* name(){
        return columnName.value;
    }
    static auto getter(C obj){
        return obj.*value;
    };

    static void setter(C obj, T arg){
        obj.*value = arg;
    };
};
