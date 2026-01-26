#!/bin/bash

# ══════════════════════════════════════════════════════════════════════════════ #
# | ArchUpd (Arch Update): A script for safely updating Arch Linux             | #
# | Installer: ArchUpd Hardening Installer v.2.5-portable_ru                   | # 
# | Version (archupd): v.2.2-2026-sys_ru                                       | #
# | Author: LeoIKT                                                             | #
# | License: MIT                                                               | #
# ══════════════════════════════════════════════════════════════════════════════ #

set -e
[[ $EUID -ne 0 ]] && exec sudo -E "$0" "$@"

# Версия
readonly SCRIPT_VERSION="v.2.5-portable_ru"   #  <----- Installer ver.
readonly ARCHUPD_VERSION="v.2.2-2026-sys_ru"  #  <----- ArchUpd ver.

# Цвета для вывода
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly CYAN='\033[0;36m'
readonly NC='\033[0m'
readonly BOLD='\033[1m'

# Константы путей
readonly BIN_DIR="/usr/local/bin"
readonly CONFIG_DIR="/etc/archupd"
readonly LOG_DIR="/var/log/archupd"
readonly SYSTEMD_DIR="/etc/systemd/system"

# ============================================
# ФУНКЦИИ УТИЛИТЫ
# ============================================

print_header() {
    echo -e "${CYAN}══════════════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}${CYAN}|          ArchUpd Installer ${SCRIPT_VERSION}               |${NC}"
    echo -e "${CYAN}══════════════════════════════════════════════════════════════${NC}"
    echo -e ""
    echo -e "${CYAN} Please wait...${NC}"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}!${NC} $1"
}

print_info() {
    echo -e "${BLUE}→${NC} $1"
}

# ============================================
# ПРОВЕРКИ
# ============================================

check_root() {
    if [[ "${EUID}" -ne 0 ]]; then
        print_error "Этот скрипт должен запускаться с правами root"
        print_info "Используйте: ${BOLD}sudo ./install_archupd.sh${NC}"
        exit 1
    fi
}

check_distro() {
    if [[ ! -f "/etc/arch-release" ]]; then
        print_error "Этот скрипт предназначен только для Arch Linux"
        exit 1
    fi
}

