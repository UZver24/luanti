#!/bin/bash

# Скрипт запуска для программы Luanti
# Автор: AI Assistant
# Дата: $(date)

set -e  # Остановка при ошибке

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

# Пути к исполняемым файлам
LUANTI_BIN="./bin/luanti"
BUILD_LUANTI_BIN="./build/bin/luanti"

# Проверка существования исполняемого файла
check_executable() {
    local executable_path=""
    
    # Проверяем в порядке приоритета
    if [ -f "$LUANTI_BIN" ]; then
        executable_path="$LUANTI_BIN"
    elif [ -f "$BUILD_LUANTI_BIN" ]; then
        executable_path="$BUILD_LUANTI_BIN"
    else
        print_error "Исполняемый файл luanti не найден!"
        print_info "Возможные пути:"
        print_info "  - $LUANTI_BIN"
        print_info "  - $BUILD_LUANTI_BIN"
        print_info ""
        print_info "Убедитесь, что проект собран. Запустите: ./build.sh"
        exit 1
    fi
    
    echo "$executable_path"
}

# Проверка зависимостей для запуска
check_runtime_dependencies() {
    print_info "Проверка зависимостей для запуска..."
    
    local missing_deps=()
    
    # Проверяем основные библиотеки
    if ! ldd "$1" >/dev/null 2>&1; then
        print_error "Не удалось проверить зависимости исполняемого файла"
        return 1
    fi
    
    # Проверяем наличие критических библиотек
    local critical_libs=("libSDL2" "libOpenGL" "libGLU" "libfreetype")
    
    for lib in "${critical_libs[@]}"; do
        if ! ldd "$1" | grep -q "$lib"; then
            missing_deps+=("$lib")
        fi
    done
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_warning "Возможно отсутствуют библиотеки: ${missing_deps[*]}"
        print_info "Установите их с помощью вашего пакетного менеджера"
    else
        print_success "Все зависимости найдены"
    fi
}

# Создание директории для пользовательских данных
create_user_dirs() {
    print_info "Проверка пользовательских директорий..."
    
    local user_dirs=(
        "worlds"
        "mods"
        "textures"
        "screenshots"
        "logs"
    )
    
    for dir in "${user_dirs[@]}"; do
        if [ ! -d "$dir" ]; then
            mkdir -p "$dir"
            print_info "Создана директория: $dir"
        fi
    done
    
    print_success "Пользовательские директории готовы"
}

# Запуск с профилированием
run_with_profiling() {
    local executable="$1"
    local args="$2"
    
    print_info "Запуск с профилированием..."
    
    # Проверяем наличие perf
    if command -v perf >/dev/null 2>&1; then
        print_info "Используется perf для профилирования"
        perf record -g "$executable" $args
    elif command -v valgrind >/dev/null 2>&1; then
        print_info "Используется valgrind для профилирования"
        valgrind --tool=callgrind "$executable" $args
    else
        print_warning "Инструменты профилирования не найдены, запуск без профилирования"
        "$executable" $args
    fi
}

# Запуск с отладкой
run_with_debug() {
    local executable="$1"
    local args="$2"
    
    print_info "Запуск в режиме отладки..."
    
    # Устанавливаем переменные окружения для отладки
    export MINETEST_DEBUG=1
    export MINETEST_LOG_LEVEL=3
    
    # Запускаем с gdb если доступен
    if command -v gdb >/dev/null 2>&1; then
        print_info "Запуск с gdb..."
        gdb --args "$executable" $args
    else
        print_warning "gdb не найден, запуск без отладчика"
        "$executable" $args
    fi
}

# Основная функция
main() {
    print_info "=== Скрипт запуска Luanti ==="
    
    # Парсинг аргументов
    local debug_mode=false
    local profiling_mode=false
    local world_path=""
    local server_mode=false
    local port=""
    local address=""
    local config_file=""
    local log_file=""
    
    # Собираем аргументы для передачи в luanti
    local luanti_args=()
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --debug)
                debug_mode=true
                shift
                ;;
            --profile)
                profiling_mode=true
                shift
                ;;
            --world|-w)
                world_path="$2"
                luanti_args+=("--world" "$2")
                shift 2
                ;;
            --server|-s)
                server_mode=true
                luanti_args+=("--server")
                shift
                ;;
            --port|-p)
                port="$2"
                luanti_args+=("--port" "$2")
                shift 2
                ;;
            --address|-a)
                address="$2"
                luanti_args+=("--address" "$2")
                shift 2
                ;;
            --config|-c)
                config_file="$2"
                luanti_args+=("--config" "$2")
                shift 2
                ;;
            --log|-l)
                log_file="$2"
                luanti_args+=("--logfile" "$2")
                shift 2
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            --)
                shift
                luanti_args+=("$@")
                break
                ;;
            -*)
                # Передаем неизвестные флаги в luanti
                luanti_args+=("$1")
                shift
                ;;
            *)
                # Все остальное передаем в luanti
                luanti_args+=("$1")
                shift
                ;;
        esac
    done
    
    # Проверяем исполняемый файл
    local executable=$(check_executable)
    print_success "Найден исполняемый файл: $executable"
    
    # Проверяем зависимости
    check_runtime_dependencies "$executable"
    
    # Создаем пользовательские директории
    create_user_dirs
    
    # Настройка логирования
    if [ -z "$log_file" ]; then
        log_file="logs/luanti_$(date +%Y%m%d_%H%M%S).log"
        luanti_args+=("--logfile" "$log_file")
    fi
    
    print_info "Лог файл: $log_file"
    
    # Запуск в зависимости от режима
    if [ "$debug_mode" = true ]; then
        run_with_debug "$executable" "${luanti_args[*]}"
    elif [ "$profiling_mode" = true ]; then
        run_with_profiling "$executable" "${luanti_args[*]}"
    else
        print_info "Запуск Luanti..."
        print_info "Аргументы: ${luanti_args[*]}"
        print_info "Для выхода нажмите Ctrl+C"
        echo ""
        
        # Запускаем программу
        "$executable" "${luanti_args[@]}"
    fi
    
    print_success "Luanti завершил работу"
}

# Показать справку
show_help() {
    echo "Использование: $0 [опции] [аргументы luanti]"
    echo ""
    echo "Опции скрипта:"
    echo "  --debug              Запуск в режиме отладки с gdb"
    echo "  --profile            Запуск с профилированием (perf/valgrind)"
    echo "  --world, -w PATH     Путь к миру"
    echo "  --server, -s         Запуск в режиме сервера"
    echo "  --port, -p PORT      Порт для сервера"
    echo "  --address, -a ADDR   Адрес для привязки сервера"
    echo "  --config, -c FILE    Файл конфигурации"
    echo "  --log, -l FILE       Файл лога"
    echo "  --help, -h           Показать эту справку"
    echo ""
    echo "Примеры:"
    echo "  $0                    # Обычный запуск"
    echo "  $0 --world myworld    # Запуск с миром"
    echo "  $0 --server --port 30000  # Запуск сервера"
    echo "  $0 --debug            # Запуск с отладчиком"
    echo "  $0 --profile          # Запуск с профилированием"
    echo ""
    echo "Все остальные аргументы передаются в luanti"
    echo "Для получения справки luanti: $0 -- --help"
}

# Запуск основной функции
main "$@" 