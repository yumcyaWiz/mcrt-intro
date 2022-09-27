# bvh
add_executable(4-bvh-bvh "bvh.cpp")
set_target_properties(4-bvh-bvh PROPERTIES OUTPUT_NAME "bvh")
target_include_directories(4-bvh-bvh PUBLIC "include/")
target_link_libraries(4-bvh-bvh PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
    tinyobjloader
)

# visualize
add_executable(4-bvh-visualize "visualize.cpp")
set_target_properties(4-bvh-visualize PROPERTIES OUTPUT_NAME "visualize")
target_include_directories(4-bvh-visualize PUBLIC "include/")
target_link_libraries(4-bvh-visualize PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
    tinyobjloader
)