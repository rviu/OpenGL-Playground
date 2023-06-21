add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC external/glfw/include)

target_link_libraries(${PROJECT_NAME} glfw)
