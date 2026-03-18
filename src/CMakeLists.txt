cmake_minimum_required(VERSION 3.16)

# 设置可执行文件名称
set(BIN_NAME DPostman)

# 查找Qt6库，并指定需要的组件为Core
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
find_package(Dtk6Core REQUIRED)

# 设置Qt6标准项目,该命令包含在Qt6的Core组件中
qt_standard_project_setup()

# 包含源文件
file(GLOB_RECURSE SRCS
    "*.h"
    "*.cpp"
)

# 添加可执行文件
qt_add_executable(${BIN_NAME} 
    ${SRCS}
)

# 指定链接库
target_link_libraries(${BIN_NAME} PRIVATE 
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Dtk6::Core
)

# 安装二进制
install(TARGETS ${BIN_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