check_internet() {
    if ! ping -c 1 archlinux.org &> /dev/null; then
        print_warning "Нет подключения к интернету. Проверьте соединение."
        read -p "Продолжить установку? (y/N): " -n 1 -r
        echo
        if [[ ! "${REPLY}" =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
}

# ============================================
# ОПРЕДЕЛЕНИЕ ПОЛЬЗОВАТЕЛЯ
# ============================================

determine_user() {
    print_info "Определение пользователя системы..."
    
    # Если скрипт запущен через sudo, берем пользователя, который вызвал sudo
    if [[ -n "${SUDO_USER}" ]]; then
        REAL_USER="${SUDO_USER}"
    else
        # Иначе пытаемся определить через who
        REAL_USER=$(who am i | awk '{print $1}' 2>/dev/null || echo "${USER}")
    fi
    
    # Безопасное получение домашней директории
    if [[ "${REAL_USER}" == "root" ]]; then
        REAL_HOME="/root"
    else
        REAL_HOME=$(getent passwd "${REAL_USER}" 2>/dev/null | cut -d: -f6)
    fi
    
    # Fallback если не удалось определить
    if [[ -z "${REAL_HOME}" ]]; then
        REAL_HOME="${HOME}"
        print_warning "Не удалось определить домашнюю директорию, использую ${REAL_HOME}"
    fi
    
    # Путь для бэкапов пользователя
    BACKUP_DIR="${REAL_HOME}/Documents/Arch_Backups"
    
    print_success "Пользователь: ${REAL_USER}"
    print_success "Домашняя директория: ${REAL_HOME}"
    print_success "Директория бэкапов: ${BACKUP_DIR}"
}

# ============================================
# УСТАНОВКА ЗАВИСИМОСТЕЙ
# ============================================

install_dependencies() {
    print_info "Установка зависимостей..."
    
    local dependencies=("timeshift" "cronie" "pacman-contrib" "rsync")
    local missing_packages=()
    
    # Проверяем какие пакеты уже установлены
    for pkg in "${dependencies[@]}"; do
        if ! pacman -Qi "${pkg}" &> /dev/null; then
            missing_packages+=("${pkg}")
        fi
    done
    
    if [[ ${#missing_packages[@]} -eq 0 ]]; then
        print_success "Все зависимости уже установлены"
        return 0
    fi
    
    # Обновляем базу пакетов
    print_info "Обновление базы пакетов..."
    pacman -Sy --noconfirm
    
    # Устанавливаем недостающие пакеты
    print_info "Установка пакетов: ${missing_packages[*]}"
    pacman -S --needed --noconfirm "${missing_packages[@]}"
    
    # Включаем cronie если не включен
    if ! systemctl is-enabled cronie &> /dev/null; then
        print_info "Включение службы cronie..."
        systemctl enable --now cronie
    fi
    
    print_success "Зависимости установлены"
}

# ============================================
# СОЗДАНИЕ ДИРЕКТОРИЙ
# ============================================

create_directories() {
    print_info "Создание структуры директорий..."
    
    # Системные директории (от root)
    mkdir -p "${CONFIG_DIR}"
    mkdir -p "${LOG_DIR}"
    mkdir -p "${BIN_DIR}"
    
    # Пользовательские директории (от имени пользователя)
    if [[ "${REAL_USER}" != "root" ]]; then
        sudo -u "${REAL_USER}" mkdir -p "${BACKUP_DIR}"
        sudo -u "${REAL_USER}" mkdir -p "${REAL_HOME}/.local/log"
    else
        mkdir -p "${BACKUP_DIR}"
        mkdir -p "/root/.local/log"
    fi
    
    # Устанавливаем правильные права
    chmod 755 "${CONFIG_DIR}"
    chmod 755 "${LOG_DIR}"
    chmod 755 "${BIN_DIR}"
    
    print_success "Директории созданы"
}

# ============================================
# УСТАНОВКА ОСНОВНОГО СКРИПТА
# ============================================

install_main_script() {
    print_info "Установка основного скрипта archupd..."
    
# Создаем скрипт в /usr/local/bin/archupd
# ----------------------------------------------------------------------------------------------------------------- #
# --------------------------------- script body begin (line from '#!/bin/bash') =---------------------------------- #
# ----------------------------------------------------------------------------------------------------------------- #
    cat > "${BIN_DIR}/archupd" << 'EOF'
#!/bin/bash

# ══════════════════════════════════════════════════════════════════════════════ #
# | ArchUpd: A script for safely updating Arch Linux                           | #
# | Version: v.2.2-2026-sys_ru                                                 | #
# | Author: LeoIKT                                                             | #
# | License: MIT                                                               | #
# ══════════════════════════════════════════════════════════════════════════════ #

readonly SCRIPT_VER=v.2.2-2026-sys_ru
set -euo pipefail

# При запуске без sudo перезапуститься с правами root
[[ $EUID -ne 0 ]] && exec sudo -E "$0" "$@"

if [[ -n "${SUDO_USER:-}" ]]; then
    REAL_USER="${SUDO_USER}"
else
    REAL_USER="${USER:-root}" # Fallback на root, если даже USER пуст
fi

if [[ "${REAL_USER}" == "root" ]]; then
    REAL_HOME="/root"
else
    REAL_HOME=$(getent passwd "${REAL_USER}" 2>/dev/null | cut -d: -f6)
    
    # Если getent не сработал, используем fallback
    if [[ -z "${REAL_HOME}" ]]; then
        if [[ "${USER}" == "root" ]]; then
            REAL_HOME="/root"
        else
            REAL_HOME="${HOME}"
        fi
    fi
fi

readonly KERNEL_NAME="linux"
[[ -f "/etc/archupd/config.conf" ]] && source "/etc/archupd/config.conf"
readonly VMLINUZ="/boot/vmlinuz-${KERNEL_NAME}"
readonly INITRAMFS="/boot/initramfs-${KERNEL_NAME}.img"
readonly BACKUP_ROOT="${REAL_HOME}/Documents/Arch_Backups"
readonly TIMESTAMP=$(date +%Y%m%d_%H%M%S)
readonly CUR_BACKUP_DIR="${BACKUP_ROOT}/backup_${TIMESTAMP}"
CONFIRM_FLAG="" 
[[ "$*" == *"--noconfirm"* ]] && CONFIRM_FLAG="--noconfirm"

readonly LOG_DIR="${REAL_HOME}/.local/log"
readonly LOG_FILE="${LOG_DIR}/archupd.log"

# Цвета
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'
BOLD='\033[1m'
[[ -f "/etc/archupd/config.conf" ]] && source "/etc/archupd/config.conf"

init_logging() {
    mkdir -p "${LOG_DIR}" 2>/dev/null || true
    touch "${LOG_FILE}" 2>/dev/null || true
}

log() {
    local message="${1}"
    local timestamp=$(date +"%Y-%m-%d %H:%M:%S")

    echo -e "${BLUE}[$(date +%H:%M:%S)]${NC} ${message}"

    echo "[${timestamp}] ${message}" >> "${LOG_FILE}" 2>/dev/null || true
}

error_exit() { 
    echo -e "${RED}[ОШИБКА]${NC} ${1}" 
    exit 1 
}

cleanup() {
    local exit_code="${?}"
    
    # Возврат /boot в RO режим если нужно
    if findmnt -no OPTIONS /boot 2>/dev/null | grep -q "rw"; then
        log "${YELLOW}Возврат /boot в безопасный режим (RO)...${NC}"
        sudo mount -o remount,ro /boot 2>/dev/null || true
    fi

    log "Скрипт завершен с кодом: ${exit_code}"
    
    if [[ ${exit_code} -ne 0 ]]; then
        echo -e "${RED}✗ Скрипт завершен с ошибкой${NC}"
    fi
}

check_env() {
    log "--- [1/7] Проверка окружения ---"
    
    init_logging
    log "Пользователь: ${REAL_USER}"
    log "Домашняя директория: ${REAL_HOME}"
    log "Текущий пользователь системы: ${USER}"
    log "SUDO_USER: ${SUDO_USER:-не установлен}"
    
    if [[ ! -f /etc/arch-release ]]; then
        error_exit "Этот скрипт работает только на Arch Linux и производных"
    fi
    
    if [[ -f /etc/archiso ]] || [[ -d /run/archiso ]]; then
        error_exit "Скрипт не работает в live-среде Arch"
    fi

    if [[ ${EUID} -eq 0 ]] && [[ -z "${SUDO_USER}" ]]; then
        log "${YELLOW}Внимание: скрипт запущен от root напрямую${NC}"
        log "${YELLOW}Бэкапы будут сохранены в /root${NC}"
    fi
    
    for cmd in pacman timeshift paccache findmnt rsync; do
        if ! command -v "${cmd}" &>/dev/null; then
            error_exit "Не найдена утилита: ${cmd}"
        fi
    done
    
    if [[ ! -d "/boot" ]]; then
        log "${YELLOW}Предупреждение: директория /boot не найдена${NC}"
    fi
    
    local free_space_mb=$(df -m / | awk 'NR==2 {print $4}')
    if [[ ${free_space_mb} -lt 2048 ]]; then
        log "${YELLOW}Мало свободного места: ${free_space_mb}MB (рекомендуется >2GB)${NC}"
    fi

    echo -e "${GREEN}✓ Окружение проверено${NC}"
}

do_backup() {
    log "--- [2/7] Бэкап пользовательских конфигов ---"
    
    mkdir -p "${CUR_BACKUP_DIR}" 2>/dev/null || error_exit "Не удалось создать директорию бэкапа"
    
    local config_dirs=("hypr" "waybar" "kitty")
    local backed_up=0

    for dir in "${config_dirs[@]}"; do
        local source_dir="${REAL_HOME}/.config/${dir}"
        if [[ -d "${source_dir}" ]]; then
            if rsync -a "${source_dir}/" "${CUR_BACKUP_DIR}/${dir}/" 2>/dev/null; then
                log "  → ${dir} скопирован"
                # Используем || true, чтобы set -e не ругался на инкремент
                ((backed_up++)) || true
            else
                log "${YELLOW}  → Не удалось скопировать ${dir}${NC}"
            fi
        else
            log "${YELLOW}  → Директория ${dir} не найдена${NC}"
        fi
    done
    
    if [[ -f "/etc/tlp.conf" ]]; then
        if cp /etc/tlp.conf "${CUR_BACKUP_DIR}/tlp.conf" 2>/dev/null; then
            log "  → Системный tlp.conf скопирован"
        fi
    fi
    
    if sudo pacman -Qqe > "${CUR_BACKUP_DIR}/pkglist.txt" 2>/dev/null; then
        log "  → Список пакетов сохранен"
    else
        log "${YELLOW}  → Не удалось сохранить список пакетов${NC}"
    fi
    
    sudo chown -R "${REAL_USER}:${REAL_USER}" "${CUR_BACKUP_DIR}"

    log "Бэкап сохранен в: ${CUR_BACKUP_DIR}"
    return 0
}

create_snapshot() {
    log "--- [3/7] Снимок Timeshift ---"
    if ! sudo timeshift --create --comments "Before Update ${TIMESTAMP}" --tags D; then
        local confirm
        if ! read -p "Снимок не создан. Продолжить? (y/N): " confirm; then
            log "Пользователь прервал ввод"
            exit 1
        fi
        [[ "${confirm,,}" != "y" ]] && exit 1
    fi
}

prepare_system() {
    log "--- [4/7] Подготовка системы ---"
    sudo sysctl -w kernel.yama.ptrace_scope=2 net.ipv4.conf.all.rp_filter=1 &>/dev/null || true
    if ! findmnt /boot &>/dev/null; then
        sudo mount /boot 2>/dev/null || {
            log "${RED}Не удалось смонтировать /boot${NC}"
            log "Проверьте /etc/fstab и права доступа"
            return 1
        }
    fi
    sudo mount -o remount,rw /boot
}

run_upgrade() {
    log "--- [5/7] Обновление системы ---"
    
    log "Обновление ключей (archlinux-keyring)..."
    sudo pacman -Sy archlinux-keyring ${CONFIRM_FLAG}
    
    log "Запуск полного обновления системы..."
    if sudo pacman -Syu ${CONFIRM_FLAG}; then
        log "Система успешно обновлена"
    else
        error_exit "Ошибка при выполнении pacman -Syu"
    fi
}

verify_kernel() {
    log "--- [6/7] Проверка целостности ядра ---"
    
    local kernel_status=0  # 0=ok, 1=missing, 2=bad_format

    if [[ ! -s "${VMLINUZ}" ]] || [[ ! -s "${INITRAMFS}" ]]; then
        log "${YELLOW}Файлы не читаются. Проверка прав...${NC}"
        sudo chmod +r "${VMLINUZ}" "${INITRAMFS}" 2>/dev/null || true
        
        if [[ ! -s "${VMLINUZ}" ]] || [[ ! -s "${INITRAMFS}" ]]; then 
            kernel_status=1
        fi
    fi

    if [[ ${kernel_status} -eq 0 ]] && command -v file &>/dev/null; then
        local vmlinuz_info=$(file -b "${VMLINUZ}" 2>/dev/null)
        local initrd_info=$(file -b "${INITRAMFS}" 2>/dev/null)

        if [[ ! "$vmlinuz_info" =~ "kernel" ]] && [[ ! "$vmlinuz_info" =~ "executable" ]]; then
             kernel_status=2
        fi
        
        if [[ ! "$initrd_info" =~ "compressed" ]] && [[ ! "$initrd_info" =~ "cpio" ]]; then
             kernel_status=2
        fi
    fi

    case ${kernel_status} in
        1) log "${RED}✗ Файлы ядра отсутствуют или пустые${NC}" ;;
        2) log "${YELLOW}! Файлы ядра имеют сомнительный формат${NC}" ;;
        *) log "${GREEN}✓ Файлы ядра в порядке${NC}" ;;
    esac

    if [[ ${kernel_status} -gt 0 ]]; then
        log "${MAGENTA}Критический сбой. Запуск процедуры ремонта...${NC}"

        if sudo pacman -S ${CONFIRM_FLAG} "${KERNEL_NAME}"; then
            log "${GREEN}Ядро успешно переустановлено.${NC}"
            
            if [[ ! -s "${VMLINUZ}" ]]; then
                 error_exit "РЕМОНТ НЕ УДАЛСЯ! Система может не загрузиться. Срочно переустановите ядро вручную."
            fi
        else
            error_exit "Pacman не смог переустановить ядро. Проверьте соединение!"
        fi
    else
        local size_mb=$(du -h "${VMLINUZ}" | cut -f1)
        log "Детали: ${size_mb} [${vmlinuz_info%%,*}]"
    fi

    sudo mount -o remount,ro /boot 2>/dev/null || true
}


clean_maintenance() {
    log "--- [7/7] Очистка кэша пакетов ---"
    
    if sudo paccache -r -k 2 -q 2>/dev/null; then
        log "Кэш очищен"
    else
        log "${YELLOW}Кэш пуст или уже очищен${NC}"
    fi
    
    return 0
}

run_full_cycle() {
    if [[ ${EUID} -ne 0 ]]; then
        echo -e "${RED}Ошибка: Для полного цикла обновления требуются права sudo.${NC}"
        echo -e "${YELLOW}Для обновления выполните:${NC} sudo ./archupd.sh"
        exit 1
    fi

    check_env
    do_backup
    create_snapshot
    prepare_system
    run_upgrade
    verify_kernel
    clean_maintenance
    
    echo -e "\n${GREEN}════════════════════════════════════════${NC}"
    echo -e "${BOLD}${GREEN}   ОБНОВЛЕНИЕ ЗАВЕРШЕНО УСПЕШНО          ${NC}"
    echo -e "${GREEN}════════════════════════════════════════${NC}"
    
    local duration=$SECONDS
    local min=$(($SECONDS / 60))
    local sec=$(($SECONDS % 60))

    log "Общее время выполнения: ${min}м ${sec}с"

    read -p "Перезагрузить систему сейчас? (y/N): " rb
    if [[ "${rb,,}" == "y" ]]; then
        log "Перезагрузка..."
        sudo reboot
    else
        log "Перезагрузка отложена"
        echo -e "${YELLOW}Не забудьте перезагрузиться позже!${NC}"
    fi
    
    exit 0
}

case "${1:-}" in
    "--verify")
        trap cleanup EXIT INT TERM
        if ! findmnt /boot &>/dev/null; then sudo mount /boot; fi
        sudo mount -o remount,rw /boot &>/dev/null || true
        verify_kernel
        exit 0
        ;;
    
    "--check")
        trap cleanup EXIT INT TERM
        
        check_env
        echo -e "${GREEN}✓ Система готова к обновлению${NC}"
        exit 0
        ;;
    
    "--help"|"-h")
        echo -e "${BOLD}ArchUpd ${SCRIPT_VER}${NC}"
        echo "Использование:"
        echo "  archupd                - Полный цикл обновления"
        echo "  archupd --verify       - Проверка и ремонт ядра"
        echo "  archupd --check        - Проверка окружения"
        echo "  archupd --noconfirm    - Проверка окружения"
        echo "  archupd --help или -h  - Эта справка"
        echo ""                      
        echo "Запуск:"               
        echo "  archupd          - Для полного обновления"
        echo "  archupd --check  - Для проверки"
        exit 0
        ;;
    
    "")
        trap cleanup EXIT INT TERM
        run_full_cycle
        ;;
    
    *)
        echo -e "${RED}Неизвестный аргумент: ${1}${NC}"
        echo "Используйте: archupd --help"
        exit 1
        ;;
