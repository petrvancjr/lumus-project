#lumus project

![esp32-pins](/assets/esp32-pins.png)

## Setup:

Espressif [2], library builder I'm using [1], original setup [3].

_Note: clear IDF_PATH, PATH variables!_

```bash
sudo apt-get install curl
git clone https://github.com/espressif/esp32-arduino-lib-builder
cd esp32-arduino-lib-builder
./tools/update-components.sh
./tools/install-esp-idf.sh
wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
tar -xzf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
rm xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
export IDF_PATH="$HOME/esp32-arduino-lib-builder/esp-idf"
export PATH="$PATH:$HOME/esp32-arduino-lib-builder/xtensa-esp32-elf/bin"
# in lumus-project
make menuconfig
./build.sh
make flash
```

Don't forget to change device port to your own USB. For example /dev/ttyUSB0.
Note: Every restart needs export.
Creating custom project (on different folder). I moved every file except exported files. Then ./build.sh, make menuconfig, make flash.
The file, so I don't have to export every bash opens is ~/.bashrc

### Reading USB from device:

```
sudo apt-get install screen
#      USB port     baud rate
screen /dev/ttyUSB0 115200
```

### Info:

GPIO RGB: 0, 16, 17
GPIO point lights (bodovky): 15
GPIO point lights on/off (bodovky): 21
MAC ubuntu 44:1c:a8:e2:b5:7f
MAC ESP32 24:6f:28:b0:26:d0

### References:

[1] [https://github.com/espressif/esp32-arduino-lib-builder](https://github.com/espressif/esp32-arduino-lib-builder)
[2] [https://docs.espressif.com/projects/esp-idf/en/release-v3.0/get-started/index.html](https://docs.espressif.com/projects/esp-idf/en/release-v3.0/get-started/index.html#setup-toolchain)
[3] [https://github.com/espressif/arduino-esp32/blob/master/docs/lib_builder.md](https://github.com/espressif/arduino-esp32/blob/master/docs/lib_builder.md)
[4] LED and PWM, [http://arduino-er.blogspot.com/2017/06/arduino-core-for-esp32-output-pwm-to.html](http://arduino-er.blogspot.com/2017/06/arduino-core-for-esp32-output-pwm-to.html)
[5] Web Server, [https://randomnerdtutorials.com/esp32-web-server-arduino-ide/](https://randomnerdtutorials.com/esp32-web-server-arduino-ide/)
[6] UDP Sending, [http://www.iotsharing.com/2017/06/how-to-use-udpip-with-arduino-esp32.html](http://www.iotsharing.com/2017/06/how-to-use-udpip-with-arduino-esp32.html)

### Issues & Solutions:

#### DNS [(link)](https://www.notion.so/petovns/Fix-error-dns-632936bd13234869bb74a07dd77a2c79#7b6b43841a084036948ae2a108d5e83d)

In file:
/home/..../components/arduino/libraries/WiFi/src/WiFiSTA.cpp:491

```
IPAddress WiFiSTAClass::dnsIP(uint8_t dns_no)
{
if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
return IPAddress();
}
// ip_addr_t dns_ip = dns_getserver(dns_no);
// return IPAddress(dns_ip.u_addr.ip4.addr);
const ip_addr_t * dns_ip = dns_getserver(dns_no);
return IPAddress(dns_ip->u_addr.ip4.addr);
}
```
In file:
```
/home/..../components/arduino/libraries/WiFi/src/ETH.cpp:196

IPAddress ETHClass::dnsIP(uint8_t dns_no)
{
//ip_addr_t dns_ip = dns_getserver(dns_no);
//return IPAddress(dns_ip.u_addr.ip4.addr);
const ip_addr_t * dns_ip = dns_getserver(dns_no);
return IPAddress(dns_ip->u_addr.ip4.addr);
}
```
