cmake_minimum_required(VERSION 3.5)

project(BankWireTransferSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(BankWireTransferSimulator
    main.cpp
    mainwindow.cpp
    bank.cpp
    mainwindow.ui
)

target_link_libraries(BankWireTransferSimulator Qt5::Widgets)
