# main
add_executable(2-simple-pt-main "main.cpp")
set_target_properties(2-simple-pt-main PROPERTIES OUTPUT_NAME "main")
target_include_directories(2-simple-pt-main PUBLIC "include/")
target_link_libraries(2-simple-pt-main PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
)

# reference
add_subdirectory("reference/")