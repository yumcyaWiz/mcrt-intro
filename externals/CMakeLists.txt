# tinyobjloader
add_library(tinyobjloader
    "tinyobjloader.cpp"
)
target_include_directories(tinyobjloader PUBLIC "tinyobjloader/")

# glm
add_subdirectory("glm/")

# spdlog
SET(SPDLOG_MASTER_PROJECT ON CACHE BOOL "" FORCE)
add_subdirectory(spdlog)

# stb_image
add_library(stb_image
    "stb_image.cpp"
)
target_include_directories(stb_image PUBLIC "stb/")

# stb_image_write
add_library(stb_image_write
    "stb_image_write.cpp"
)
target_include_directories(stb_image_write PUBLIC "stb/")

# argparse
add_library(argparse INTERFACE)
target_include_directories(argparse INTERFACE "argparse/include")

# indicators
add_library(indicators INTERFACE)
target_include_directories(indicators INTERFACE "indicators/include")