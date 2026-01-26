#!/bin/bash

# ══════════════════════════════════════════════════════════════════════════════ #
# | ArchUpd: A script for safely updating Arch Linux                           | #
# | Version: v.2.2-2026_ru                                                     | #
# | Author: LeoIKT                                                             | #
# | License: MIT                                                               | #
# ══════════════════════════════════════════════════════════════════════════════ #

# ══════════════════════════════════════════════════════════════════════════════ #
# ! Этот "портативный" скрипт archupd.sh вы можете загрузить или скопировать   ! #
# ! файлом и запускать его из любой директории через ./archupd.sh              ! #
# ! Если вы скопировали код из RAW или загрузили zip-архивом из GitHub,        ! #
# ! выдайте разрешение скрипту: sudo chmod 755 filename.sh                     ! #
# ══════════════════════════════════════════════════════════════════════════════ #
# ! Для удобства используйте ./install_archupd.sh                              ! #
# ! Установщик запишет скрипт в /usr/local/bin и                               ! #
# ! Сделает его дотсупным для запуска через команду archupd                    ! #
# ══════════════════════════════════════════════════════════════════════════════ #

readonly SCRIPT_VER=v.2.2-2026-port_ru

# Безопасный режим
set -euo pipefail

# При запуске без sudo перезапуститься с правами root
[[ $EUID -ne 0 ]] && exec sudo -E "$0" "$@"

# ============================================
# ОПРЕДЕЛЕНИЕ ПРАВ ПОЛЬЗОВАТЕЛЯ И ПУТЕЙ
# ============================================
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

# --- Конфигурация ---
readonly KERNEL_NAME="linux"
[[ -f "/etc/archupd/config.conf" ]] && source "/etc/archupd/config.conf"
readonly VMLINUZ="/boot/vmlinuz-${KERNEL_NAME}"
readonly INITRAMFS="/boot/initramfs-${KERNEL_NAME}.img"
readonly BACKUP_ROOT="${REAL_HOME}/Documents/Arch_Backups"
readonly TIMESTAMP=$(date +%Y%m%d_%H%M%S)
readonly CUR_BACKUP_DIR="${BACKUP_ROOT}/backup_${TIMESTAMP}"
CONFIRM_FLAG="" 
# Если запуск с аргументом --noconfirm, устанавливаем флаг
[[ "$*" == *"--noconfirm"* ]] && CONFIRM_FLAG="--noconfirm"

# --- Логи ---
readonly LOG_DIR="${REAL_HOME}/.local/log"
readonly LOG_FILE="${LOG_DIR}/archupd.log"

# Цвета
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'
BOLD='\033[1m'

# --- Переопределение настроек из конфига ---
[[ -f "/etc/archupd/config.conf" ]] && source "/etc/archupd/config.conf"

# ============================================
# ФУНКЦИИ ЛОГИРОВАНИЯ
# ============================================

init_logging() {
    mkdir -p "${LOG_DIR}" 2>/dev/null || true
    touch "${LOG_FILE}" 2>/dev/null || true
}

log() {
    local message="${1}"
    local timestamp=$(date +"%Y-%m-%d %H:%M:%S")
    
    # Вывод в консоль
    echo -e "${BLUE}[$(date +%H:%M:%S)]${NC} ${message}"
    
    # Запись в лог (игнорируем ошибки записи)
    echo "[${timestamp}] ${message}" >> "${LOG_FILE}" 2>/dev/null || true
}

error_exit() { 
    echo -e "${RED}[ОШИБКА]${NC} ${1}" 
    exit 1 
}

# ============================================
# ОСНОВНЫЕ ФУНКЦИИ
# ============================================

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
    
    # Отладочная информация
    log "Пользователь: ${REAL_USER}"
    log "Домашняя директория: ${REAL_HOME}"
    log "Текущий пользователь системы: ${USER}"
    log "SUDO_USER: ${SUDO_USER:-не установлен}"
    
    if [[ ! -f /etc/arch-release ]]; then
        error_exit "Этот скрипт работает только на Arch Linux и производных"
    fi
    
    # Проверка на chroot или live-систему
    if [[ -f /etc/archiso ]] || [[ -d /run/archiso ]]; then
        error_exit "Скрипт не работает в live-среде Arch"
    fi

    # Проверка на прямой запуск от root
    if [[ ${EUID} -eq 0 ]] && [[ -z "${SUDO_USER}" ]]; then
        log "${YELLOW}Внимание: скрипт запущен от root напрямую${NC}"
        log "${YELLOW}Бэкапы будут сохранены в /root${NC}"
    fi
    
    # Проверка утилит
    for cmd in pacman timeshift paccache findmnt rsync; do
        if ! command -v "${cmd}" &>/dev/null; then
            error_exit "Не найдена утилита: ${cmd}"
        fi
    done
    
    # Проверка что мы не в chroot или ином окружении
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

    # 1. Цикл только по пользовательским папкам
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
    
    # 2. Системные конфиги (ВНЕ ЦИКЛА)
    if [[ -f "/etc/tlp.conf" ]]; then
        # Если мы уже под sudo, команда пройдет мгновенно
        if cp /etc/tlp.conf "${CUR_BACKUP_DIR}/tlp.conf" 2>/dev/null; then
            log "  → Системный tlp.conf скопирован"
        fi
    fi
    
    # 3. Бэкап списка пакетов
    # Используем sudo для pacman на случай ограничений
    if sudo pacman -Qqe > "${CUR_BACKUP_DIR}/pkglist.txt" 2>/dev/null; then
        log "  → Список пакетов сохранен"
    else
        log "${YELLOW}  → Не удалось сохранить список пакетов${NC}"
    fi
    
    # 4. Установка владельца (чтобы ты мог открывать бэкапы без sudo)
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
    # --noconfirm пропустит все вопросы [Y/n]
    if sudo pacman -Syu ${CONFIRM_FLAG}; then
        log "Система успешно обновлена"
    else
        error_exit "Ошибка при выполнении pacman -Syu"
    fi
}

