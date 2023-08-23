# Trantor 

Welcome to Trantor, an ORM written in C++ 20 for SQLite databases! Trantor is an Object Relational Mapper (ORM) that allows developers to interact with a SQLite database using object-oriented programming principles. Utilizing the power of C++20 and template metaprogramming, Trantor provides a simple, robust, and efficient way to interact with SQLite databases.

## Features

- Ensured type safety during compilation; Avoided code generation
- Minimized the need for repetitive code
- No need to inherit from specific sources or adhere to predefined interfaces
- Utilized SQL-esque syntax for composing complex queries without resorting to raw SQL
- Eliminated the possibility of creating invalid queries"

## Using Trantor

1. Write your objects

```cpp
struct Object {
    int id = 0;
    std::string some_text;
};

```
`trantor` doesn't provide a base class or anything like that. The types for the
SQL table are instead inferred from types used in the C++ code.

2. Create a `Table` for your object
```cpp
using ObjectTable = trantor::Table<"objects", Object,
    trantor::Column<"id", &Object::id, trantor::PrimaryKey<>>,
    trantor::Column<"some_text", &Object::some_text>
>;
```
This is the mapping that will tell `trantor` how to deal with `Object` and what table
in the database it is referring to.

3. Create a connection
```cpp
auto connection = trantor::Connection<ObjectTable>("./path/to/my/data.db");
```
The connection template accepts a list of tables, and should contain all the tables
that your application is going to work with.
