add_library(modelwash_io STATIC
  assimp_import.cpp
  glb_export.cpp
)
target_include_directories(modelwash_io
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(modelwash_io
  PUBLIC
    modelwash_core
    assimp
    Qt6::Gui
)
if(MSVC)
  target_compile_options(modelwash_io PRIVATE /utf-8)
endif()