esac

EOF
# ----------------------------------------------------------------------------------------------------------------- #
# -------------------------------------- script body end (line before 'EOF') -------------------------------------- #
# ----------------------------------------------------------------------------------------------------------------- #

    # Устанавливаем права
    chmod 755 "${BIN_DIR}/archupd"
    chown root:root "${BIN_DIR}/archupd"
    
    print_success "Основной скрипт установлен в ${BIN_DIR}/archupd"
}

# ============================================
# КОНФИГУРАЦИЯ TIMESHIFT
# ============================================

configure_timeshift() {
    print_info "Настройка Timeshift..."
    
    # Создаем базовую конфигурацию если ее нет
    if [[ ! -f "/etc/timeshift/timeshift.json" ]]; then
        print_info "Создание базовой конфигурации Timeshift..."
        
        mkdir -p /etc/timeshift
        
        cat > /etc/timeshift/timeshift.json << 'TS_EOF'
{
  "backup_device_uuid" : "auto",
  "parent_device_uuid" : "",
  "do_first_run" : "false",
  "btrfs_mode" : "false",
  "include_btrfs_home" : "false",
  "stop_cron_emails" : "true",
  "schedule_monthly" : "false",
  "schedule_weekly" : "false",
  "schedule_daily" : "false",
  "schedule_hourly" : "false",
  "schedule_boot" : "false",
  "count_monthly" : "2",
  "count_weekly" : "3",
  "count_daily" : "5",
  "count_hourly" : "6",
  "count_boot" : "5",
  "snapshot_size" : "0",
  "snapshot_count" : "0",
  "date_format" : "%Y-%m-%d %H:%M:%S",
  "exclude" : [
    "/home/***",
    "/root/***",
    "/opt/***",
    "/var/lib/docker/***",
    "/var/lib/libvirt/***",
    "/var/log/***",
    "/var/tmp/***",
    "/var/cache/***"
  ],
  "exclude-apps" : []
}
TS_EOF
        
        print_success "Базовая конфигурация Timeshift создана"
    else
        print_success "Timeshift уже настроен"
    fi
}

