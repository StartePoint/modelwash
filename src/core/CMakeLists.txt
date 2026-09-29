add_library(modelwash_core STATIC
  document.cpp
  command.cpp
  wash.cpp
)
target_include_directories(modelwash_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(modelwash_core
  PUBLIC
    meshoptimizer
)
if(MSVC)
  target_compile_options(modelwash_core PRIVATE /utf-8)
endif()
target_compile_definitions(modelwash_core PUBLIC MESHOPTIMIZER_ALLOC_EXPORT=)
