# pt
add_executable(3-pt-pt "pt.cpp")
set_target_properties(3-pt-pt PROPERTIES OUTPUT_NAME "pt")
target_include_directories(3-pt-pt PUBLIC "include/")
target_link_libraries(3-pt-pt PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
    tinyobjloader
)