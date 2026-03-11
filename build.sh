#!/bin/bash

# Скрипт сборки для проекта Luanti
# Автор: AI Assistant
# Дата: $(date)

set -e  # Остановка при ошибке

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

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
    print_info "Проверка зависимостей..."
    
    local missing_deps=()
    
    # Проверяем основные инструменты
    command -v cmake >/dev/null 2>&1 || missing_deps+=("cmake")
    command -v make >/dev/null 2>&1 || missing_deps+=("make")
    command -v g++ >/dev/null 2>&1 || missing_deps+=("g++")
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Отсутствуют зависимости: ${missing_deps[*]}"
        print_info "Установите их с помощью вашего пакетного менеджера"
        exit 1
    fi
    
    print_success "Все зависимости найдены"
}

# Очистка предыдущей сборки
clean_build() {
    print_info "Очистка предыдущей сборки..."
    rm -rf "$SCRIPT_DIR/build/"
    print_success "Очистка завершена"
}

# Создание директории сборки
create_build_dir() {
    print_info "Создание директории сборки..."
    mkdir -p "$SCRIPT_DIR/build"
    cd "$SCRIPT_DIR/build"
}

# Конфигурация CMake
configure_cmake() {
    local build_type=${1:-Release}
    local run_in_place=${2:-TRUE}
    local build_client=${3:-TRUE}
    local build_server=${4:-FALSE}
    local build_vr=${5:-FALSE}
    local openvr_path=${6:-$SCRIPT_DIR/../openvr}
    
    print_info "Конфигурация CMake..."
    print_info "Тип сборки: $build_type"
    print_info "Запуск из исходников: $run_in_place"
    print_info "Сборка клиента: $build_client"
    print_info "Сборка сервера: $build_server"
    print_info "VR поддержка: $build_vr"
    
    if [ "$build_vr" = "TRUE" ]; then
        print_info "Путь к OpenVR SDK: $openvr_path"
    fi
    
    cmake "$SCRIPT_DIR" \
        -DCMAKE_BUILD_TYPE=$build_type \
        -DRUN_IN_PLACE=$run_in_place \
        -DBUILD_CLIENT=$build_client \
        -DBUILD_SERVER=$build_server \
        -DBUILD_WITH_VR=$build_vr \
        -DOPENVR_SDK_PATH=$openvr_path \
        -DBUILD_UNITTESTS=TRUE \
        -DBUILD_DOCUMENTATION=TRUE \
        -B . \
        -S "$SCRIPT_DIR"
    
    print_success "Конфигурация завершена"
}

# Сборка проекта
build_project() {
    local jobs=${1:-$(nproc)}
    
    print_info "Начало сборки с $jobs параллельными процессами..."
    make -j$jobs
    
    print_success "Сборка завершена успешно!"
}

# Запуск тестов
run_tests() {
    print_info "Запуск unit-тестов..."
    if [ -f "$SCRIPT_DIR/bin/luanti" ]; then
        "$SCRIPT_DIR/bin/luanti" --run-unittests
        print_success "Тесты завершены"
    else
        print_warning "Исполняемый файл не найден, тесты пропущены"
    fi
}

# Установка (опционально)
install_project() {
    if [ "$1" = "install" ]; then
        print_info "Установка проекта..."
        sudo make install
        print_success "Установка завершена"
    fi
}

# Основная функция
main() {
    print_info "=== Скрипт сборки Luanti ==="
    
    # Парсинг аргументов
    local build_type="Release"
    local run_in_place="TRUE"
    local build_client="TRUE"
    local build_server="FALSE"
    local build_vr="TRUE"
    local openvr_path="$SCRIPT_DIR/../openvr"
    local clean=false
    local install=false
    local jobs=$(nproc)
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --debug)
                build_type="Debug"
                shift
                ;;
            --release)
                build_type="Release"
                shift
                ;;
            --clean)
                clean=true
                shift
                ;;
            --install)
                install=true
                shift
                ;;
            --server-only)
                build_client="FALSE"
                build_server="TRUE"
                shift
                ;;
            --client-only)
                build_client="TRUE"
                build_server="FALSE"
                shift
                ;;
            --system-install)
                run_in_place="FALSE"
                shift
                ;;
            --vr)
                build_vr="TRUE"
                shift
                ;;
            --no-vr)
                build_vr="FALSE"
                shift
                ;;
            --openvr-path)
                openvr_path="$2"
                shift 2
                ;;
            --jobs|-j)
                jobs="$2"
                shift 2
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                print_error "Неизвестный аргумент: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Проверка зависимостей
    check_dependencies
    
    # Очистка при необходимости
    if [ "$clean" = true ]; then
        clean_build
    fi
    
    # Создание директории сборки
    create_build_dir
    
    # Конфигурация и сборка
    configure_cmake "$build_type" "$run_in_place" "$build_client" "$build_server" "$build_vr" "$openvr_path"
    build_project "$jobs"
    
    # Возврат в корневую директорию
    cd "$SCRIPT_DIR"
    
    # Запуск тестов
    run_tests
    
    # Установка при необходимости
    if [ "$install" = true ]; then
        install_project "install"
    fi
    
    print_success "=== Сборка завершена успешно! ==="
    print_info "Исполняемый файл: $SCRIPT_DIR/bin/luanti"
}

# Показать справку
show_help() {
    echo "Использование: $0 [опции]"
    echo ""
    echo "Опции:"
    echo "  --debug              Сборка в режиме отладки"
    echo "  --release            Сборка в режиме релиза (по умолчанию)"
    echo "  --clean              Очистить предыдущую сборку"
    echo "  --install            Установить после сборки"
    echo "  --server-only        Собрать только сервер"
    echo "  --client-only        Собрать только клиент (по умолчанию)"
    echo "  --system-install     Сборка для системной установки"
    echo "  --vr                 Включить поддержку VR (OpenVR) (по умолчанию)"
    echo "  --no-vr              Отключить поддержку VR"
    echo "  --openvr-path PATH   Путь к OpenVR SDK (по умолчанию: $(pwd)/../openvr)"
    echo "  --jobs, -j N         Количество параллельных процессов (по умолчанию: все доступные)"
    echo "  --help, -h           Показать эту справку"
    echo ""
    echo "Примеры:"
    echo "  $0                    # Сборка с поддержкой VR (по умолчанию)"
    echo "  $0 --debug            # Сборка в режиме отладки с VR"
    echo "  $0 --clean --install  # Очистить, собрать и установить"
    echo "  $0 --server-only      # Собрать только сервер"
    echo "  $0 --no-vr            # Сборка без поддержки VR"
    echo "  $0 --openvr-path /path/to/openvr  # Сборка с VR и указанным путем к SDK"
}

# Запуск основной функции
main "$@" 
