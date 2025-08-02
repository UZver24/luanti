cmake_minimum_required(VERSION 3.12)
project(vr_test)

# Настройки компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Поиск зависимостей
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

# Путь к OpenVR SDK
set(OPENVR_SDK_PATH "${CMAKE_SOURCE_DIR}/../../openvr")
if(NOT EXISTS ${OPENVR_SDK_PATH})
    message(FATAL_ERROR "OpenVR SDK not found at ${OPENVR_SDK_PATH}")
endif()

# Добавляем заголовочные файлы OpenVR
include_directories(${OPENVR_SDK_PATH}/headers)

# Находим библиотеку OpenVR
if(WIN32)
    set(OPENVR_LIB_PATH "${OPENVR_SDK_PATH}/lib/win64")
elseif(APPLE)
    set(OPENVR_LIB_PATH "${OPENVR_SDK_PATH}/lib/osx64")
else()
    set(OPENVR_LIB_PATH "${OPENVR_SDK_PATH}/lib/linux64")
endif()

find_library(OPENVR_LIBRARY
    NAMES openvr_api
    PATHS ${OPENVR_LIB_PATH}
    NO_DEFAULT_PATH
)

if(NOT OPENVR_LIBRARY)
    message(FATAL_ERROR "OpenVR library not found in ${OPENVR_LIB_PATH}")
endif()

# Создаем исполняемый файл
add_executable(vr_test main.cpp)

# Линкуем библиотеки
target_link_libraries(vr_test
    ${OPENGL_LIBRARIES}
    SDL2::SDL2
    SDL2::SDL2main
    GLEW::GLEW
    ${OPENVR_LIBRARY}
    GL
    GLU
)

# Настройки компиляции для Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(vr_test dl pthread)
endif()

# Настройки компиляции для Windows
if(WIN32)
    target_link_libraries(vr_test opengl32 glu32)
endif()

# Настройки компиляции для macOS
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(vr_test ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif()

# Флаги компиляции
target_compile_definitions(vr_test PRIVATE
    LINUX
    POSIX
)

message(STATUS "VR Test project configured successfully")
message(STATUS "OpenVR SDK path: ${OPENVR_SDK_PATH}")
message(STATUS "OpenVR library: ${OPENVR_LIBRARY}") 