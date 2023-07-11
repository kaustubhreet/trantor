#pragma once

#include "common.hpp"

namespace trantor{
    enum class conflict_t {
        ABORT,
        ROLLBACK,
        FAIL,
        IGNORE,
        REPLACE
    };

    static constexpr char* conflictStr(conflict_t t){
        switch(t){
            case conflict_t::ABORT: {
                return "ABORT";
            }
            case conflict_t::ROLLBACK: {
                return "ROLLBACK";
            }
            case conflict_t::FAIL: {
                return "FAIL";
            }
            case conflict_t::IGNORE: {
                return "IGNORE";
            }
            case conflict_t::REPLACE: {
                return "REPLACE";
            };

            return "unknown conflict";

        }
    }
}
