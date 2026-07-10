cmake_minimum_required(VERSION 3.10.0)

project(task1 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

add_executable(task1 
    src/main.cpp 
    src/MainWindow.cpp 
    src/MainWindow.h 
    ui/MainWindow.ui
    src/orderbook.cpp
    src/orderbook.h
    src/users.cpp
    src/users.h
    src/ordertablemodel.cpp
    src/ordertablemodel.h
    src/resources.qrc
    src/consolereader.h
    )

add_compile_definitions(ORDERBOOK_THREAD_WAIT=5)


target_link_libraries(task1 PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Multimedia)

