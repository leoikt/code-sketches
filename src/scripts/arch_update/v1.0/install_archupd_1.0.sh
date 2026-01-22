#!/bin/bash

# ================================================================
# INSTALLER FOR ARCHUPD (Hardening Edition 2026)
# ================================================================

echo "--- Начинаем установку тулчейна archupd ---"

# 1. Проверка и установка зависимостей
dependencies=("timeshift" "cronie")
for pkg in "${dependencies[@]}"; do
    if ! pacman -Qs "$pkg" > /dev/null; then
        echo "--- Пакет $pkg не найден. Установка... ---"
        sudo pacman -S "$pkg" --noconfirm
    else
        echo "--- Пакет $pkg уже установлен. ---"
    fi
done

# Включаем и запускаем cronie (нужен для работы расписания Timeshift)
sudo systemctl enable --now cronie

# 2. Создание папки и самого скрипта обновления
SCRIPT_DIR="$HOME/scripts"
mkdir -p "$SCRIPT_DIR"

cat << 'EOF' > "$SCRIPT_DIR/archupd.sh"
#!/bin/bash
echo "================================================"
echo "   ЗАПУСК БЕЗОПАСНОГО ОБНОВЛЕНИЯ ARCHUPD        "
echo "================================================"

echo "--- [1/5] Создание снимка Timeshift ---"
sudo timeshift --create --comments "Before archupd" --tags D
if [ $? -ne 0 ]; then
    echo "!!! Ошибка снимка. Продолжить без бэкапа? (y/n) !!!"
    read -r confirm
    if [ "$confirm" != "y" ]; then exit 1; fi
fi

echo "--- [2/5] Перевод /boot в режим записи (RW) ---"
if ! mountpoint -q /boot; then sudo mount /boot; fi
sudo mount -o remount,rw /boot

echo "--- [3/5] Обновление системы ---"
sudo pacman -Sy archlinux-keyring --noconfirm
sudo pacman -Syu
UPD_STATUS=$?

echo "--- [4/5] Проверка целостности ядра ---"
if [ -f /boot/vmlinuz-linux ] && [ -f /boot/initramfs-linux.img ]; then
    echo "--- Ядро на месте. ---"
    KERNEL_OK=1
else
    echo "!!! КРИТИЧЕСКАЯ ОШИБКА: Ядро потеряно! !!!"
    KERNEL_OK=0
fi

echo "--- [5/5] Возврат /boot в защиту (RO) ---"
sudo mount -o remount,ro /boot

if [ $UPD_STATUS -eq 0 ] && [ $KERNEL_OK -eq 1 ]; then
    echo "Успех. Ребут через 5 сек."
    sleep 5
    sudo reboot
else
    echo "Ошибка! Ребут отменен."
    exit 1
fi
EOF

chmod +x "$SCRIPT_DIR/archupd.sh"
echo "--- Скрипт archupd.sh создан в $SCRIPT_DIR ---"

# 3. Авто-настройка Timeshift (Daily 5 + RSYNC)
echo "--- Настройка конфигурации Timeshift (Daily 5) ---"
sudo mkdir -p /etc/timeshift

# Генерируем конфиг
sudo cat << EOF | sudo tee /etc/timeshift/timeshift.json > /dev/null
{
  "backup_device_uuid": "$(lsblk -no UUID $(df / | tail -n1 | awk '{print $1}'))",
  "parent_device_uuid": "",
  "do_first_run": "false",
  "btrfs_mode": "false",
  "include_btrfs_home_for_snapshots": "false",
  "include_btrfs_home_for_restore": "false",
  "stop_cron_emails": "true",
  "schedule_monthly": "false",
  "schedule_weekly": "false",
  "schedule_daily": "true",
  "schedule_hourly": "false",
  "schedule_boot": "false",
  "count_monthly": "2",
  "count_weekly": "3",
  "count_daily": "5",
  "count_hourly": "6",
  "count_boot": "5",
  "snapshot_boot": "false",
  "date_format": "%Y-%m-%d %H:%M:%S",
  "exclude": [
    "/home/$(whoami)/.cache/**",
    "/root/.cache/**",
    "/run/**",
    "/proc/**",
    "/sys/**",
    "/dev/**",
    "/tmp/**"
  ],
  "exclude-apps": []
}
EOF

# 3.5. Настройка ужесточения ядра (Hardening sysctl)
echo "--- [3.5] Применение настроек ядра для повышения индекса безопасности ---"
sudo cat << EOF | sudo tee /etc/sysctl.d/99-hardened.conf > /dev/null
# Защита от дампов памяти
fs.suid_dumpable = 0
# Защита FIFO и регулярных файлов в /tmp
fs.protected_fifos = 2
fs.protected_regular = 2
# Отключение Magic SysRq
kernel.sysrq = 0
# Ограничение BPF
kernel.unprivileged_bpf_disabled = 1
# Сетевая безопасность
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.default.accept_redirects = 0
net.ipv6.conf.all.accept_redirects = 0
net.ipv6.conf.default.accept_redirects = 0
net.ipv4.conf.all.log_martians = 1
net.ipv4.conf.default.log_martians = 1
net.ipv4.conf.all.rp_filter = 1
EOF
sudo sysctl --system

# 4. Добавление алиасов
add_alias() {
    local rc_file=$1
    if [ -f "$rc_file" ]; then
        if ! grep -q "alias archupd=" "$rc_file"; then
            echo "alias archupd='$SCRIPT_DIR/archupd.sh'" >> "$rc_file"
            echo "--- Алиас добавлен в $rc_file ---"
        fi
    fi
}

add_alias "$HOME/.bashrc"
add_alias "$HOME/.zshrc"

echo    "================================================"
echo -e "\n--- Установка завершена! ---"
echo    "Чтобы начать пользоваться командой archupd прямо сейчас, введите:"
echo -e "\033[1;32msource ~/.zshrc\033[0m"
echo    "   Затем запускайте обновление: archupd         "
echo    "================================================"