# ============================================
# НАСТРОЙКА БЕЗОПАСНОСТИ ЯДРА
# ============================================

configure_kernel_hardening() {
    print_info "Настройка безопасности ядра..."
    
    cat > "${CONFIG_DIR}/99-hardened.conf" << 'SYSCTL_EOF'
# ArchUpd Kernel Hardening Configuration
# ========================================

# Защита от дампов и атак на ссылки
fs.suid_dumpable = 0
fs.protected_fifos = 2
fs.protected_regular = 2
fs.protected_hardlinks = 1
fs.protected_symlinks = 1

# Безопасность ядра
kernel.sysrq = 0
kernel.unprivileged_bpf_disabled = 1
kernel.kptr_restrict = 2
kernel.dmesg_restrict = 1
kernel.yama.ptrace_scope = 2

# Сетевой стек
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.default.rp_filter = 1
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv4.conf.all.secure_redirects = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.tcp_syncookies = 1
net.ipv4.tcp_timestamps = 0
SYSCTL_EOF

    # Копируем в системную директорию
    cp "${CONFIG_DIR}/99-hardened.conf" /etc/sysctl.d/99-hardened.conf
    
    # Применяем настройки
    sysctl --system &> /dev/null || true
    
    print_success "Настройки безопасности ядра применены"
}

