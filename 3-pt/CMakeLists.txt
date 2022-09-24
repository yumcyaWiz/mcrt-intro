# main
add_executable(3-pt-main "main.cpp")
set_target_properties(3-pt-main PROPERTIES OUTPUT_NAME "main")
target_include_directories(3-pt-main PUBLIC "include/")
target_link_libraries(3-pt-main PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
    tinyobjloader
)

# reference
add_subdirectory("reference/")
