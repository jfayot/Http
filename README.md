# HTTP library

A header-only HTTP library based on boost::beast and nlohmann::json.

## Features

*   GET, PUT, POST, PATCH and DELETE resources from/to a REST service
*   HTTP/HTTPS
*   Basic and Bearer token authentication
*   Post FormData (Get FormData not yet implemented)
*   Asynchronous response wrapped into future
*   Upload/Download files

## Dependencies

*   Boost::beast
*   OpenSSL
*   nlohmann::json
*   fmtlib::fmt

All dependencies, except OpenSSL, are pulled and compiled thanks to CPM.cmake

## Build


Use cmake to configure and build:

```console
cmake -S . -B build
cmake --build build 
```

## Minimal example

```c++
#include "Http/HttpTypes.hpp"
struct Person
{
    std::string name;
    int age;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Person, name, age)
};

int main()
{
    Person person{ "captain", 42 };
    
    Http::Client client{ "http://127.0.0.1:8080" };

    auto res = client.post("/Persons").body(person).send().get();

    if (true == res.ok()) ...
```
