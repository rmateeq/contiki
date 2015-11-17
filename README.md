The Contiki Operating System
============================

[![Build Status](https://travis-ci.org/contiki-os/contiki.svg?branch=master)](https://travis-ci.org/contiki-os/contiki/branches)

Contiki is an open source operating system that runs on tiny low-power
microcontrollers and makes it possible to develop applications that
make efficient use of the hardware while providing standardized
low-power wireless communication for a range of hardware platforms.

Contiki is used in numerous commercial and non-commercial systems,
such as city sound monitoring, street lights, networked electrical
power meters, industrial monitoring, radiation monitoring,
construction site monitoring, alarm systems, remote house monitoring,
and so on.

For more information, see the Contiki website:

[http://contiki-os.org](http://contiki-os.org)

WALC 2015
===========================

![WALC 2015](http://tecdigital.tec.ac.cr/servicios/hoyeneltec/sites/default/files/field/image/act-101.png "WALC 2015")

For the [WALC 2015](http://eslared.net/walc2015/?page_id=2&lang=es_ES) a week-workshop has been prepared, heavily based on
Contiki and Zolertia platforms.  This years edition takes place in Costa Rica, hosted by the [TEC (Tecnologico de Costa Rica)](http://www.tec.ac.cr/Paginas/index.html)

Track 6 instructors
---------------------------

* Marco Zennaro, ICTP (Coordinator)
* Antonio Lignan, Zolertia
* Alvaro Vives, Nodo6
* Agustín Pelaez, Ubidots
* Ermanno Pietrosemoli, ICTP

Agenda (Track 6 Internet of Things)
---------------------------

* Monday
  - 11:00–12:00: Intro to IoT (Marco Zennaro)
  - 12:00–12:30: Intro to the Lab (Alvaro Vives and Antonio Lignan)
    - Intro to RPi and Zolertia motes
    - Intro to IoT book
    - Objective of the track and project introduction
  -  Lunch
  - 1:30–3:30: Intro to IPv6 (Alvaro Vives)
  - 4:00–6:30: IPv6 Lab with Raspberry Pi (Alvaro Vives)
    
* Tuesday
  - 8–10: IPv6 Advanced (Alvaro Vives)
  - 10:30–12:00: IPv6 Lab and Wireshark (Alvaro Vives)
  - Lunch
  - 1:30–6:30: Product Canvas and Contiki Introduction: basics and sensors (Antonio Lignan)
    - Preparing the workshop project
    - Introduction to Contiki
    - Working with sensors in Contiki and basic examples

* Wednesday
  - 8-12: Wireless with Contiki (IPv6 and UDP over RPL) (Antonio Lignan)
  - Lunch
  - 1:30-2:30: IoT and Entrepreneurship + Demo (Agustín Pelaez)
  - 2-30-4:00: Wireless Technologies for IoT (Ermanno Pietrosemoli)
  - 4:00-5:00: IoT simulation with Cooja (Antonio Lignan)
  - 5:00-6:30: Work in teams: Work on the project!

* Thursday
  - 8-10: IoT Architectures
  - 10-12: IoT Protocols (RESTFull HTTP, COAP, MQTT) (Antonio Lignan)
  - Lunch
  - 1:30-2:30: Interacting with the Ubidots Cloud Platform (Agustin Pelaez)
  - 2:30-4:30: Send data to a real IoT cloud platform (Antonio Lignan)
  - 4:30-6:00: Work in teams: Work on the project!

* Friday
  - 8-10: Energy Considerations (Ermanno)
  - 10:00-12:00: Work in teams: Work on the project!
  - Lunch
  - 1:30-5:00: From the Lab to the field: deployments and presentations

Recommended lectures
---------------------------

* [IoT in 5 days (Internet de las Cosas en 5 días - Spanish)](http://wireless.ictp.it/Papers/InternetdelasCosas.pdf)
* [IoT in 5 days (sources - English)](https://github.com/marcozennaro/IPv6-WSN-book)

Material
---------------------------

The workshop has available the following material:

* [Zolertia Z1 motes](http://zolertia.io/z1)
* [Raspberry Pi 2](https://www.raspberrypi.org/products/raspberry-pi-2-model-b/)
* [Groove sensors](http://www.seeedstudio.com/depot/s/grovefamily.html)
    * [Light sensor](http://www.seeedstudio.com/depot/Grove-Light-SensorP-p-1253.html?cPath=25_27)
    * [PIR motion sensor](http://www.seeedstudio.com/depot/PIR-Motion-sensor-module-p-74.html)
    * [Water flow sensor](http://www.seeedstudio.com/depot/G12-Water-Flow-Sensor-p-635.html)
    * [Loudness sensor](http://www.seeedstudio.com/depot/Grove-Loudness-Sensor-p-1382.html)
* [Phidget sensors](http://www.phidgets.com/)
    * [Rotary angle sensor](http://www.phidgets.com/products.php?product_id=1109)
    * [Temperature sensor](http://www.phidgets.com/products.php?product_id=1124)
    * [Indoor Light sensor](http://www.phidgets.com/products.php?product_id=1142)
    * [Humidity/Temperature sensor](http://www.phidgets.com/products.php?product_id=1125)
    * [Sound Sensor](http://www.phidgets.com/products.php?product_id=1133)
    * [Precision light sensor](http://www.phidgets.com/products.php?product_id=1127)
* [Zolertia Temperature and Humidity sensor](http://www.sensirion.com/en/products/humidity-temperature/humidity-temperature-sensor-sht2x/)

And the following slides:

* Day 1:
    * [The Internet of Things](https://drive.google.com/file/d/0B1xz6K9Dqu_CR2U0SjJ0Z19KSFVuZzdRcDExZWZ1LWFZZnhn/view?usp=sharing)
    * [Introduccion a IPv6](https://drive.google.com/file/d/0B1xz6K9Dqu_CNE8tcFZKTW91LXc/view?usp=sharing)
    * [Laboratorio IPv6 I](https://drive.google.com/file/d/0B1xz6K9Dqu_CbTdoV2F4eFNIdkE/view?usp=sharing)
    * [IoT brainstorming e introducción al workshop](http://www.slideshare.net/alignan/walc15-day-1-iot-and-workshop-introduction)
* Day 2:
    * [IPv6 y el IoT](https://drive.google.com/file/d/0B1xz6K9Dqu_CdV81RG53Qlphb3c/view?usp=sharing)
    * [Laboratorio IPv6 II](https://drive.google.com/file/d/0B1xz6K9Dqu_CVm8wSUZCcVJ2a00/view?usp=sharing)
    * [Preparacion del proyecto](http://www.slideshare.net/alignan/walc15-day-2-preparacion-del-proyecto)
* Day 3:
    * Pending to add
* Day 4:
    * Pending to add
* Day 5:
    * Pending to add

And [Etherpad](http://etherpad.org/) as our colaborative document placeholder

[https://public.etherpad-mozilla.org/p/WALC2015-IoT](https://public.etherpad-mozilla.org/p/WALC2015-IoT)


Acknowledgments
---------------------------

To-Do
