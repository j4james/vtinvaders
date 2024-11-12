cmake_minimum_required(VERSION 3.15)
project(vtinvaders)

set(
    MAIN_FILES
    "src/main.cpp"
    "src/aliens.cpp"
    "src/capabilities.cpp"
    "src/coloring.cpp"
    "src/engine.cpp"
    "src/font.cpp"
    "src/missiles.cpp"
    "src/options.cpp"
    "src/os.cpp"
    "src/screen.cpp"
    "src/shields.cpp"
    "src/turret.cpp"
    "src/status.cpp"
    "src/ufo.cpp"
)

set(
    DOC_FILES
    "README.md"
    "LICENSE.txt"
)

if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

add_executable(vtinvaders ${MAIN_FILES})

if(UNIX)
    target_link_libraries(vtinvaders -lpthread)
endif()

set_target_properties(vtinvaders PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED On)
source_group("Doc Files" FILES ${DOC_FILES})
