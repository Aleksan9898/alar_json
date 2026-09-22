cmake_minimum_required(VERSION 3.20)
project(alar_json VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(alar_json
    src/json.cpp
    src/json_value.cpp
    src/json_error.cpp
    src/lexer/json_lexer.cpp
    src/parser/json_parser.cpp
    src/parser/json_parser_error.cpp
)

# Стандарт C++20 без компиляторных расширений (strict ISO C++)
target_compile_features(alar_json PUBLIC cxx_std_20)
set_target_properties(alar_json PROPERTIES CXX_EXTENSIONS OFF)

# Пути к заголовкам: include — наружу, src — только для внутренней сборки
target_include_directories(alar_json
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser
)

# === Модуль тестирования (CTest + GoogleTest) ===
include(CTest)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    # Корректная обработка MSVC CRT при сборке на Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Исполняемый файл тестов
    add_executable(alar_json_tests
        tests/test_json_value.cpp
        tests/test_json_lexer.cpp
        tests/test_json_parser.cpp
        tests/test_json_api.cpp
    )

    # Прямой доступ тестов к приватным заголовкам в src/ (lexer.hpp, parser.hpp)
    target_include_directories(alar_json_tests
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    # Линковка с основной библиотекой и готовым main() от GTest
    target_link_libraries(alar_json_tests
        PRIVATE
        alar_json
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(alar_json_tests)
endif()

option(BUILD_FUZZING "Build libFuzzer target" OFF)

if(BUILD_FUZZING)
    target_compile_options(alar_json
        PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )

    target_link_options(alar_json
        PRIVATE
        -fsanitize=address,undefined
    )

    add_executable(alar_json_fuzzer
        fuzz/fuzz_json_parser.cpp
    )

    target_link_libraries(alar_json_fuzzer
        PRIVATE
        alar_json
    )

    target_compile_options(alar_json_fuzzer
        PRIVATE
        -fsanitize=fuzzer,address,undefined
        -fno-omit-frame-pointer
    )

    target_link_options(alar_json_fuzzer
        PRIVATE
        -fsanitize=fuzzer,address,undefined
    )
endif()
