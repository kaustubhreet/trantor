#pragma once

#include "common.hpp"
#include <string>
#include <sstream>

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

    namespace column_constraint{
        template<FixedLengthString value>
        struct Default{
            static const std::string to_string(){
                std::stringstream ss;
                ss << "DEFAULT '" << value.value << "'";
                return ss.str();
            }
        };

        template<FixedLengthString value>
        struct Collate{
            static const std::string to_string(){
                std::stringstream ss;
                ss << "COLLATE '" << value.value << "'";
                return ss.str();
            }
        };

        template<FixedLengthString constraint, conflict_t onConflict>
        struct ConstraintWithConflictClause{
            static std::string to_string(){
                std::stringstream ss;
                ss << constraint.value << "ON CONFLICT" << conflictStr(onConflict);
                return ss.str();
            }
        };

        template<conflict_t onConflict=conflict_t::ABORT>
        using Unique = ConstraintWithConflictClause<"UNIQUE", onConflict>;

        template<conflict_t onConflict=conflict_t::ABORT>
        using NotNull = ConstraintWithConflictClause<"NOT NULL", onConflict>;

        template<conflict_t onConflict=conflict_t::ABORT>
        using PrimaryKey = ConstraintWithConflictClause<"PRIMARY KEY", onConflict>;

        template<conflict_t onConflict=conflict_t::ABORT>
        using PrimaryKeyAsc = ConstraintWithConflictClause<"PRIMARY KEY ASC", onConflict>;

        template<conflict_t onConflict=conflict_t::ABORT>
        using PrimaryKeyDesc = ConstraintWithConflictClause<"PRIMARY KEY DESC", onConflict>;
    }
}