# ============================================
# НАСТРОЙКА АЛИАСОВ И ПУТЕЙ
# ============================================
configure_command() {
    # Простейшая проверка: если функции log нет, создаем временную
    if ! command -v log &>/dev/null; then
        log() { echo -e "$1"; }
    fi

    log "Настройка системной команды..."
    
    local target_bin="/usr/local/bin/archupd"
    # Убедись, что BIN_DIR определен выше в инсталлере
#    local source_script="${BIN_DIR}/archupd" 

    # 1. Копируем/Обновляем скрипт (лучше использовать -f для перезаписи)
#    if [[ -f "$source_script" ]]; then
#        sudo cp -f "$source_script" "$target_bin"
#    else
#        echo "Ошибка: Исходный файл $source_script не найден!"
#        return 1
#    fi

    # 2. Выставляем права (только один путь к файлу)
    sudo chmod 755 "$target_bin"
    sudo chown root:root "$target_bin"

    # 3. Вычищаем старые алиасы
    local shell_files=(".bashrc" ".zshrc")
    for shell_file in "${shell_files[@]}"; do
        local file_path="${REAL_HOME}/${shell_file}"
        if [[ -f "${file_path}" ]]; then
            # Удаляем старые хвосты (используем sudo, так как файл в home может иметь разные права)
            sed -i '/alias archupd=/d' "$file_path"
            sed -i '/# ArchUpd alias/d' "$file_path"
        fi
    done

    echo -e "\033[0;32m✓ Команда 'archupd' теперь доступна глобально в системе.\033[0m"
}

