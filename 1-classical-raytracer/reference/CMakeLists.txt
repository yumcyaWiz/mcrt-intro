# normal
add_executable(normal "normal.cpp")
target_include_directories(normal PUBLIC "include/")
target_link_libraries(normal PUBLIC
    spdlog::spdlog
    glm
    stb_image_write
)

# shadow
add_executable(shadow "shadow.cpp")
target_include_directories(shadow PUBLIC "include/")
target_link_libraries(shadow PUBLIC
    spdlog::spdlog
    glm
    stb_image_write
    OpenMP::OpenMP_CXX
)