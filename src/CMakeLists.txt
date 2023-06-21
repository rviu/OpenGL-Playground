set(APP_NAME hello_triangle)

add_executable(${APP_NAME} ${APP_NAME}.cpp)

target_link_libraries(
  ${APP_NAME}
  glfw
  glad
)
