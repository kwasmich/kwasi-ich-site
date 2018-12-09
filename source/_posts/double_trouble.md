---
title: Twice the trouble for half the price
author: Michael Kwaśnicki
date: 2018-12-09
tags: [Arduino, Hardware, Raspberry Pi]
#photos: rpiomx.svg
---

If something is sold with phrases like "like ..." or "compatible to ..." or "drop in replacement for ..." and in the same time it is much cheaper, then you should think twice if it is worth the money.
In some cases I had to learn it the hard way.

<!-- more -->

**Disclaimer:**
This is a collection of experiences I made on my own with the hope that it might save you some trouble.
The links to products are not recommendations and I don't get any affiliates for them.
They are just examples of devices I purchased and what the difference between the cheaper and the pricier ones are.

This list will be updated as I find more examples for products that cost the half, provide half of the claimed value or are troublesome in other ways.



USB to Ethernet Adapter (JP1082 vs. LogiLink UA0025C)
-----------------------------------------------------
Adaptors labelled __JP1082__ like [this one on AliExpress][JP1082] on the bottom and otherwise have no markings are not worth their money.
In earlier versions of Raspbian (I think it was prior to 2017, perhaps it was just an issue in Jessie) they caused kernel panics right at the moment you plug them in.
In later versions they seemed to work well until you upgrade your network to use IPv6.
Those devices are __NOT__ capable of acquiring a global IPv6 address.
That caused me a lot of trouble to find that out.

If you want to use additional Ethernet ports with your Raspberry Pi, then consider using devices that are sold by a brand that is known for manufacturing network equipment and are therefore experienced.
I switched to [LogiLink UA0025C][UA0025C] of which the JP1082 is a knock-off.



5V 1A USB power supply (5V-1A vs. Apple iPhone charger)
-------------------------------------------------------
Some cheap USB phone chargers that look exactly like the ones from Apple iPhones should be omitted.
Apple's original charger provide 1A of current at 5V.
The knock-offs state to provide [5V-1A][5V-1A] but in reality they provide only 0.5A.
It is funny that the URL states it is 500mA.
But it is not mentioned anywhere else on the page.

This is some recurring phenomenon where Chinese ratings are completely exaggerated and in most cases by factor two.
This is what [Andreas Spiess][Spiess] tells us on his YouTube videos.
So if you need 1A then you should buy a 2A charger if you want to spend your money on AliExpress.



Bluetooth to UART module (SPP-C vs HC-05)
-----------------------------------------
I am not sure if this really is a general issue but I had severe trouble with the drop-in replacement for the HC-05 named [SPP-C].
After some research I can only say that the module is __NOT COMPATIBLE__ with the Broadcom 2046 Bluetooth host chip.

As I wrote in the [Arduino StackExchange post][SPP-ArduinoStackExchange], I was not able to reproduce the issue with other Bluetooth host chips.
But since I cannot rule out other possible incompatibilities, I'd recommend you to stick with the real [HC-05].



Atmel microcontroller programmer (USBtinyISP vs. Atmel AVRISP mkII)
-------------------------------------------------------------------
The [USBtinyISP][USBtinyISP] has limitations that are not stated anywhere.
While trying to upgrade my 3D-Printer to the latest firmware from Marlin I ran into those limitations.
My 3D-Printer is an [Anet A6][AnetA6] which is a Prusa i3 clone.
The ATmega1284P is the heart (or brain) of the printer.

Before trying to burn the new firmware I tried to backup the current firmware.
But it didn't work.
I could not get past 50%.
While trying to flash the new firmware onto it using USBtinyISP I was encountering errors.

It turned out that the USBtinyISP cannot read or write past 64 kiB.
Which is problematic since the ATmega1284P has 128 kiB of flash memory.
Luckily I could get an original [Atmel AVRISP mkII][AVRISP] (which is discontinued) and successfully flash the firmware of my 3D-Printer.



Bluetooth AUX Receiver
----------------------
Bluetooth audio receivers with regular 3.5mm headphone jack are a cheap way to upgrade your existing car radio (or headphones) with Bluetooth.
But [this particular Bluetooth audio receiver][BT-Audio1a] has a major flaw, as it cannot play music while charging (rendering it useless in a car).
Also the claimed 4h playback time when fully charged is twice as long as in reality.
(Other seller claim just [2.5h playback][BT-Audio1b] for exactly the same device.)
When the battery gets low the music is muted every few seconds and an annoying beep sounds.

The [other Bluetooth audio receiver][BT-Audio2] can play while charging, does have even less than half of the claimed playback time of 8h and is not that annoying when the battery is about to die.

With both models I was able to connect to multiple Bluetooth host devices but only I stream was able to play at the time.
And in both cases I was unable to use the built-in microphone.



[JP1082]:   https://www.aliexpress.com/item/USB-2-0-To-LAN-RJ45-Ethernet-10-100Mbps-Network-Card-Adapter-CSUG-For-PC-Laptop/32715354849.html
[UA0025C]:  http://logilink.eu/showproduct/UA0025C.htm
[5V-1A]:    https://www.aliexpress.com/item/EU-Wall-Charger-USB-Plug-5V-500mA-AC-Micro-USB-Power-Adapter-For-Iphone-4s-5/32655488918.html
[Spiess]:   http://www.sensorsiot.org
[SPP-ArduinoStackExchange]: https://arduino.stackexchange.com/questions/26873/sending-data-using-bluetooth-spp-c-module-without-a-breakout-board
[SPP-C]:    https://www.aliexpress.com/item/SPP-C-Bluetooth-serial-pass-through-module-wireless-serial-communication-from-machine-Wireless-SPPC-Bluetooth-Module/32819227387.html
[HC-05]:    https://www.aliexpress.com/item/HC-05-Bluetooth-serial-adapter-module-from-one-group-CSR-51-microcontroller/32293429853.html
[USBtinyISP]:   https://www.aliexpress.com/item/1Set-USBTiny-USBtinyISP-AVR-ISP-Programmer-Bootloader-Meag2560-UNO-R3-With-10pin-Programming-Cable/32886299494.html
[AnetA6]:   http://www.anet3dprinter.com/e_products/A6-Prusa-i3-3d-printer-353.html
[AVRISP]:   https://www.microchip.com/developmenttools/ProductDetails/PartNo/atavrisp2
[BT-Audio1a]:   https://www.aliexpress.com/item/New-Hot-3-5mm-Jack-Clip-on-Wireless-Bluetooth-Receiver-Mini-Bluetooth-Car-kit-Audio-Music/32870023612.html
[BT-Audio1b]:   https://www.aliexpress.com/item/Universal-3-5mm-jack-Bluetooth-Aux-Audio-Receiver-Adapter-Hands-free-Car-Kit-Music-Receiver-with/32956106509.html
[BT-Audio2]:    https://www.aliexpress.com/item/Bluetooth-AUX-Audio-3-5MM-Jack-Music-Bluetooth-Receiver-Car-Kit-Wireless-Speaker-Headphone-Adapter-Hands/32829768711.html
