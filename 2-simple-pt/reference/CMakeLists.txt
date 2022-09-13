# main
add_executable(2-simple-pt-ref "main.cpp")
set_target_properties(2-simple-pt-ref PROPERTIES OUTPUT_NAME "main")
target_include_directories(2-simple-pt-ref PUBLIC "include/")
target_link_libraries(2-simple-pt-ref PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
)