cmake_minimum_required(VERSION 3.16)

# lib's version
set(version 1.0.0)

project(Http
  VERSION ${version}
  LANGUAGES CXX
)

# make cache variables for install destinations
include(GNUInstallDirs)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=c8cdc32c03816538ce22781ed72964dc864b2a34a310d3b7104812a5ca2d835d
)

include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)
CPMUsePackageLock(package-lock.cmake)

# check if project is standalone or embedded
if (NOT DEFINED IS_ROOT_PROJECT)
  set(IS_ROOT_PROJECT OFF)
  if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(IS_ROOT_PROJECT ON)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
  endif ()
endif ()

option(BUILD_TESTS "Build tests" ${IS_ROOT_PROJECT})
option(ENABLE_COVERAGE "Enable coverage" ${IS_ROOT_PROJECT})

CPMGetPackage(OpenSSL)
CPMGetPackage(CPMPackageProject)
CPMGetPackage(CPMLicenses)
CPMGetPackage(Ccache)
CPMGetPackage(Boost)
CPMGetPackage(fmt)
CPMGetPackage(nlohmann_json)

cpm_licenses_create_disclaimer_target(write-licenses
  "${CMAKE_CURRENT_BINARY_DIR}/third_party.txt"
  "${CPM_PACKAGES}"
)

add_library(http_client INTERFACE
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

add_library(Http::client ALIAS http_client)

target_include_directories(http_client
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(http_client INTERFACE cxx_std_17)

target_link_libraries(http_client
  INTERFACE
    Boost::beast
    Boost::uuid
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    fmt::fmt-header-only
)

add_dependencies(http_client write-licenses)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/third_party.txt"
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/include/Http
)

packageProject(
  NAME http_client
  VERSION ${PROJECT_VERSION}
  NAMESPACE Http
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include
  VERSION_HEADER Http/version.h
  DISABLE_VERSION_SUFFIX YES
  COMPATIBILITY SameMajorVersion
  DEPENDENCIES "OpenSSL 3.3.1; fmt 11.0.2; Boost 1.85.0; nlohmann_json 3.11.3"
)
