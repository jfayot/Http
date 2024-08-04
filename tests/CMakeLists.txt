cmake_minimum_required(VERSION 3.18)

CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  GIT_TAG release-1.12.1
  VERSION 1.12.1
  OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
  ECLUDE_FROM_ALL True
)

add_executable(tests
  main.cpp
)

if (MSVC)
  target_compile_options(tests PRIVATE /bigobj)
endif ()

target_link_libraries(tests
  PRIVATE
    Http::client
    gtest
    gtest_main
    gmock
)

if (ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES GNU AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(tests
      PRIVATE
        "--coverage"
      )
    target_link_options(tests
      PRIVATE
        "--coverage"
      )
  endif()
endif()

# ---- Enable testing ----
enable_testing()
add_test(tests tests)