# ============================================
# СОЗДАНИЕ КОНФИГУРАЦИОННОГО ФАЙЛА
# ============================================

create_config_file() {
    print_info "Создание конфигурационного файла..."
    
    cat > "${CONFIG_DIR}/archupd.conf" << 'CONFIG_EOF'
# ArchUpd Configuration File
# ==========================
# Этот файл автоматически генерируется при установке
# Для изменения настроек отредактируйте этот файл и перезапустите скрипт

# Основные настройки
KERNEL_NAME="linux"                    # Используемое ядро (linux, linux-lts, linux-zen, linux-hardened)
ENABLE_TIMESHIFT_SNAPSHOTS="true"      # Включить автоматические снимки Timeshift
KEEP_PACKAGE_CACHE_VERSIONS="2"        # Сколько версий пакетов хранить в кэше
AUTO_REBOOT="false"                    # Автоматическая перезагрузка после обновления

# Пути
BACKUP_DIR="${HOME}/Documents/Arch_Backups"  # Директория для бэкапов
LOG_DIR="${HOME}/.local/log"            # Директория для логов

# Проверки
CHECK_DISK_SPACE="true"                # Проверять свободное место на диске
REQUIRED_DISK_SPACE_MB="2048"          # Требуемое свободное место (2GB)
VERIFY_KERNEL_INTEGRITY="true"         # Проверять целостность ядра

# Настройки безопасности
PROTECT_BOOT_RO="true"                 # Защищать /boot в режиме только для чтения
APPLY_SYSCTL_HARDENING="true"          # Применять настройки безопасности ядра
CONFIG_EOF

    # Создаем символическую ссылку для удобства
    if [[ "${REAL_USER}" != "root" ]]; then
        ln -sf "${CONFIG_DIR}/archupd.conf" "${REAL_HOME}/.archupd.conf" 2>/dev/null || true
    fi
    
    print_success "Конфигурационный файл создан: ${CONFIG_DIR}/archupd.conf"
}

