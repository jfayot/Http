cmake_minimum_required(VERSION 3.16)

project(Http)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)

find_package(Boost 1.78.0 REQUIRED)
find_package(OpenSSL 1.1 	 REQUIRED)
find_package(nlohmann_json 3.10 REQUIRED)
find_package(fmt 8.1 REQUIRED)

set(HTTP_INCLUDE_FILES
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
  
  set(HTTP_SRC_FILES
  ${CMAKE_CURRENT_LIST_DIR}/src/EchoServer.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/example.cpp
)

add_executable(Http)

target_sources(Http
  PRIVATE
    ${HTTP_INCLUDE_FILES}
    ${HTTP_SRC_FILES}
)

target_include_directories(Http
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_compile_features(Http
  PUBLIC
    cxx_std_17
)

target_link_libraries(Http
  PUBLIC
    Boost::boost
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    fmt::fmt-header-only
)
