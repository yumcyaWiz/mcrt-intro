# main
add_executable(main "main.cpp")
target_link_libraries(main PUBLIC
    spdlog::spdlog
    glm
)

# reference
add_executable(reference "reference/main.cpp")
target_include_directories(reference PUBLIC "reference/")
target_link_libraries(reference PUBLIC
    spdlog::spdlog
    glm
    stb_image_write
    OpenMP::OpenMP_CXX
)