# ============================================
# СОЗДАНИЕ SYSTEMD-СЕРВИСА (ОПЦИОНАЛЬНО)
# ============================================
create_systemd_service() {
    log "--- Настройка автоматизации systemd ---"
    
    # Путь к системным юнитам
    local unit_dir="/etc/systemd/system"
    
    read -p "Включить еженедельное обновление по расписанию? (y/N): " -n 1 -r
    echo
    
    if [[ "${REPLY}" =~ ^[Yy]$ ]]; then
        # 1. Создаем сервис
        sudo tee "${unit_dir}/archupd.service" > /dev/null << SERVICE_EOF
[Unit]
Description=ArchUpd Secure System Update
After=network-online.target
Wants=network-online.target

[Service]
Type=oneshot
# Используем полный путь и флаг --noconfirm
ExecStart=/usr/local/bin/archupd --noconfirm
User=root
# Запрещаем запуск, если ноутбук работает от батареи
ExecCondition=/usr/bin/sh -c 'on_ac_power'
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
SERVICE_EOF

        # 2. Создаем таймер
        sudo tee "${unit_dir}/archupd.timer" << TIMER_EOF
[Unit]
Description=Weekly ArchUpd Timer

[Timer]
# Запуск каждое воскресенье в 11:00
OnCalendar=Sun 11:00:00
# Если ноут был выключен, запустить сразу после включения
Persistent=true
# Разброс времени (чтобы не нагружать сервера одновременно)
RandomizedDelaySec=3h

[Install]
WantedBy=timers.target
TIMER_EOF

        # 3. Активация
        sudo systemctl daemon-reload
        sudo systemctl enable --now archupd.timer
        
        log "${GREEN}✓ Таймер активирован (Каждое воскресенье в 11:00)${NC}"
        log "${YELLOW}Внимание: автообновление сработает только при питании от сети!${NC}"
    else
        log "Установка таймера пропущена."
    fi
}

