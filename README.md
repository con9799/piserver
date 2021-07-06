
# piserver
Raspberry pi server lite uchun atmega328p asosida qo'shimcha boshqaruv qism. 

> Atmega 328: 
> Menyu boshqaruvi uchun: PB4, PBS 
> Bazzer: PB2  
> Sovutish ventilyatori: PD5 (pwm asosida)
> WS2812b: PD7

![atmega328](https://raw.githubusercontent.com/con9799/piserver/main/asset/atmega328-pinout.png)

# Server qismni sozlash:

    sudo nano /etc/rc.local

Faylning eng oxiriga ushbu qatorni qo'shib server faylni saqlang va `sudo reboot` 

    sudo python3 /server.py &
