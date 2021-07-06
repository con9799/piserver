'''
Author: Manuchehr Usmonov
E-mail: con9799@mail.ru
Telegram: @kibertexnik, @devcon, @iplosvoy
https://github.com/con9799/piserver
'''

import serial
import time
import socket
import subprocess

arduino = serial.Serial(port='/dev/ttyAMA0', baudrate=9600, timeout=.1)

hostname = None
ip = None
tunnel = None
cpuusage = None
ramusage = None
totaldisk = None
useddisk = None
useddiskp = None
swapa = None
swapu = None
cputemp = None
gputemp = None
voltage = None
uptime = None
sendGlobalData = False
data = []
def updateData():
    global hostname, ip, tunnel, cpuusage, ramusage, totaldisk, useddisk, useddiskp, swapa, swapu, cputemp, gputemp, voltage, uptime
    syst2 = "uname -n"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    hostname = process.communicate()[0].decode("utf-8").rstrip()
    hostname = '01_'+hostname

    syst2 = "ip a s wlan0 | egrep -o 'inet [0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' | cut -d' ' -f2"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    ip = process.communicate()[0].decode("utf-8").rstrip()
    ip = '02_'+ip

    syst2 = "ip a s lo | egrep -o 'inet [0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' | cut -d' ' -f2"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    tunnel = process.communicate()[0].decode("utf-8").rstrip()
    tunnel = '03_'+tunnel

    syst2 = "ip a s lo | egrep -o 'inet [0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' | cut -d' ' -f2"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    tunnel = process.communicate()[0].decode("utf-8").rstrip()
    tunnel = '03_'+tunnel

    syst2 = "top -d 0.5 -b -n2 | grep \"Cpu(s)\"|tail -n 1 | awk '{print $2 + $4}'"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    cpuusage = process.communicate()[0].decode("utf-8").rstrip()
    cpuusage = '04_'+str(int(float(cpuusage)))+"%"


    syst2 = "free | grep Mem | awk '{print $3/$2 * 100.0}'"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    ramusage = process.communicate()[0].decode("utf-8").rstrip()
    ramusage = '05_'+str(int(float(ramusage)))+"%"

    syst2 = "lsblk -b --output SIZE -n -d /dev/sda1"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    totaldisk = process.communicate()[0].decode("utf-8").rstrip()
    totaldisk = '06_'+'{:,.0f}'.format(float(totaldisk)/float(1<<30))+"Gb"

    syst2 = "df -h --total / | grep total | awk '{ print $3 }'"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    useddisk = process.communicate()[0].decode("utf-8").rstrip()
    useddisk = useddisk.replace("G", "")
    useddisk = '07_'+str(int(float(useddisk)))+"Gb"

    syst2 = "df -h --total / | grep total | awk '{ print $5 }'"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    useddiskp = process.communicate()[0].decode("utf-8").rstrip()
    useddiskp = useddiskp.replace("%", "")
    useddiskp = '08_'+str(int(float(useddiskp)))+"%"


    syst2 = "free | grep Swap | awk '{print $2}'"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    swapa = process.communicate()[0].decode("utf-8").rstrip()
    swapa = '09_'+str(int(round(int(swapa) / 1024, 3)) )+'Mb'

    syst2 = "free | grep Swap | awk '{print $3}'"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    swapu = process.communicate()[0].decode("utf-8").rstrip()
    swapu = '10_'+str(int(round(int(swapu) / 1024, 3)) )+'Mb'

    syst2 = "echo $(($(cat /sys/class/thermal/thermal_zone0/temp) / 1000))"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    cputemp = process.communicate()[0].decode("utf-8").rstrip()
    cputemp = '11_'+str(int(cputemp))+'c'

    syst2 = "ps -e | wc -l"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    gputemp = process.communicate()[0].decode("utf-8").rstrip()
    gputemp = '12_'+str(int(gputemp))

    syst2 = "ps -e | wc -l"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    voltage = process.communicate()[0].decode("utf-8").rstrip()
    voltage = '13_1.2v'

    syst2 = "awk '{print int($1/3600)\":\"int(($1%3600)/60)}' /proc/uptime"
    process = subprocess.Popen(syst2, stdout=subprocess.PIPE, shell=True)
    uptime = process.communicate()[0].decode("utf-8").rstrip()
    uptime = '14_'+uptime

def getData():
    global data, hostname, ip, tunnel, cpuusage, ramusage, totaldisk, useddisk, useddiskp, swapa, swapu, cputemp, gputemp, voltage, uptime
    updateData()
    data.append( hostname )
    data.append( ip )
    data.append( tunnel )
    data.append( cpuusage )
    data.append( ramusage )
    data.append( totaldisk )
    data.append( useddisk )
    data.append( useddiskp )
    data.append( swapa )
    data.append( swapu )
    data.append( cputemp )
    data.append( gputemp )
    data.append( voltage )
    data.append( uptime )
def writeData():
    getData()
    for x in data:
        arduino.write(bytes(x, encoding='utf8'))
        print(x)
        time.sleep(0.3)
time.sleep(5)
while True:
    writeData()
