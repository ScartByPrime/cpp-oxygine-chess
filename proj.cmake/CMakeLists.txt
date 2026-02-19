cmake_minimum_required(VERSION 3.10)
project(Chess VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")

add_subdirectory(../oxygine-framework oxygine-build)

add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS}")

file(GLOB CHESS_SOURCES 
    "../src/*.cpp"
)

add_executable(Chess ${CHESS_SOURCES})

target_link_libraries(Chess ${OXYGINE_CORE_LIBS})

if (WIN32)
    set_target_properties(Chess PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if (EMSCRIPTEN)
    set_target_properties(Chess PROPERTIES SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s WASM=0 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -s TOTAL_MEMORY=50331648 -s FORCE_FILESYSTEM=1")

    target_link_libraries(Chess ${OXYGINE_JS_LIBRARIES})

    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data.js")
        em_link_pre_js(Chess "${CMAKE_CURRENT_SOURCE_DIR}/data.js")
    endif()
endif()
