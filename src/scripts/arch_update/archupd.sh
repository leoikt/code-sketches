#!/bin/bash

echo "================================================"
echo "   ЗАПУСК БЕЗОПАСНОГО ОБНОВЛЕНИЯ ARCHUPD        "
echo "================================================"

# 1. Создаем снимок системы через Timeshift
echo "--- [1/5] Создание снимка Timeshift (D) ---"
sudo timeshift --create --comments "Before archupd" --tags D

if [ $? -ne 0 ]; then
    echo "!!! Ошибка создания снимка. Продолжить без бэкапа? (y/n) !!!"
    read -r confirm
    if [ "$confirm" != "y" ]; then exit 1; fi
fi

# 2. Подготовка /boot (перевод в RW)
echo "--- [2/5] Перевод /boot в режим записи (RW) ---"
if ! mountpoint -q /boot; then
    sudo mount /boot
fi
sudo mount -o remount,rw /boot

# 3. Обновление ключей и системы
echo "--- [3/5] Обновление ключей и системы ---"
sudo pacman -Sy archlinux-keyring --noconfirm
sudo pacman -Syu
UPD_STATUS=$?

# 4. Проверка целостности ядра
echo "--- [4/5] Проверка целостности ядра в /boot ---"
if [ -f /boot/vmlinuz-linux ] && [ -f /boot/initramfs-linux.img ]; then
    echo "--- Проверка пройдена: ядро и образы на месте. ---"
    KERNEL_OK=1
else
    echo "!!! КРИТИЧЕСКАЯ ОШИБКА: Файлы ядра не найдены! !!!"
    echo "!!! НЕ ПЕРЕЗАГРУЖАЙТЕСЬ! Переустановите ядро вручную. !!!"
    KERNEL_OK=0
fi

# 5. Возврат /boot в режим защиты (RO)
echo "--- [5/5] Возврат /boot в режим защиты (RO) ---"
sudo mount -o remount,ro /boot

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
