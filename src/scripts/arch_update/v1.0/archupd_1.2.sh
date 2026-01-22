#!/bin/bash

echo "================================================"
echo "   ЗАПУСК БЕЗОПАСНОГО ОБНОВЛЕНИЯ ARCHUPD        "
echo "================================================"

# 0. Бэкап конфигов
echo "--- [0/6] Создание бэкапа hyprland, списка пакетов и *.conf  ---"
# Папка для бэкапа
BACKUP_DIR="$HOME/Documents/Arch_cfg_backup_$(date +%Y%m%d)"
mkdir -p "$BACKUP_DIR"

# Копируем конфиги приложений
cp -r ~/.config/hypr "$BACKUP_DIR/"
cp -r ~/.config/waybar "$BACKUP_DIR/"
cp -r ~/.config/kitty "$BACKUP_DIR/"
cp ~/.config/tlp.conf "$BACKUP_DIR/" 2>/dev/null || sudo cp /etc/tlp.conf "$BACKUP_DIR/"

# Копируем системные настройки
sudo cp /boot/loader/entries/arch.conf "$BACKUP_DIR/"
sudo cp /etc/modprobe.d/*.conf "$BACKUP_DIR/"
sudo cp /etc/sysctl.d/*.conf "$BACKUP_DIR/"

# Сохраняем список установленных пакетов (чтобы не вспоминать потом)
pacman -Qqe > "$BACKUP_DIR/pkglist.txt"
echo "Бэкап создан в $BACKUP_DIR"

# 1. Создаем снимок системы через Timeshift
echo "--- [1/6] Создание снимка Timeshift (D) ---"
sudo timeshift --create --comments "Before archupd" --tags D

if [ $? -ne 0 ]; then
    echo "!!! Ошибка создания снимка. Продолжить без бэкапа? (y/n) !!!"
    read -r confirm
    if [ "$confirm" != "y" ]; then exit 1; fi
fi

# 2. Подготовка /boot (перевод в RW)
echo "--- [2/6] Перевод /boot в режим записи (RW) ---"
if ! mountpoint -q /boot; then
    sudo mount /boot
fi
sudo mount -o remount,rw /boot

# 3. Обновление ключей и системы
echo "--- [3/6] Обновление ключей и системы ---"
sudo pacman -Sy archlinux-keyring --noconfirm
sudo pacman -Syu
UPD_STATUS=$?

# 4. Проверка целостности ядра
echo "--- [4/6] Проверка целостности ядра в /boot ---"
if [ -f /boot/vmlinuz-linux ] && [ -f /boot/initramfs-linux.img ]; then
    echo "--- Проверка пройдена: ядро и образы на месте. ---"
    KERNEL_OK=1
else
    echo "!!! КРИТИЧЕСКАЯ ОШИБКА: Файлы ядра не найдены! !!!"
    echo "!!! НЕ ПЕРЕЗАГРУЖАЙТЕСЬ! Переустановите ядро вручную. !!!"
    KERNEL_OK=0
fi

# 5. Возврат /boot в режим защиты (RO)
echo "--- [5/6] Возврат /boot в режим защиты (RO) ---"
sudo mount -o remount,ro /boot

# 6. Чистка старых пакетов
echo "--- [6/6] Очистка кэша старых пакетов (оставляем 2 последние версии) ---"
sudo paccache -r -k 2


# Итоговый отчет
if [ $UPD_STATUS -eq 0 ] && [ $KERNEL_OK -eq 1 ]; then
    echo "================================================"
    echo "   ОБНОВЛЕНИЕ ЗАВЕРШЕНО УСПЕШНО                 "
    echo "   Перезагрузка через 5 секунд...               "
    echo "================================================"
    sleep 5
    sudo reboot
else
    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    echo "   ОШИБКА! ПЕРЕЗАГРУЗКА ОТМЕНЕНА.               "
    echo "   Проверьте систему вручную перед выключением. "
    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    exit 1
fi
