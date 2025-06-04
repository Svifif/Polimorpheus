cmake_minimum_required(VERSION 3.15)
project(GLFWDemo)

# Жёстко прописываем пути к vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/Users/artem/vcpkg/scripts/buildsystems/vcpkg.cmake" FORCE)
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "" FORCE)

# Явно указываем путь к конфигурации GLFW
set(glfw3_DIR "C:/Users/artem/vcpkg/installed/x64-windows/share/glfw3" CACHE PATH "" FORCE)

# Ищем GLFW
find_package(glfw3 CONFIG REQUIRED)

# Ищем OpenGL
find_package(OpenGL REQUIRED)

# Создаём исполняемый файл
add_executable(graf graf.cpp)

# Линкуем библиотеки
target_link_libraries(graf PRIVATE 
    glfw
    OpenGL::GL
)