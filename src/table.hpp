#pragma once

#include "./common.hpp"
#include <optional>

namespace trantor{
    /**
     *
     * @tparam t
     * @tparam Column
     */
    template <typename t, typename... Column>
    class Table{
    public:
        void printColumns(T &obj){
            (
                    [&]{
                        std::cout<<"column: "<<Column::getter(obj) << std::endl;
                    })(), ...);
        }

        std::optional<Error> create(){
            return std::nullopt;
        }
    };
}

/**
 *
 * @tparam C
 * @tparam T
 * @tparam Getter
 * @tparam Setter
 */
template <typename C, typename T, T (C::*Getter)(), void (C::*Setter)(T)>
class ColumnPrivate{
public:
    static auto getter(C obj){
        return (obj.*Getter)();
    };

    static void setter(C obj, T arg){
        obj.*Setter(arg);
    };
};

/**
 * @brief
 * @tparam C
 * @tparam T
 * @tparam value
 */
template <typename C, typename T, T (C::*value)>
class Column{
public:
    static auto getter(C obj){
        return obj.*value;
    };

    static void setter(C obj, T arg){
        obj.*value = arg;
    };
};
