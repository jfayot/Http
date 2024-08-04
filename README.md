# HTTP library

A header-only HTTP client library based on boost::beast and nlohmann::json.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/jfayot/Http/blob/main/LICENSE)

[![codecov](https://codecov.io/github/jfayot/Http/graph/badge.svg?token=5I11CHFIZI)](https://codecov.io/github/jfayot/Http)
[![Windows Build Status](https://github.com/jfayot/Http/actions/workflows/tests.yml/badge.svg)](https://github.com/jfayot/Http/actions/workflows/tests.yml)

## Features

*   GET, PUT, POST, PATCH and DELETE resources from/to a REST service
*   HTTP/HTTPS
*   Basic and Bearer token authentication
*   Post FormData
*   Asynchronous response wrapped into future
*   Upload/Download files

## Dependencies

*   OpenSSL
*   Boost::beast
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
  Http::Request request = client.post("/persons").body(person);
  std::shared_future<Http::Response> future = request.send();

  // Do whatever needed
  // ...

  Http::Response res = future.get();

  if (true == res.ok())
  {
    Person content = res.body().get<Person>();
  }
}
```
