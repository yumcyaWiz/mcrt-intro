# ggx
add_executable(5-ggx-ggx "ggx.cpp")
set_target_properties(5-ggx-ggx PROPERTIES OUTPUT_NAME "ggx")
target_include_directories(5-ggx-ggx PUBLIC "include/")
target_link_libraries(5-ggx-ggx PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
    tinyobjloader
)