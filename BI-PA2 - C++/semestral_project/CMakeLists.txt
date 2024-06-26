cmake_minimum_required(VERSION 3.25)
project(semestral_project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -pedantic -Wextra")

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
add_executable(semestral_project src/main.cpp src/notes/CToDoList.h
        src/notes/CToDoList.cpp src/notes/CTextNote.h
        src/notes/CTextNote.cpp src/notes/CShopList.h
        src/notes/CShopList.cpp src/notes/CNote.cpp
        src/additional_for_notes/CTag.cpp src/notes/CFileManager.cpp
        src/notes/CFileManager.h src/notes/CFolder.h
        src/notes/CFolder.cpp
        src/menus/CMenu.h src/menus/CMenu.cpp
        src/menus/CLeftMenuBar.h src/menus/CLeftMenuBar.cpp
        src/CApplication.cpp src/CApplication.h src/menus/CLeftFolderMenu.cpp
        src/menus/CLeftFolderMenu.h src/menus/CLeftFileMenu.cpp
        src/menus/CLeftFileMenu.h src/menus/CLeftEditMenu.cpp
        src/menus/CLeftEditMenu.h src/additional_for_notes/CDate.cpp)
target_link_libraries(semestral_project ${CURSES_LIBRARIES})