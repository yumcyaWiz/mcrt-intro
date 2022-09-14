# ao
add_executable(2-simple-pt-ao "ao.cpp")
set_target_properties(2-simple-pt-ao PROPERTIES OUTPUT_NAME "ao")
target_include_directories(2-simple-pt-ao PUBLIC "include/")
target_link_libraries(2-simple-pt-ao PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
)

# pt
add_executable(2-simple-pt-pt "pt.cpp")
set_target_properties(2-simple-pt-pt PROPERTIES OUTPUT_NAME "pt")
target_include_directories(2-simple-pt-pt PUBLIC "include/")
target_link_libraries(2-simple-pt-pt PUBLIC
    spdlog::spdlog
    glm
    stb_image
    stb_image_write
    OpenMP::OpenMP_CXX
)