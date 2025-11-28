# OnePlus 13 Super Flasher для Arch Linux

## Предупреждение
Также, как в случае прошивки из Windows, Вы запускаете скрипт на свой страх и риск. В случае неправильной конфигурации устройства или вашей системы процесс может завершиться безуспешно или вовсе soft-brick'ом устройства. Ни производитель, ни я, ни авторы оригинального скрипта не будут нести никакую ответственность за вашу неуспешную попытку перепрошивки. В случае с Windows скрипт протестирован и отлажен, здесь же вы рискуете кратно.

## 📋 Описание
Альфа-версия скрипта. Без теста на момент 28.11.2025
Перевод и адаптация Windows-флэшера для перепрошивки OnePlus 13 китайской версии на глобалку.

Скрипт `flasher.sh` поместить в ту же директорию, где находится Windows-скрипт `Super_Flasher.bat`

## 🚀 Запуск

```bash
./flasher.sh
```

или

```bash
sh flasher.sh
```

## ⚙️ Подготовка

### 1. Установка пакетов
```bash
sudo pacman -S android-tools
sudo pacman -S gvfs-mtp mtpfs # mtp mode control
```
*или*
```bash
yay -S android-tools-latest
```
*(или установка другого пакета другим менеджером, содержащего adb & fastboot)*

### 2. Настройка групп пользователя
```bash
sudo groupadd plugdev
sudo usermod -aG plugdev $USER
sudo groupadd adbusers
sudo usermod -aG adbusers $USER
```

**Примечание:** Ваш пользователь должен быть в группах `adbusers` и `plugdev`

### 3. Установка Android udev
```bash
sudo pacman -S android-udev
```

### 4. Настройка udev правил
Проверьте файл `/etc/udev/rules.d/51-android.rules` — если он не существует или пустой, создайте и заполните:

```bash
# Android devices
SUBSYSTEM=="usb", ATTR{idVendor}=="0502", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="0b05", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="413c", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="0489", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="04c5", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="04e8", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="05c6", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="054c", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="0fce", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="1004", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="12d1", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="18d1", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="19d2", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="1bbb", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="22b8", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="25e3", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="2836", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="22d9", MODE="0666", GROUP="adbusers", GROUP="plugdev"
SUBSYSTEM=="usb", ATTR{idVendor}=="2a70", MODE="0666", GROUP="adbusers", GROUP="plugdev"
```

### 5. Применение правил и завершение настройки
```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```
и...
```bash
logout
или
sudo reboot
```

### 6. Проверки
```bash
adb version
fastboot --version
groups
lsusb
ls -la /dev/bus/usb/
getfacl /dev/bus/usb/*/*
adb devices (для подключенного смартфона в режиме отладки)
```

---

**Теперь система готова для работы с Android устройствами через ADB и Fastboot. (но это не точно...)**
**В остальном придерживайтесь базовой инструкции по подготовке и проверке устройства и системы к прошивке**