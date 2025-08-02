#!/bin/bash

# VR Test Build Script
# Собирает и запускает тестовое VR приложение

set -e

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Функции для вывода
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Проверка зависимостей
check_dependencies() {
    print_info "Checking dependencies..."
    
    local missing_deps=()
    
    # Проверяем CMake
    if ! command -v cmake &> /dev/null; then
        missing_deps+=("cmake")
    fi
    
    # Проверяем make
    if ! command -v make &> /dev/null; then
        missing_deps+=("make")
    fi
    
    # Проверяем g++
    if ! command -v g++ &> /dev/null; then
        missing_deps+=("g++")
    fi
    
    # Проверяем SDL2
    if ! pkg-config --exists sdl2; then
        missing_deps+=("libsdl2-dev")
    fi
    
    # Проверяем OpenGL
    if ! pkg-config --exists gl; then
        missing_deps+=("libgl1-mesa-dev")
    fi
    
    # Проверяем GLEW
    if ! pkg-config --exists glew; then
        missing_deps+=("libglew-dev")
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing dependencies: ${missing_deps[*]}"
        print_info "Install them with: sudo apt install ${missing_deps[*]}"
        exit 1
    fi
    
    print_success "All dependencies found"
}

# Очистка
clean_build() {
    print_info "Cleaning build directory..."
    rm -rf build
    print_success "Build directory cleaned"
}

# Сборка проекта
build_project() {
    print_info "Building VR test project..."
    
    # Создаем папку для сборки
    mkdir -p build
    cd build
    
    # Конфигурируем CMake
    print_info "Configuring CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    # Собираем проект
    print_info "Compiling..."
    make -j$(nproc)
    
    cd ..
    print_success "VR test project built successfully"
}

# Запуск теста
run_test() {
    print_info "Running VR test..."
    
    if [ ! -f "build/vr_test" ]; then
        print_error "VR test executable not found. Build the project first."
        exit 1
    fi
    
    print_info "Starting VR test application..."
    print_info "Make sure SteamVR is running and PICO 4 is connected!"
    print_info "Press ESC to exit the test"
    
    # Запускаем тест
    ./build/vr_test
}

# Основная функция
main() {
    print_info "=== VR Test Build Script ==="
    
    # Парсинг аргументов
    local clean=false
    local run=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --clean)
                clean=true
                shift
                ;;
            --run)
                run=true
                shift
                ;;
            --help|-h)
                echo "Usage: $0 [OPTIONS]"
                echo "Options:"
                echo "  --clean    Clean build directory before building"
                echo "  --run      Run the VR test after building"
                echo "  --help     Show this help message"
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    # Проверяем зависимости
    check_dependencies
    
    # Очистка если нужно
    if [ "$clean" = true ]; then
        clean_build
    fi
    
    # Сборка проекта
    build_project
    
    # Запуск если нужно
    if [ "$run" = true ]; then
        run_test
    else
        print_info "Build completed. Run with: $0 --run"
    fi
}

# Запуск основной функции
main "$@" 