add_library(modelwash_presentation STATIC
  document_session.cpp
  include/modelwash/document_session.hpp
)
target_include_directories(modelwash_presentation
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(modelwash_presentation
  PUBLIC
    modelwash_io
    Qt6::Core
    Qt6::Gui
)
if(MSVC)
  target_compile_options(modelwash_presentation PRIVATE /utf-8)
endif()