# ============================================
# ЗАВЕРШЕНИЕ УСТАНОВКИ
# ============================================

print_summary() {
    echo -e "\n${GREEN}══════════════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}${GREEN}|                   УСТАНОВКА ЗАВЕРШЕНА                      |${NC}"
    echo -e "${GREEN}══════════════════════════════════════════════════════════════${NC}"
    
    echo -e "\n${CYAN}${BOLD}КЛЮЧЕВАЯ ИНФОРМАЦИЯ:${NC}"
    echo -e "  ${BOLD}Версия:${NC}          ${ARCHUPD_VERSION}"
    echo -e "  ${BOLD}Пользователь:${NC}    ${REAL_USER}"
    echo -e "  ${BOLD}Домашняя директория:${NC} ${REAL_HOME}"
    
    echo -e "\n${CYAN}${BOLD}УСТАНОВЛЕННЫЕ КОМПОНЕНТЫ:${NC}"
    echo -e "  ${BOLD}Основной скрипт:${NC} ${BIN_DIR}/archupd"
    echo -e "  ${BOLD}Конфигурация:${NC}    ${CONFIG_DIR}/"
    echo -e "  ${BOLD}Логи:${NC}            ${LOG_DIR}/"
    echo -e "  ${BOLD}Бэкапы:${NC}          ${BACKUP_DIR}/"
    
    echo -e "\n${CYAN}${BOLD}ИСПОЛЬЗОВАНИЕ:${NC}"
    echo -e "  ${BOLD}Полное обновление без подтверждения:${NC}"
    echo -e "    ${GREEN}archupd --noconfirm${NC}"
    echo -e "  ${BOLD}Проверка системы:${NC}"
    echo -e "    ${GREEN}archupd --check${NC}"
    echo -e "  ${BOLD}Проверка и ремонт ядра при необходимости:${NC}"
    echo -e "    ${GREEN}archupd --verify${NC}"
    echo -e "  ${BOLD}Справка:${NC}"
    echo -e "    ${GREEN}archupd --help${NC}"
    
    echo -e "\n${CYAN}${BOLD}СЛЕДУЮЩИЕ ШАГИ:${NC}"
    echo -e "  1. Применить изменения в shell:"
    echo -e "     ${YELLOW}source ~/.bashrc${NC} или перезапустите терминал"
    echo -e "  2. Проверить установку:"
    echo -e "     ${YELLOW}archupd --check${NC}"
    echo -e "  3. Выполнить первое обновление:"
    echo -e "     ${YELLOW}sudo archupd${NC}"
    
    echo -e "\n${YELLOW}${BOLD}ВАЖНО:${NC}"
    echo -e "  • Скрипт создан для Arch Linux"
    echo -e "  • Используйте на свой страх и риск"
    echo -e "  • Всегда проверяйте бэкапы перед критическими обновлениями"
    echo -e "  • Для обратной связи откройте issue на GitHub"
    
    echo -e "\n${GREEN}══════════════════════════════════════════════════════════════${NC}"
    echo -e "${GREEN}|                Счастливого обновления!                     |${NC}"
    echo -e "${GREEN}══════════════════════════════════════════════════════════════${NC}"
}

# ============================================
# ОСНОВНАЯ ФУНКЦИЯ УСТАНОВКИ
# ============================================

main() {
    print_header
    check_root
    check_distro
    check_internet
    determine_user
    install_dependencies
    create_directories
    install_main_script
    configure_timeshift
    configure_kernel_hardening
    configure_command
    create_config_file
    create_systemd_service
    print_summary
}

# ============================================
# ЗАПУСК СКРИПТА
# ============================================

# Обработка прерываний
trap 'print_error "Установка прервана"; exit 1' INT TERM

# Запускаем установку
main

exit 0
