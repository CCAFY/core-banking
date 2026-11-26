# 시스템에 GoogleTest가 있으면 그걸 쓰고, 없으면 내려받는다.
find_package(GTest QUIET)
if(NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
  )
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

# 새 테스트 파일은 여기에 추가한다.
add_executable(ledger_tests
  support/db_fixture.cpp
  test_smoke.cpp
)
target_include_directories(ledger_tests PRIVATE ${PQ_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(ledger_tests PRIVATE
  LEDGER_SCHEMA_FILE="${CMAKE_SOURCE_DIR}/db/schema.sql")
target_link_libraries(ledger_tests PRIVATE
  ledger ledger_options GTest::gtest GTest::gtest_main ${PQ_LIBRARY})

# 테스트들이 같은 DB를 쓰므로 실행 파일 하나를 통째로 순서대로 돌린다.
add_test(NAME ledger_tests COMMAND ledger_tests)
