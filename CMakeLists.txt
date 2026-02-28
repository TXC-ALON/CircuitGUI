cmake_minimum_required(VERSION 3.16)
project(CircuitGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 QUIET COMPONENTS Core Gui Widgets)
if(Qt6_FOUND)
  set(QT_PACKAGE Qt6)
else()
  find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
  set(QT_PACKAGE Qt5)
endif()

find_package(Boost REQUIRED)

add_executable(circuitgui
  src/main.cpp
  src/items/instanceitem.cpp
  src/items/portitem.cpp
  src/items/netitem.cpp
  src/io/jsonloader.cpp
  src/graph/graphlayout.cpp
)

target_include_directories(circuitgui PRIVATE src)

target_link_libraries(circuitgui PRIVATE
  ${QT_PACKAGE}::Core
  ${QT_PACKAGE}::Gui
  ${QT_PACKAGE}::Widgets
  Boost::boost
)
