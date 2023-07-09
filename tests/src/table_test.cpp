#include <gtest/gtest.h>
#include <filesystem>
#include "trantor.hpp"

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
    int _id;
    std::string _name;
    auto getId() { return _id; }
    void setId(int id) { _id = id; }
    auto getName() { return _name; }
    void setName(std::string name) { _name = name; }
};

TEST_F(TableTest, Columns){
    Table<"test", Object,
        Column<"id", &Object::_id>,
        Column<"name", &Object::_name> > table;

    Object o;
    o.setId(10);
    o.setName("Steve");
    table.printColumns(o);

    ASSERT_EQ(table.columnName(0), "id");
    ASSERT_EQ(table.columnName(1), "name");
}

TEST_F(TableTest, ColumnPrivate){
    Table<"test", Object,
        ColumnPrivate<"id", &Object::getId, &Object::setId>,
        ColumnPrivate<"name", &Object::getName, &Object::setName> > table;

    Object o;
    o.setId(100);
    o.setName("Steve");
    table.printColumns(o);

    ASSERT_EQ(table.columnName(0), "id");
    ASSERT_EQ(table.columnName(1), "name");
}