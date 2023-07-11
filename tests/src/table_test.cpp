#include <gtest/gtest.h>
#include <filesystem>
#include "trantor.hpp"
#include <regex>

using namespace trantor;

void logger(trantor::LogLevel level, const char* msg) {
    std::cout << "connection logger (" << (int)level << "): ";
    std::cout << msg << std::endl;
}

class TableTest : public ::testing::Test {
protected:
    void SetUp() override{
        auto createdConn = Connection::Create("test.db", 0, 0, &logger);
        if(!std::holds_alternative<Connection>(createdConn)) {
            throw "Unable to open connection";
        }

        myConn = std::make_shared<Connection>(std::move(std::get<Connection>(createdConn)));
    }

    std::shared_ptr<Connection> myConn;

    void TearDown() override{
        myConn = nullptr;
        std::filesystem::remove("test.db");
    }
};

struct Object{
    int _id = 0;
    std::string _name;
    auto getId() { return _id; }
    void setId(int id) { _id = id; }
    auto getName() { return _name; }
    void setName(std::string name) { _name = name; }
};

using table_t = Table<"test", Object,
    Column<"id", &Object::_id>,
    Column<"name", &Object::_name>>;

using tablepriv_t = Table<"test", Object,
        Column<"id", &Object::getId, &Object::setId>,
        Column<"name", &Object::getName, &Object::setName>>;


TEST_F(TableTest, Columns){
    table_t table;

    ASSERT_EQ(table.columnName(0), "id");
    ASSERT_EQ(table.columnName(1), "name");
}

TEST_F(TableTest, ColumnPrivate){
    tablepriv_t table;

    //table.printTable();

    ASSERT_EQ(table.columnName(0), "id");
    ASSERT_EQ(table.columnName(1), "name");
}

TEST_F(TableTest, CreateTableQuery){
    std::string query = table_t::createTableQuery(false);
    std::cout<<query;
    std::regex reg("\\s+");
    auto trimmed = std::regex_replace(query, reg, " ");

    ASSERT_EQ(trimmed, "CREATE TABLE test ( id INTEGER, name TEXT );");

    std::string same = tablepriv_t::createTableQuery(false);
    ASSERT_EQ(same, query);
}