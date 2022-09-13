# main
add_executable(main "main.cpp")
target_link_libraries(main PUBLIC
    spdlog::spdlog
    glm
    stb_image_write
    OpenMP::OpenMP_CXX
)

# reference
add_subdirectory("reference/")