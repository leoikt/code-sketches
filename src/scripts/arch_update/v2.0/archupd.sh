#!/bin/bash

#  ============================================
#  | ArchUpd: Secure Arch Linux Update        |
#  | Version: v.2.1-2026-RU                   |
#  | Author: LeoIKT                           |
#  | License: MIT                             |
#  ============================================

set -euo pipefail

# --- Конфигурация ---
readonly KERNEL_NAME="linux"
readonly VMLINUZ="/boot/vmlinuz-${KERNEL_NAME}"
readonly INITRAMFS="/boot/initramfs-${KERNEL_NAME}.img"
readonly BACKUP_ROOT="$HOME/Documents/Arch_Backups"
readonly TIMESTAMP=$(date +%Y%m%d_%H%M%S)
readonly CUR_BACKUP_DIR="${BACKUP_ROOT}/backup_${TIMESTAMP}"

# Цвета
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; BLUE='\033[0;34m'; NC='\033[0m'; BOLD='\033[1m'

# --- Логирование и очистка ---
log() { echo -e "${BLUE}[$(date +%H:%M:%S)]${NC} $1"; }
error_exit() { echo -e "${RED}[ОШИБКА]${NC} $1"; exit 1; }

cleanup() {
    local exit_code=$?
    
    # Проверяем, смонтирован ли /boot в режиме RW. 
    # Если да — возвращаем в RO. Если нет — ничего не делаем.
    if findmnt -no OPTIONS /boot | grep -q "rw"; then
        log "${YELLOW}Завершение... Возврат /boot в безопасный режим (RO)...${NC}"
        sudo mount -o remount,ro /boot 2>/dev/null || true
    fi

    if [[ $exit_code -ne 0 ]]; then
        echo -e "${RED}Скрипт завершен с ошибкой (код: $exit_code).${NC}"
    fi
}
trap cleanup EXIT INT TERM

# --- ФУНКЦИИ ЭТАПОВ ---

check_env() {
    log "--- [1/7] Проверка окружения ---"
    [[ $EUID -eq 0 ]] && error_exit "Не запускайте скрипт через sudo напрямую."
    for cmd in pacman timeshift paccache findmnt; do
        command -v "$cmd" &>/dev/null || error_exit "Утилита $cmd не найдена."
    done
}

do_backup() {
    log "--- [2/7] Бэкап пользовательских конфигов ---"
    mkdir -p "${CUR_BACKUP_DIR}"
    cp -r ~/.config/{hypr,waybar,kitty} "${CUR_BACKUP_DIR}/" 2>/dev/null || log "${YELLOW}Предупреждение: конфиги не найдены.${NC}"
    pacman -Qqe > "${CUR_BACKUP_DIR}/pkglist.txt"
    log "Бэкап сохранен в: ${CUR_BACKUP_DIR}"
}

create_snapshot() {
    log "--- [3/7] Снимок Timeshift ---"
    if ! sudo timeshift --create --comments "Before Update ${TIMESTAMP}" --tags D; then
        read -p "Снимок не создан. Продолжить на свой страх и риск? (y/N): " confirm
        [[ "${confirm,,}" != "y" ]] && exit 1
    fi
}

prepare_system() {
    log "--- [4/7] Hardening и разблокировка /boot ---"
    sudo sysctl -w kernel.yama.ptrace_scope=2 net.ipv4.conf.all.rp_filter=1 &>/dev/null
    if ! findmnt /boot &>/dev/null; then sudo mount /boot; fi
    sudo mount -o remount,rw /boot
}

run_upgrade() {
    log "--- [5/7] Обновление системы ---"
    sudo pacman -Sy archlinux-keyring --noconfirm
    sudo pacman -Syu
}

verify_kernel() {
    log "--- [6/7] Проверка целостности ядра ---"
    local KERNEL_OK=0
    if [ -s "$VMLINUZ" ] && [ -s "$INITRAMFS" ]; then
        log "${GREEN}Ядро в порядке.${NC}"
        KERNEL_OK=1
    else
        log "${RED}!!! КРИТИЧЕСКАЯ ОШИБКА: ЯДРО ПОВРЕЖДЕНО !!!${NC}"
        log "Попытка автоматического восстановления..."
        sudo pacman -S --noconfirm "$KERNEL_NAME"
        [ -s "$VMLINUZ" ] && KERNEL_OK=1
    fi
    
    # Возврат в RO после проверки
    if mountpoint -q /boot; then sudo mount -o remount,ro /boot; fi
    
    if [[ $KERNEL_OK -eq 0 ]]; then
        error_exit "Не удалось восстановить ядро. НЕ ПЕРЕЗАГРУЖАЙТЕСЬ!"
    fi
}

clean_maintenance() {
    log "--- [7/7] Очистка кэша пакетов ---"
    sudo paccache -r -k 2
}

# --- ПОЛНЫЙ ЦИКЛ ---
run_full_cycle() {
    check_env
    do_backup
    create_snapshot
    prepare_system
    run_upgrade
    verify_kernel
    clean_maintenance
    
    echo -e "\n${GREEN}================================================${NC}"
    echo -e "${GREEN}   ОБНОВЛЕНИЕ ЗАВЕРШЕНО УСПЕШНО                 ${NC}"
    echo -e "${GREEN}================================================${NC}"
    
    read -p "Перезагрузить систему сейчас? (y/N): " rb
    [[ "${rb,,}" == "y" ]] && sudo reboot
}

# ============================================
# Точка входа
# ============================================

case "${1:-}" in
    "--verify")
        # Для проверки ядра всё равно нужно RW, если придется восстанавливать
        if ! findmnt /boot &>/dev/null; then sudo mount /boot; fi
        sudo mount -o remount,rw /boot &>/dev/null || true
        verify_kernel
        ;;
    "--check")
        check_env
        log "${GREEN}Система готова к обновлению.${NC}"
        ;;
    "--help"|"-h")
        echo -e "${BOLD}ArchUpd v.2.1-2026${NC}"
        echo "Использование:"
        echo "  archupd          - Полный цикл (Бэкап + Снимок + Обновление)"
        echo "  archupd --verify - Проверка и восстановление ядра, если битое"
        echo "  archupd --check  - Проверка зависимостей, бэкап конфигов"
        ;;
    "")
        run_full_cycle
        ;;
    *)
        echo -e "${RED}Неизвестный аргумент: $1${NC}. Используйте --help"
        exit 1
        ;;
esac
