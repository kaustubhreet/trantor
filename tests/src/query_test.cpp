#include <gtest/gtest.h>
#include <filesystem>
#include "trantor.hpp"

using namespace trantor;

void loggerQueryTest(trantor::LogLevel level, const char* msg) {
    std::cout << "connection logger (" << (int)level << "): ";
    std::cout << msg << std::endl;
}

struct Object {
    int id = 0;
    int someId = 0;
    std::string someText;
    float someFloat;
};

using table_t = Table<"test", Object,
        Column<"id", &Object::id, column_constraint::PrimaryKey<conflict_t::ABORT>>,
        Column<"text", &Object::someText, column_constraint::Unique<conflict_t::REPLACE>>,
        Column<"float", &Object::someFloat>,
        Column<"someId", &Object::someId >
>;

using MyConnection = Connection<table_t>;

class QueryTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto createdConn = MyConnection::create("C:/sqlite3/test.db", 0, 0, &loggerQueryTest);
        if (!std::holds_alternative<MyConnection>(createdConn)) {
            throw "Unable to open connection";
        }

        myConn = std::make_shared<MyConnection>(std::move(std::get<MyConnection>(createdConn)));
    }

    std::shared_ptr<MyConnection> myConn;

    void TearDown() override {
        myConn = nullptr;
        std::filesystem::rename("C:/sqlite3/test.db", "C:/sqlite3/test.db.old");
    }
};

TEST_F(QueryTest, CreateTables) {
    auto err = myConn->createTables(false);
    if (err) std::cout << std::string(err.value()) << std::endl;
    ASSERT_FALSE(err);
}

TEST_F(QueryTest, CreateIfExistsTables) {
    auto err = myConn->createTables(true);
    if (err) std::cout << std::string(err.value()) << std::endl;
    ASSERT_FALSE(err);

    err = myConn->createTables(true);
    if (err) std::cout << std::string(err.value()) << std::endl;
    ASSERT_FALSE(err);
}

TEST_F(QueryTest, FindNothing) {
    auto createTablesErr = myConn->createTables(true);
    ASSERT_FALSE(createTablesErr);

    auto result = myConn->find<Object>(1);
    if (std::holds_alternative<Error>(result)) {
        auto err = std::get<Error>(result);
        std::cout << std::string(err) << std::endl;
        ASSERT_FALSE(true);
    } else {
        auto record = std::get<std::optional<Object>>(result);
        if (record.has_value()) {
            std::cout << record.value().id << std::endl;
            std::cout << record.value().someId << std::endl;
        }
        ASSERT_FALSE(record);
    }
}

TEST_F(QueryTest, InsertSomething) {
    auto err = myConn->createTables(true);
    if (err) {
        std::cout << std::string(err.value()) << std::endl;
    }
    ASSERT_FALSE(err);
    Object obj;

    obj.someText = "Some text";
    obj.someFloat = 3.14;
    obj.someId = 42;

    err = myConn->insert(obj);
    if (err) {
        std::cout << std::string(err.value()) << std::endl;
    }
    ASSERT_FALSE(err);
}

TEST_F(QueryTest, FindSomething) {
    auto err = myConn->createTables(true);
    ASSERT_FALSE(err);

    Object obj;
    obj.someText = "Some text";
    obj.someFloat = 3.14;
    obj.someId = 42;

    err = myConn->insert(obj);
    ASSERT_FALSE(err);

    auto result = myConn->find<Object>(1);
    if (std::holds_alternative<Error>(result)) {
        auto err = std::get<Error>(result);
        std::cout << std::string(err) << std::endl;
        ASSERT_FALSE(true);
    } else {
        auto record = std::get<std::optional<Object>>(result);
        ASSERT_TRUE(record);
        ASSERT_EQ(record.value().id, 1);
        ASSERT_EQ(record.value().someText, "Some text");
        ASSERT_EQ(record.value().someFloat, 3.14);
        ASSERT_EQ(record.value().someId, 37);
    }
}