verify_kernel() {
    log "--- [6/7] Проверка целостности ядра ---"
    
    local kernel_status=0  # 0=ok, 1=missing, 2=bad_format

    # 1. Проверка существования и прав
    if [[ ! -s "${VMLINUZ}" ]] || [[ ! -s "${INITRAMFS}" ]]; then
        log "${YELLOW}Файлы не читаются. Проверка прав...${NC}"
        sudo chmod +r "${VMLINUZ}" "${INITRAMFS}" 2>/dev/null || true
        
        if [[ ! -s "${VMLINUZ}" ]] || [[ ! -s "${INITRAMFS}" ]]; then 
            kernel_status=1
        fi
    fi

    # 2. Проверка формата (учитываем современные методы сжатия Zstd/Zsetup)
    if [[ ${kernel_status} -eq 0 ]] && command -v file &>/dev/null; then
        local vmlinuz_info=$(file -b "${VMLINUZ}" 2>/dev/null)
        local initrd_info=$(file -b "${INITRAMFS}" 2>/dev/null)

        # Проверка vmlinuz: ищем упоминание ядра
        if [[ ! "$vmlinuz_info" =~ "kernel" ]] && [[ ! "$vmlinuz_info" =~ "executable" ]]; then
             kernel_status=2
        fi
        
        # Проверка initramfs: это может быть Zstandard или cpio
        if [[ ! "$initrd_info" =~ "compressed" ]] && [[ ! "$initrd_info" =~ "cpio" ]]; then
             kernel_status=2
        fi
    fi

    # 3. Обработка результатов
    case ${kernel_status} in
        1) log "${RED}✗ Файлы ядра отсутствуют или пустые${NC}" ;;
        2) log "${YELLOW}! Файлы ядра имеют сомнительный формат${NC}" ;;
        *) log "${GREEN}✓ Файлы ядра в порядке${NC}" ;;
    esac

    # 4. Восстановление
    if [[ ${kernel_status} -gt 0 ]]; then
        log "${MAGENTA}Критический сбой. Запуск процедуры ремонта...${NC}"
        
        # Переустановка пакета ядра (это пересоберет и vmlinuz, и initramfs)
        if sudo pacman -S ${CONFIRM_FLAG} "${KERNEL_NAME}"; then
            log "${GREEN}Ядро успешно переустановлено.${NC}"
            
            # Финальная проверка после ремонта
            if [[ ! -s "${VMLINUZ}" ]]; then
                 error_exit "РЕМОНТ НЕ УДАЛСЯ! Система может не загрузиться. Срочно переустановите ядро вручную."
            fi
        else
            error_exit "Pacman не смог переустановить ядро. Проверьте соединение!"
        fi
    else
        # Если всё ок, выводим инфо для спокойствия
        local size_mb=$(du -h "${VMLINUZ}" | cut -f1)
        log "Детали: ${size_mb} [${vmlinuz_info%%,*}]"
    fi
    
    # Возврат в RO (безопасность)
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


# ============================================
# ФУНКЦИЯ ПОЛНОГО ЦИКЛА
# ============================================
run_full_cycle() {

    # (legacy)
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


# ============================================
# ТОЧКА ВХОДА (ИСПРАВЛЕННАЯ)
# ============================================
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
        echo "  archupd               - Полный цикл обновления"
        echo "  archupd --verify      - Проверка и ремонт ядра"
        echo "  archupd --check       - Проверка окружения"
        echo "  archupd --noconfirm   - Проверка окружения"
        echo "  archupd --help или -h - Эта справка"
        echo ""                      
        echo "Запуск:"               
        echo "  ./archupd.sh          - Для полного обновления"
        echo "  ./archupd.sh --check  - Для проверки"
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
