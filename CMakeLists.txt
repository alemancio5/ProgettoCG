cmake_minimum_required(VERSION 3.24)

project(ProgettoCG)

set($ENV{VULKAN_SDK} "/Library/CLion/VulkanSDK/1.3.275.0/macOS")
set(GLFW "/Library/CLion/VulkanSDK/libs/glfw-3.4.bin.MACOS")
set(GLM "/Library/CLion/VulkanSDK/libs/glm")
set(MAIN ProgettoCG.cpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")

list(APPEND INCLUDE_DIRS "${GLFW}/include" ${GLM} headers)
list(APPEND LINK_LIBS "${GLFW}/lib-arm64/libglfw3.a")

add_executable(ProgettoCG ProgettoCG.cpp)

find_package(Vulkan REQUIRED)
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

include_directories(${SFML_INCLUDE_DIRS})

target_link_libraries(ProgettoCG sfml-system sfml-window sfml-graphics sfml-audio sfml-network)

foreach(dir IN LISTS Vulkan_INCLUDE_DIR INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PUBLIC ${dir})
endforeach()

foreach(lib IN LISTS Vulkan_LIBRARIES LINK_LIBS)
    target_link_libraries(${PROJECT_NAME} ${lib})
endforeach()

file(COPY ${CMAKE_CURRENT_LIST_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_LIST_DIR}/modules DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_LIST_DIR}/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

