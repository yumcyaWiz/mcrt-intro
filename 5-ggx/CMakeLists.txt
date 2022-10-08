# main
add_executable(5-ggx-main "main.cpp")
set_target_properties(5-ggx-main PROPERTIES OUTPUT_NAME "main")
target_include_directories(5-ggx-main PUBLIC "include/")
target_link_libraries(5-ggx-main PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
    tinyobjloader
)

# reference
add_subdirectory("reference/")