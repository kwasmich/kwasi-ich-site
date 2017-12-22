---
title: Silencing GreenBox
author: Michael Kwaśnicki
date: 2017-12-22
tags: [3D Printing, STL, OpenSCAD]
photos: greenbox.png
---

I bought a Gigabyte Brix Gaming computer, which is a pretty strong machine for its size. But it has a major drawback: it is incredibly loud. Thus I have 3D printed a case and replaced the two 40 mm fans with 120 mm ones that are quiet.

<!-- more -->


### Introduction ###

My [Gigabyte Brix Gaming GB-BXi5G-760](http://www.gigabyte.de/Mini-PcBarebone/GB-BXi5G-760-rev-10) does a very good job when playing games. I couldn't play on it without in-ear phones as it stands right in front of me on my desk. Using [Steam Link](http://store.steampowered.com/app/353380/Steam_Link/) and putting the computer somewhere else might have been an option but not all games play well on a TV screen or with the noticable bit of latency.

As I've got a 3D printer, I thought that might be a good exercise to dive into CAD. So I tried Autodesk's [Fusion 360](https://www.autodesk.de/products/fusion-360/overview) and also visited a course on that. But I hadn't much success with it. Having a 3D computer graphics software development background I switched over to [OpenSCAD](http://www.openscad.org), which worked better for me, as I consider myself a software developer. So I tried different designs keeping the original air flow direction.

And here is the result:

<figure>
    <a href="{% asset_path green_box_1.jpg %}">{% asset_img t_green_box_1.jpg %}</a>
    <a href="{% asset_path green_box_2.jpg %}">{% asset_img t_green_box_2.jpg %}</a>
    <a href="{% asset_path green_box_3.jpg %}">{% asset_img t_green_box_3.jpg %}</a>
    <figcaption>The final design looks pretty industrial.</figcaption>
</figure>

<figure>
    <a href="{% asset_path green_box_4.jpg %}">{% asset_img t_green_box_4.jpg %}</a>
    <a href="{% asset_path green_box_5.jpg %}">{% asset_img t_green_box_5.jpg %}</a>
    <figcaption>The design consists of two parts that are kept together by a steel rod from a wire hanger cut into pieces and bent with pliers. The wire connecting the fans is lead through the vent holes in the back plate.</figcaption>
</figure>

You can get the STL-files from [Thingiverse](https://www.thingiverse.com/thing:2726222).




### Required Tools ###

* 3D printer (for obvious reasons)
* Pliers (to cut and bend the wire hanger)
* Screwdriver
* A sharp tool to take apart the fan connectors




### Bill of Materials ###

* Black PLA filament
* 2 x 120 mm Fans (I had some spare)
* 2 x [120 mm Grill](https://www.aliexpress.com/item/YOC-3Pcs-120mm-4-7-Metal-Fan-Guard-Protective-Grill-for-PC-Ventilator/32691000340.html)
* 8 x 4x40 mm countersunk screws
* 1 x Wire Hanger
* 2 x [4-pin JST male connector 1.25 mm pitch](https://www.aliexpress.com/item/10-Set-1-25mm-Pitch-Male-Connector-Wire-15CM-Long-28AWG-2-3-4-5-6/32787305004.html) these match the connector inside the Brix
* 2 x [3-pin JST connector 2.54 mm pitch](https://www.reichelt.de/?ARTICLE=185111) for non-PWM fans (these match the connector of my spare fans)
* 4 x Rubber Feet

Links leading to products are just for illustration to give you an idea of what you might need. It is no recommendation or whatsoever.




### Steps ###

__Disclaimer:__ Be aware that you will void the warranty and probably damage your computer if you are not careful. So think twice before you do something you might regret. I take no responsibility for any damage or injuries while carying out these steps!

<figure>
    <a href="{% asset_path green_box_6.jpg %}">{% asset_img t_green_box_6.jpg %}</a>
    <a href="{% asset_path green_box_7.jpg %}">{% asset_img t_green_box_7.jpg %}</a>
    <a href="{% asset_path green_box_8.jpg %}">{% asset_img t_green_box_8.jpg %}</a>
    <a href="{% asset_path green_box_9.jpg %}">{% asset_img t_green_box_9.jpg %}</a>
</figure>

* Unscrew the four (4) long black screws on the bottom plate.
* Remove the bottom plate by carefully lifting it. Depending on your setup there might be a HDD or SSD connected via SATA on the inside of the plate which must be unplugged.
* Unscrew and remove the mSATA SDD (if present).
* Unplug the WiFi and Bluetooth antennas.

<figure>
    <a href="{% asset_path green_box_10.jpg %}">{% asset_img t_green_box_10.jpg %}</a>
    <a href="{% asset_path green_box_14.jpg %}">{% asset_img t_green_box_14.jpg %}</a>
    <a href="{% asset_path green_box_17.jpg %}">{% asset_img t_green_box_17.jpg %}</a>
    <a href="{% asset_path green_box_15.jpg %}">{% asset_img t_green_box_15.jpg %}</a>
</figure>

* Unscrew the one screw that holds the upper part of the mainboard.
* Remove the back plate by pulling it upwards.
* Remove the 40 mm fans by disconnecting them first from the lower part of the mainboard and then pulling them upwards. They are not glued but sit pretty tight.
* The fans are connected in a very inaccessible place. I had fun plugging in the adapter cable I made for my fans.

<figure>
    <a href="{% asset_path green_box_19.jpg %}">{% asset_img t_green_box_19.jpg %}</a>
    <a href="{% asset_path green_box_13.jpg %}">{% asset_img t_green_box_13.jpg %}</a>
    <a href="{% asset_path green_box_12.jpg %}">{% asset_img t_green_box_12.jpg %}</a>
    <a href="{% asset_path green_box_11.jpg %}">{% asset_img t_green_box_11.jpg %}</a>    
</figure>

* Disassemble the connector on the fan and lead each wire through a hole in the back plate.
* Reassemble the connector afterwards.
* Be careful to do it on the right end of the vent holes.
* If done right, the connectors are at the place where there used to be the 40 mm fans.

<figure>
    <a href="{% asset_path green_box_16.jpg %}">{% asset_img t_green_box_16.jpg %}</a>
    <a href="{% asset_path green_box_18.jpg %}">{% asset_img t_green_box_18.jpg %}</a>
    <a href="{% asset_path green_box_20.jpg %}">{% asset_img t_green_box_20.jpg %}</a>
    <a href="{% asset_path green_box_3.jpg %}">{% asset_img t_green_box_3.jpg %}</a>
</figure>

* Connect the fans.
* Plug in the upper part of the mainboard. Make sure the inter mainboard connector stands upright and connects both parts.
* Reassemble the computer in reverse order.
* Form a rod that keeps both parts of the case togehter.
* Assemble the Case.

You're done. Now your Brix should be much quieter.




### Update ###

I upgraded my GreenBox (as I call my Brix) to use PWM fans. That makes the whole thing quieter even still.
