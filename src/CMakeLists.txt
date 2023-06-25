set(SOURCES hello_window.cpp hello_triangle.cpp container.cpp lighting.cpp)

foreach(source ${SOURCES})
  get_filename_component(name ${source} NAME_WE)

  add_executable(${name} ${source})

  target_link_libraries(
    ${name}
    PUBLIC glfw
    PUBLIC glad
    PUBLIC stb_image
    PRIVATE glm
  )
endforeach()
