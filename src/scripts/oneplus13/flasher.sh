#!/bin/bash
# v.0.03

# Функция для обработки ошибок
handle_error() {
    echo "Произошла ошибка! Скрипт завершен."
    exit 1
}

# Устанавливаем обработку ошибок
trap handle_error ERR
set -e

title="OOS Super Flasher"
echo "**********************************************************************"
echo ""
echo "                Oneplus 13 - OOS Super Flasher          "
echo "           Packaged by docnok63, Jonas Salo & Daniel    "
echo "         script inspired by FTH PHONE 1902 and Venkayy  "
echo "         GNU/Linux script porting and testing by LeoIKT "

# Переходим в директорию скрипта
cd "$(dirname "$0")"

# Проверяем наличие fastboot
fastboot="fastboot"
if ! command -v $fastboot &> /dev/null; then
    echo "fastboot не найден. Убедитесь, что он установлен и доступен в PATH."
    exit 1
fi

device_info=$($fastboot getvar product 2>&1 | grep -oP 'product:\s*\K\w+')
if [ "$device_info" != "sun" ]; then 
    echo "ОШИБКА: Устройство не OnePlus 13 CN (PJZ110)! Обнаружено: $device_info"
    exit 1
fi

file="vendor_boot"
echo "************************      START FLASH     ************************"
$fastboot --set-active=a

# Прошиваем основные образы
echo "Прошиваем основные образы..."
$fastboot flash boot OOS_FILES_HERE/boot.img
$fastboot flash dtbo OOS_FILES_HERE/dtbo.img
$fastboot flash init_boot OOS_FILES_HERE/init_boot.img
$fastboot flash modem OOS_FILES_HERE/modem.img
$fastboot flash recovery OOS_FILES_HERE/recovery.img
$fastboot flash vbmeta OOS_FILES_HERE/vbmeta.img
$fastboot flash vbmeta_system OOS_FILES_HERE/vbmeta_system.img
$fastboot flash vbmeta_vendor OOS_FILES_HERE/vbmeta_vendor.img
$fastboot flash vendor_boot OOS_FILES_HERE/vendor_boot.img

# Проверяем наличие super.img
if [ -f "super.img" ]; then
    $fastboot flash super super.img
else
    echo "super.img не найден. Пропускаем..."
fi

# Перезагружаем в fastbootd
echo "*******************      REBOOTING TO FASTBOOTD     *******************"
echo "#################################"
echo "# Выберите English на телефоне  #"
echo "#################################"
$fastboot reboot fastboot

read -n 1 -s -r -p "Нажмите любую клавишу для продолжения..."

# Список исключенных образов
excluded_images=(
    "boot.img" "dtbo.img" "init_boot.img" "modem.img" "recovery.img"
    "vbmeta.img" "vbmeta_system.img" "vbmeta_vendor.img" "vendor_boot.img"
    "my_bigball.img" "my_carrier.img" "my_company.img" "my_engineering.img"
    "my_heytap.img" "my_manifest.img" "my_preload.img" "my_product.img"
    "my_region.img" "my_stock.img" "odm.img" "product.img" "system.img"
    "system_dlkm.img" "system_ext.img" "vendor.img" "vendor_dlkm.img"
)

