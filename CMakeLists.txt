cmake_minimum_required(VERSION 3.16)

project(Http)

include(cmake/CPM.cmake)

# check if project is standalone or embedded
if (NOT DEFINED IS_ROOT_PROJECT)
  set(IS_ROOT_PROJECT OFF)
  if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(IS_ROOT_PROJECT ON)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
  endif ()
endif ()

option(BUILD_TESTS "Build tests" ${IS_ROOT_PROJECT})

CPMAddPackage(
  NAME nlohmann_json
  VERSION 3.11.3
  GITHUB_REPOSITORY nlohmann/json
  OPTIONS
    "JSON_BuildTests OFF"
    "JSON_MultipleHeaders OFF"
)

CPMAddPackage(
  NAME fmt
  GIT_TAG 11.0.2
  GITHUB_REPOSITORY fmtlib/fmt
  EXCLUDE_FROM_ALL True
)

CPMAddPackage(
  NAME Boost
  VERSION 1.84.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  OPTIONS
    "BOOST_ENABLE_CMAKE ON"
    "BOOST_INCLUDE_LIBRARIES beast\\\;uuid"
  EXCLUDE_FROM_ALL True
)

find_package(OpenSSL 1.1 REQUIRED)

add_library(Http INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/Base64.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/basic_file_body.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/basic_multipart_body.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/basic_string_body.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/empty_body.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/HttpClientImpl.h
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/HttpClientImpl.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/HttpFile.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/HttpISession.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/HttpRequestImpl.h
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/HttpRequestImpl.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/HttpSessionBase.h
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/HttpSessionBase.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/HttpSessionPlain.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/detail/HttpSessionSsl.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/HttpAuth.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/HttpBody.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/HttpClient.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/HttpFormData.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/HttpForwards.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/HttpMimeTypes.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/HttpOptions.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/HttpRequest.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/HttpResponse.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Http/HttpTypes.hpp
)

add_library(Http::client ALIAS Http)

target_include_directories(Http INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_compile_features(Http INTERFACE cxx_std_17)

target_link_libraries(Http
  INTERFACE
    Boost::beast
    Boost::uuid
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    fmt::fmt-header-only
)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()

