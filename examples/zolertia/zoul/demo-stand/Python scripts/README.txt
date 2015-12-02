sudo apt-get update && sudo apt-get upgrade

...


Add differents networks to work on differents places

sudo nano /etc/network/intefaces

auto lo
iface lo inet loopback

auto eth0
allow-hotplug eth0
iface eth0 inet dhcp


auto wlan0
allow-hotplug wlan0
iface wlan0 inet dhcp
wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf
iface default inet dhcp


Now moddify and add differents networks 

sudo nano /etc/wpa_supplicant/wpa_supplicant.conf


ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
    ssid=“WIFI_SSID"
    psk=“PASSWORD"
    id_str=“HOME"
}

network={
    ssid=“WIFI_SSID"
    psk=“PASSWORD"
    id_str=“Office"
}


To recognice differents zongles or other device, although are in other port with the same name:

dmesg | grep ttyACM      // or ttyUSB, ttyAMA ...

- We see the ttyACM assigned and replace:

udevadm info -a -p $(udevadm info -q path -n /dev/ttyACM0)
 
We aim product variable data type:, idProduct, idVendor ...

- With data create:
sudo nano /etc/udev/rules.d/99-zolertia-usb.rules

99 is to execute at the end

KERNEL=="ttyACM[0-9]*" , SUBSYSTEMS=="usb", ATTRS{idVendor}=="0451" , ATTRS{idProduct}=="16c8" , ATTRS{product}=="cc2538 System-on-Chip” , SYMLINK+="ZONGLE"

- Remove device and plug in

- now ls /dev/ZONGLE


if you need identify only one device is neccesary place serial 
Si lo que se requiere es identificar cuando haya CUALQUIER Zongle conectada, no es necesario el serial, sino solo los otros valores.



Just start scripts start at the end of raspberry

sudo nano /etc/rc.local


add before  exit 0

python /home/pi/twitter-post-pic/init-raspi-photo-gmail.py
python /home/pi/twitter-post-pi/Serial-photo.py & 

adding '&' at last to infinte loops.




