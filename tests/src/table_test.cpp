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
    int _someId = 0;
    float _someFloat;
    std::string _name;
    std::string _someText;
    auto getId() { return _id; }
    void setId(int id) { _id = id; }
    auto getName() { return _name; }
    void setName(std::string name) { _name = name; }
};

using table_t = Table<"test", Object,
    Column<"id", &Object::_id>,
    Column<"name", &Object::_name>>;

using tablepriv_t = Table<"test", Object,
        ColumnP<"id", &Object::getId, &Object::setId>,
        ColumnP<"name", &Object::getName, &Object::setName>>;

using table_with_constraint_t = Table<"test_constraints", Object,
    Column<"id", &Object::_id, column_constraint::PrimaryKey<conflict_t::ABORT>>,
    Column<"name", &Object::_name, column_constraint::NotNull<>, column_constraint::Unique<>>,
    Column<"text", &Object::_someText, column_constraint::Unique<conflict_t::REPLACE>> >;


TEST_F(TableTest, Columns){
    table_t table;

    ASSERT_EQ(table.columnName(0), "id");
    ASSERT_EQ(table.columnName(1), "name");
}

TEST_F(TableTest, numberColumns){
    ASSERT_EQ(table_t::numberOfColumns, 2);
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

TEST_F(TableTest, CreateWithConstraintsTableQuery){
    std::string query = table_with_constraint_t::createTableQuery(false);
    std::cout<<query;
    std::regex reg("\\s+");
    auto trimmed = std::regex_replace(query, reg, " ");
    std::string expected = "CREATE TABLE test_constraints ( "
                           "id INTEGER PRIMARY KEY ON CONFLICT ABORT, "
                           "name TEXT NOT NULL ON CONFLICT ABORT UNIQUE ON CONFLICT ABORT, "
                           "text TEXT UNIQUE ON CONFLICT REPLACE "
                           ");";
    ASSERT_EQ(trimmed, expected);
}