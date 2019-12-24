This document is created to help with workspace setup.

This document is created and tested on ``` Ubuntu 19.10 with kernel 5.3.0-24-generic ```.

Prerequisites

```
sudo apt install python python-pip git
```

Add the user to dialout group, so it has permission to use serial devices. (No need to change $USER)

```
sudo usermod -a -G dialout $USER
```

Create a folder on your home directory to put toolchaing and SDK.

```
mkdir ~/esp
cd ~/esp
git clone https://github.com/espressif/ESP8266_RTOS_SDK.git
```
Go and download latest toolchain for Linux(64) from [github](https://github.com/espressif/ESP8266_RTOS_SDK).

Extract xtensa-lx106-elf to ~/esp

From command line 

```bash
tar -xzf /Downloaded_File/ -C ~/esp/
or
cd ~/esp 
tar -xzf /path/to/downloaded/file/
```

Need to update PATH to be able to use newly installed tools.

```
nano ~/.profile
```

Add these to the bottom. (No need to change $HOME)

```bash
export PATH="$PATH:$HOME/esp/xtensa-lx106-elf/bin"
export IDF_PATH=$HOME/esp/ESP8266_RTOS_SDK
export PATH="$PATH:$HOME/.local/bin"
```

Log out or restart your computer.

Toolchain uses esptool.py to flash the device but it needs some pip packages.

```
python -m pip install --user -r ~/esp/ESP8266_RTOS_SDK/requirements.txt
```

You are all done.

To test your tool-chain.

```bash
mkdir esp_test
cp -r ~/esp/ESP8266_RTOS_SDK/examples/get-started/hello-world ~/esp_test
cd ~/esp_test/hello_world
```

Plug in your dev board or serial flash board.

```
dmesg | grep tty
```

will produce connected serial devices like below. I'm using a cp2102 serial flash chip. If you are using ch340 or ftdi look for them. Note the connected port ```ttyUSB0```

```bash
[    0.143973] printk: console [tty0] enabled
[16179.376136] usb 1-1: cp210x converter now attached to ttyUSB0
```

You should be in the hello_world directory right now.

Run below command

```bash
make menuconfig
```

Select "Serial Flasher Config" and check if Default serial port is the same as you noted if not fix it. You may change "Flash size" too. Exit and save the config.

Now everything should be fine to run the device.



```bash
make all
```

If any error produced by this, fix before continuing.

```bash
make flash
```

If you encounter any permission errors, enter ```groups``` into terminal and check if you are included in "dialout" group. If not try adding again or check the internet.

After this your device should be flashed and running. You can run ``` make monitor``` and check if it is indeed running. It will show compile information, some system information and a countdown from 10 before restart.

```make monitor``` requires Ctrl+] combo to exit. Unfotunaly I don't have access to that combo without another modifier and it prevents exiting. I modified idf_monitor inside the SDK with another combo but didn't work for some reason. To reflash the device while in the monitor press```Ctrl+T -> Ctrl+R```. It will flash the device and start monitoring it.





