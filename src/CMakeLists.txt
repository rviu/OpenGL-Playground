set(SOURCES hello_window.cpp hello_triangle.cpp container.cpp)

foreach(source ${SOURCES})
  get_filename_component(name ${source} NAME_WE)

  add_executable(${name} ${source})

  target_link_libraries(
    ${name}
    glfw
    glad
    stb_image
  )
endforeach()
