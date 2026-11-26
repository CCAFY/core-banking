cmake_minimum_required(VERSION 3.20)
project(ledger LANGUAGES C CXX)

# 원장 코드는 C11, 테스트 코드만 C++17(GoogleTest)이다.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

option(LEDGER_SANITIZE "ASan + UBSan을 켜고 빌드한다" ON)
option(LEDGER_BUILD_TESTS "테스트를 빌드한다" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Ecpg)

# 모든 타깃에 공통으로 거는 경고와 sanitizer 옵션
add_library(ledger_options INTERFACE)
target_compile_options(ledger_options INTERFACE -Wall -Wextra -Werror)
if(LEDGER_SANITIZE)
  target_compile_options(ledger_options INTERFACE
    -fsanitize=address,undefined -fno-omit-frame-pointer)
  target_link_options(ledger_options INTERFACE -fsanitize=address,undefined)
endif()

# SQL이 들어가는 소스(.pgc)는 여기에 추가한다.
ecpg_precompile(LEDGER_ECPG_SOURCES
  src/db.pgc
)

# SQL이 없는 일반 C 소스(.c)는 여기에 추가한다.
add_library(ledger STATIC
  ${LEDGER_ECPG_SOURCES}
)
target_include_directories(ledger
  PUBLIC include
  PRIVATE src ${ECPG_INCLUDE_DIR}
)
target_compile_definitions(ledger PRIVATE _POSIX_C_SOURCE=200809L)
target_link_libraries(ledger PUBLIC ${ECPG_LIBRARY} PRIVATE ledger_options)

add_executable(ledger_cli cli/main.c)
set_target_properties(ledger_cli PROPERTIES OUTPUT_NAME ledger)
target_link_libraries(ledger_cli PRIVATE ledger ledger_options)

if(LEDGER_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
