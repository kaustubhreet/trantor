#pragma once

namespace trantor{
    enum class sql_type_t{
        INTEGER,
        TEXT,
        BLOB,
        NUMERIC
    };

    static constexpr const char* sqlTypesStr(sql_type_t t) {
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

        }

        return "type not supported";
    }


    template<typename T>
    struct MemberTypeToSqlType{
        static constexpr const sql_type_t value = sql_type_t::BLOB;
    };

#define DEFINE_MEMBER_TO_SQL(memberType, sqlType) \
    template<> \
    struct MemberTypeToSqlType<memberType>{ \
        static constexpr const sql_type_t value = sql_type_t::sqlType; \
    };

#define DEFINE_MEMBER_TO_SQL(int, INTEGER);
#define DEFINE_MEMBER_TO_SQL(float, REAL);
#define DEFINE_MEMBER_TO_SQL(bool, INTEGER);
#define DEFINE_MEMBER_TO_SQL(void, BLOB);
#undef DEFINE_MEMBER_TO_SQL

}
