# mandelbrot
add_executable(1-classical-raytracer-mandelbrot "mandelbrot.cpp")
set_target_properties(1-classical-raytracer-mandelbrot PROPERTIES OUTPUT_NAME "mandelbrot")
target_include_directories(1-classical-raytracer-mandelbrot PUBLIC "include/")
target_link_libraries(1-classical-raytracer-mandelbrot PUBLIC
    spdlog::spdlog
    glm
    stb_image_write
)

# normal
add_executable(1-classical-raytracer-normal "normal.cpp")
set_target_properties(1-classical-raytracer-normal PROPERTIES OUTPUT_NAME "normal")
target_include_directories(1-classical-raytracer-normal PUBLIC "include/")
target_link_libraries(1-classical-raytracer-normal PUBLIC
    spdlog::spdlog
    glm
    stb_image_write
)

# shadow
add_executable(1-classical-raytracer-shadow "shadow.cpp")
set_target_properties(1-classical-raytracer-shadow PROPERTIES OUTPUT_NAME "shadow")
target_include_directories(1-classical-raytracer-shadow PUBLIC "include/")
target_link_libraries(1-classical-raytracer-shadow PUBLIC
    spdlog::spdlog
    glm
    stb_image_write
    OpenMP::OpenMP_CXX
)