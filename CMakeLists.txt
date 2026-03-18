# 指定cmake最低版本
cmake_minimum_required(VERSION 3.16)

# 指定项目名称
project(DPostman LANGUAGES CXX)

# 指定c++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Standard install paths
include(GNUInstallDirs)

# 添加子目录src
add_subdirectory("src")
