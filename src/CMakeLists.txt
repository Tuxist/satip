INCLUDE_DIRECTORIES(
   ${CMAKE_BINARY_DIR}
  "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)

if(${CMAKE_BUILD_TYPE})
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DDEBUG=true)
  endif()
endif()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)

add_subdirectory(lib)
add_subdirectory(cmd)
add_subdirectory(qt)