qt_add_executable(ModelWash
  main.cpp
  main_window.cpp
  model_viewport.cpp
  include/modelwash/main_window.hpp
  include/modelwash/model_viewport.hpp
  icons.qrc
  ${CMAKE_SOURCE_DIR}/resources/icons/app.rc
)

set_target_properties(ModelWash PROPERTIES
  WIN32_EXECUTABLE TRUE
)

target_include_directories(ModelWash
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ModelWash
  PRIVATE
    modelwash_presentation
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Concurrent
)

if(WIN32)
  target_link_libraries(ModelWash PRIVATE opengl32)
endif()

if(MSVC)
  target_compile_options(ModelWash PRIVATE /utf-8)
endif()
