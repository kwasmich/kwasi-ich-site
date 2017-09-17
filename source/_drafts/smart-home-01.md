---
title: This makes my home smart
date: 2017-06-18 15:00:00
updated: 2017-06-19 21:00:00
comments: true
tags:
    - smart home
    - C
    - RPi
photos: images/dummy2.png
author: Michael Kwaśnicki
---
This is an attempt to make a smart home all by my own. The idea is to make use of common
technologies and devices and combine all together.

<!-- more -->

My aim is to to develop a system with sensors that can send events to the main core, process
the events and perform actions accordingly.



## Sensors

There is a variety of sensors I have in my mind:

* State of a window (is it shut, open or tilted).
* State of a door (is it open, closed or even locked).
* Temperature and humidity (outside and inside).
* Pressure outside.
* State change of my hue lamps.
* State of a switch (replacement).
* PIR presence/motion sensors.
* Water sensor in the basement.
* Sense the presence of a WiFi device.
* Outside brigtness using a tiny cheap PV module.
* Register the TV-Remote signals.
* …
* Much more ideas to come!



## Central Processing

All the information collected by sensors form events. Those can be time based, like for the temperature sensor or event driven like the opening or closing of a door. has to be transmitted to the central system. Most of this will be
done wirelessly. In the central system the incoming sensor data will be decoded and tagged
such that for instance the door #3 will be tagged as "door, livingroom" so that events
can be made out of it. The events can then be combined in a way much more powerful than
IFTTT. I want to be able to make something like:

If the door to the livingroom is open and it is cold outside and it is dark outside and
noone is moving in the whole flat, then turn off the light in the livingroom.

```
    if door to the livingroom is open
and if outside temperature < 15°C
and if outside brightness < 100cd
and if last motion event > 15 minutes ago

then turn off the light in the livingroom
```


## Actuators

* My hue lighting.
* Control appliances via IR transmitter.
* Output sound over speakers or piezo elements.


>> David Levithan, Wide Awake  
> Do not just seek happiness for yourself. Seek happiness for all. Through kindness. Through mercy.


``` objc title http://kwasi-ich.de kwasi-ich
[rectangle setX: 10 y: 10 width: 20 height: 20];
```

asdf
: abasdfasdf

asdfs
: asdfqwer

qwer
: qwerzuio

{% asset_img smart-home-01.png - %}
