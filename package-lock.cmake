# CPM Package Lock
# This file should be committed to version control

CPMDeclarePackage(CPMPackageProject
  NAME PackageProject.cmake
  GITHUB_REPOSITORY TheLartians/PackageProject.cmake
  VERSION 1.11.2
)

CPMDeclarePackage(CPMLicenses
  NAME CPMLicenses.cmake 
  GITHUB_REPOSITORY cpm-cmake/CPMLicenses.cmake
  VERSION 0.0.7
)

CPMDeclarePackage(Ccache
  NAME Ccache.cmake
  GITHUB_REPOSITORY TheLartians/Ccache.cmake
  VERSION 1.2.5
)

# nlohmann_json
CPMDeclarePackage(nlohmann_json
  NAME nlohmann_json
  VERSION 3.11.3
  GITHUB_REPOSITORY nlohmann/json
  OPTIONS
    "JSON_Install ON"
    "JSON_BuildTests OFF"
    "JSON_MultipleHeaders OFF"
    "JSON_ImplicitConversions OFF"
)

# fmt
CPMDeclarePackage(fmt
  NAME fmt
  GIT_TAG 11.0.2
  GITHUB_REPOSITORY fmtlib/fmt
  OPTIONS
    "FMT_INSTALL ON"
  EXCLUDE_FROM_ALL True
)

# Boost
CPMDeclarePackage(Boost
  NAME Boost
  VERSION 1.85.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.85.0/boost-1.85.0-cmake.tar.xz 
  OPTIONS
    "BOOST_ENABLE_CMAKE ON"
    "BOOST_SKIP_INSTALL_RULES OFF"
    "CMAKE_SKIP_INSTALL_RULES OFF"
    "BUILD_SHARED_LIBS OFF"
  EXCLUDE_FROM_ALL True
)

# googletest
CPMDeclarePackage(googletest
  NAME googletest
  VERSION 1.12.1
  GIT_TAG release-1.12.1
  GITHUB_REPOSITORY google/googletest
  OPTIONS
    "INSTALL_GTEST OFF"
    "gtest_force_shared_crt"
    "ECLUDE_FROM_ALL True"
)
