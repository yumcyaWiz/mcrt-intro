# main
add_executable(4-bvh-main "main.cpp")
set_target_properties(4-bvh-main PROPERTIES OUTPUT_NAME "main")
target_include_directories(4-bvh-main PUBLIC "include/")
target_link_libraries(4-bvh-main PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
    tinyobjloader
)

# reference
add_subdirectory("reference/")