# HTTP library

A header-only HTTP client library based on boost::beast and nlohmann::json.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/jfayot/Http/blob/main/LICENSE)

[![codecov](https://codecov.io/github/jfayot/Http/graph/badge.svg?token=5I11CHFIZI)](https://codecov.io/github/jfayot/Http)
[![Windows Build Status](https://github.com/jfayot/Http/actions/workflows/tests.yml/badge.svg)](https://github.com/jfayot/Http/actions/workflows/tests.yml)

## Features

* GET, PUT, POST, PATCH and DELETE resources from/to a REST service
* HTTP/HTTPS
* Basic and Bearer token authentication
* Post FormData
* Asynchronous response wrapped into future
* Upload/Download files

## Dependencies

* OpenSSL (through [openssl-cmake](https://github.com/jimmy-park/openssl-cmake))
* Boost::beast
* Boost::uuid
* nlohmann::json
* fmtlib::fmt

All dependencies are pulled and compiled thanks to [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake)

For Windows platform, you'll need to have NASM installed and your PATH pointing at it to build OpenSSL correctly.

## Build

Use cmake to configure, build and install:

```console
cmake -B build -DCMAKE_INSTALL_PREFIX=<your_install_dir>
cmake --build build --target install
```

## Include directly into your project

Use [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) to include directly into your project:

```cmake
cmake_minimum_required(VERSION 3.16)

project(your_project
  LANGUAGES CXX
)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=c8cdc32c03816538ce22781ed72964dc864b2a34a310d3b7104812a5ca2d835d
)

include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage("gh:jfayot/Http@1.0.0")

add_executable(your_target ${CMAKE_CURRENT_LIST_DIR}/main.cpp)

target_link_libraries(your_target PRIVATE Http::client)
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

  return 0;
}
```
