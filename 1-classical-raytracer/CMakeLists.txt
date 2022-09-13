# main
add_executable(1-classical-raytracer-main "main.cpp")
set_target_properties(1-classical-raytracer-main PROPERTIES OUTPUT_NAME "main")
target_include_directories(1-classical-raytracer-main PUBLIC "include/")
target_link_libraries(1-classical-raytracer-main PUBLIC
    spdlog::spdlog
    glm
    stb_image_write
    OpenMP::OpenMP_CXX
)

# reference
add_subdirectory("reference/")