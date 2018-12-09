---
title: Efficient alternative to MJPEG-Streamer
author: Michael Kwaśnicki
date: 2018-03-31
tags: [Raspberry Pi, OpenMAX, Video, Streaming]
---

The simplest way to stream video using HTTP is by using __MJPG-Streamer__ with a compatible camera. This is especially useful for embedded devices like the Raspberry Pi in a bunch of applications. But this can still be improved.


<!-- more -->


## Why using MJPEG ##

[MJPEG][MJPEG] is the simplest way to deliver a video stream over HTTP. It is actually a file stream that consists of JPEG images concatenated with a separation marker to the stream as they are produced.

It has advantages over other formats by providing:

* low latency (variable frame duration)
* arbitrary frame size
* adjustable quality (per frame)
* simple delivery
* wide support

It is not a movie format and thus lacks many features like:

* consistent frame rate
* audio support
* recording and playback
* rewinding and fastforward


### In Detail ###

The low latency comes from the lack of inter frame dependencies. Each frame is compressed individually and does not need to buffer several frames like in real video formats.

JPEG has no constraints on the frame size and provides the user with a high degree of control over the compression quality.

It is supported by most browsers ranging form Smart Phones over Desktops to Smart TVs. (But of course there are exceptions where the browser waits for the end of the download to present the image instead of showing each frame and discarding it after the next one has arrived leading to out-of-memory problems on some devices.)


## What's wrong with MJPEG-streamer? ##

[MJPEG-Streamer][Streamer] is a simple and widely used implementation that takes care of capturing images and delivering them through a built-in HTTP-Server. The original implementation relies on the availability of a V4L2 compatible device. Usually that would be a simple UVC USB webcam. But since we are focussing on the Raspberry Pi, we also have to take the official Raspberry Pi Camera into account. In the beginning the PRi Cam was only accessible via Broadcom's MMAL interface. This is what Liam Jackson did with his fork.

Using the Raspberry Pi's hardware capabilities to capture a frame and encode it into JPEG keeps the heavy work off the CPU. Especially in battery powered applications this can make a huge difference.

When using a simple USB webcam, all the heavy work is left to the CPU. In my tests it occupies a whole CPU core. With older Pis or the Pi Zero having only one core this is not really useful. (There are some special USB webcams that can deliver JPEG images right away. In those case the CPU usage is also not an issue.)

The next disadvantage of MJPEG-Streamer is that it produces an stream even noone is actively listening. Resulting in a 101% CPU load with zero clients.
Only one camera per instance.











[MJPEG]: https://en.wikipedia.org/wiki/Motion_JPEG#M-JPEG_over_HTTP
[Streamer]: https://github.com/jacksonliam/mjpg-streamer
