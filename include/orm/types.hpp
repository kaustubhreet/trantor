#pragma once

namespace trantor{
    enum class sql_type_t{
        INTEGER,
        TEXT,
        BLOB,
        NUMERIC,
        REAL
    };

    static consteval const char* sqlTypesStr(sql_type_t t) {
        switch (t) {
                case sql_type_t::INTEGER:{
                    return "INTEGER";
                }
                case sql_type_t::TEXT:{
                        return "TEXT";
                }
                case sql_type_t::BLOB:{
                        return "BLOB";
                }
                case sql_type_t::NUMERIC:{
                        return "NUMERIC";
                }
                case sql_type_t::REAL:{
                    return "REAL";
                }

        }

        return "type not supported";
    }


    template<typename T>
    struct MemberTypeToSqlType{
        static constexpr const sql_type_t value = sql_type_t::BLOB;
    };

#define DEFINE_MEMBER_TO_SQL_TYPE(memberType, sqlType) \
    template<> \
    struct MemberTypeToSqlType<memberType>{ \
        static constexpr const sql_type_t value = sql_type_t::sqlType; \
    };

DEFINE_MEMBER_TO_SQL_TYPE(int, INTEGER);
DEFINE_MEMBER_TO_SQL_TYPE(float, REAL);
DEFINE_MEMBER_TO_SQL_TYPE(bool, INTEGER);
DEFINE_MEMBER_TO_SQL_TYPE(void*, BLOB);
DEFINE_MEMBER_TO_SQL_TYPE(std::string, TEXT);
#undef DEFINE_MEMBER_TO_SQL

}