# Прошиваем остальные образы, кроме исключенных
echo "Прошиваем дополнительные образы..."
for img in OOS_FILES_HERE/*.img; do
    [ -e "$img" ] || continue  # Пропускаем если файлов нет
    filename=$(basename "$img")
    skip=0
    
    for excluded in "${excluded_images[@]}"; do
        if [ "$filename" = "$excluded" ]; then
            skip=1
            break
        fi
    done
    
    if [ $skip -eq 0 ] && [ -f "$img" ]; then
        partition_name="${filename%.img}"
        echo "Прошиваем $partition_name..."
        $fastboot flash --slot=all "$partition_name" "$img"
    fi
done

# Список разделов для логических партиций
partitions=(
    "my_bigball" "my_carrier" "my_engineering" "my_heytap" "my_manifest"
    "my_product" "my_region" "my_stock" "odm" "product" "system" "system_dlkm"
    "system_ext" "vendor" "vendor_dlkm" "my_company" "my_preload"
)

# Обрабатываем логические разделы если super.img отсутствует
if [ ! -f "super.img" ]; then
    echo "Создаем логические разделы..."
    for partition in "${partitions[@]}"; do
        # Добавляем проверку существования файла
        if [ ! -f "OOS_FILES_HERE/${partition}.img" ]; then
            echo "Файл OOS_FILES_HERE/${partition}.img не найден, пропускаем..."
            continue
        fi
        
        $fastboot delete-logical-partition "${partition}_a" 2>/dev/null || true
        $fastboot delete-logical-partition "${partition}_b" 2>/dev/null || true
        $fastboot delete-logical-partition "${partition}_a-cow" 2>/dev/null || true
        $fastboot delete-logical-partition "${partition}_b-cow" 2>/dev/null || true
        $fastboot create-logical-partition "${partition}_a" 1
        $fastboot create-logical-partition "${partition}_b" 1
        $fastboot flash "$partition" "OOS_FILES_HERE/${partition}.img"
    done
else
    echo "super.img найден. Пропускаем создание логических разделов..."
fi

echo "********************** CHECK ABOVE FOR ERRORS **************************"
echo "************** IF ERRORS, DO NOT BOOT INTO SYSTEM **********************"

# Обработка очистки данных
if [ ! -f "super.img" ]; then
    read -p "Do you want to wipe data? (y/n): " -n 1 wipe_choice
    echo
    
    if [[ $wipe_choice =~ ^[Nn]$ ]]; then
        echo "*********************** NO NEED TO WIPE DATA ****************************"
        echo "***** Flashing complete. Hit any key to reboot the phone to Android *****"
        read -n 1 -s -r
        $fastboot reboot
        exit 0
    elif [[ $wipe_choice =~ ^[Yy]$ ]]; then
        echo "****************** FLASHING COMPLETE *****************"
        echo "Wipe data by tapping Format Data on the screen, enter the code, and press format data."
        echo "Phone will automatically reboot into Android after wipe is done."
        read -n 1 -s -r
        exit 0
    fi
fi

# Подготовка к рутированию
read -p "Are you going to root your OP13? (y/n): " -n 1 root_choice
echo

if [[ $root_choice =~ ^[Nn]$ ]]; then
    # Проверяем существование файлов перед прошивкой
    if [ -f "COS_FILES_HERE/oplusstanvbk.img" ] && [ -f "COS_FILES_HERE/my_region.img" ]; then
        echo "***** Flashing COS oplusstanvbk and GLO COS my_region *****"
        $fastboot flash oplusstanvbk_a COS_FILES_HERE/oplusstanvbk.img
        $fastboot flash oplusstanvbk_b COS_FILES_HERE/oplusstanvbk.img
        $fastboot flash my_region_a COS_FILES_HERE/my_region.img
        $fastboot flash my_region_b COS_FILES_HERE/my_region.img
    else
        echo "***** COS files not found, skipping... *****"
    fi
else
    echo "*****         !!!!Flashing NO files for signal!!!!           *****"
    echo "*****     !!!!Root-Module will be needed for signal!!!!      *****"
fi

# Финальный вопрос про очистку данных
read -p "Are you flashing from ColorOS or Want to WIPE DATA?? (y/n): " -n 1 coloros_choice
echo

if [[ $coloros_choice =~ ^[Nn]$ ]]; then
    echo "*********************** NO NEED TO WIPE DATA ****************************"
    echo "***** Flashing complete. Hit any key to reboot the phone to Android *****"
    read -n 1 -s -r
    $fastboot reboot
else
    echo "****************** FLASHING COMPLETE *****************"
    echo "Wipe data by tapping Format Data on the screen, enter the code, and press format data."
    echo "Phone will automatically reboot into Android after wipe is done."
fi

read -n 1 -s -r




