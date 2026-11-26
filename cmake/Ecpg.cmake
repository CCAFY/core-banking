# ECPG(PostgreSQL 임베디드 SQL 전처리기)와 libpq 찾기.
#
# 제공하는 것
#   ECPG_EXECUTABLE, ECPG_INCLUDE_DIR, ECPG_LIBRARY
#   PQ_INCLUDE_DIR, PQ_LIBRARY            (테스트의 검증용 직접 조회에만 쓴다)
#   ecpg_precompile(<결과 변수> <a.pgc> <b.pgc> ...)   .pgc -> .c 변환 규칙 등록

find_program(PG_CONFIG_EXECUTABLE pg_config)
if(PG_CONFIG_EXECUTABLE)
  execute_process(COMMAND ${PG_CONFIG_EXECUTABLE} --bindir
    OUTPUT_VARIABLE _pg_bindir OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${PG_CONFIG_EXECUTABLE} --includedir
    OUTPUT_VARIABLE _pg_includedir OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${PG_CONFIG_EXECUTABLE} --libdir
    OUTPUT_VARIABLE _pg_libdir OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

find_program(ECPG_EXECUTABLE ecpg HINTS ${_pg_bindir} REQUIRED)
find_path(ECPG_INCLUDE_DIR ecpglib.h
  HINTS ${_pg_includedir} PATH_SUFFIXES postgresql REQUIRED)
find_library(ECPG_LIBRARY ecpg HINTS ${_pg_libdir} REQUIRED)

find_path(PQ_INCLUDE_DIR libpq-fe.h
  HINTS ${_pg_includedir} PATH_SUFFIXES postgresql REQUIRED)
find_library(PQ_LIBRARY pq HINTS ${_pg_libdir} REQUIRED)

function(ecpg_precompile out_var)
  set(_generated "")
  foreach(_pgc IN LISTS ARGN)
    get_filename_component(_name ${_pgc} NAME_WE)
    set(_src ${CMAKE_CURRENT_SOURCE_DIR}/${_pgc})
    set(_out ${CMAKE_CURRENT_BINARY_DIR}/ecpg/${_name}.c)
    add_custom_command(
      OUTPUT ${_out}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/ecpg
      COMMAND ${ECPG_EXECUTABLE} -o ${_out} ${_src}
      DEPENDS ${_src}
      COMMENT "ecpg ${_pgc}"
      VERBATIM
    )
    list(APPEND _generated ${_out})
  endforeach()
  set(${out_var} ${_generated} PARENT_SCOPE)
endfunction()
