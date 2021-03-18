cmake_minimum_required(VERSION 3.1.0)

project(serverbasler)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
        set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Gui Widgets SerialPort Network PrintSupport)
file(GLOB QT_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ui/*.ui)
file(GLOB QT_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} resources/*.qrc)
file(GLOB_RECURSE COMMON_SOURCE "*.cpp")

QT5_ADD_RESOURCES(QT_RESOURCES_CPP ${QT_RESOURCES})
QT5_WRAP_UI(QT_FORMS_HPP ${QT_FORMS})

include_directories(
        ${QT_USE_FILE}
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5PrintSupport_INCLUDE_DIRS}
        )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


include_directories(/opt/pylon/include/)

link_directories(/opt/pylon/lib/)

add_definitions( -lwiringPi  )	
find_library(wiringPi_LIB wiringPi)

add_executable(serverbasler
        main.cpp
        BaslerCam.h
        BaslerCam.cpp
        MyServer.h
        MyServer.cpp
        )

target_link_libraries(serverbasler
                      Qt5::Widgets Qt5::Core Qt5::SerialPort Qt5::Network
                      ${OpenCV_LIBS}
                      pylonutility
                      pylonbase
                      GCBase_gcc_v3_1_Basler_pylon
                      GenApi_gcc_v3_1_Basler_pylon
		      ${wiringPi_LIB}
		      -lcrypt 
		      -lrt
                      )
