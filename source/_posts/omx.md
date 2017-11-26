---
title: Introduction to OpenMAX on the Raspberry Pi
author: Michael Kwaśnicki
date: 2017-11-26
tags: [Raspberry Pi, OpenMAX]
photos: rpiomx.svg
---

As I want to dive deeper into the Raspberry Pi, I tapped into the hardware accelerated media capabilities. This is my journey into OpenMAX. 

<!-- more -->


## What is OpenMAX ##

[OpenMAX][OpenMAX] is a unified abstraction layer that allows access to hardware that otherwise requires vendor specific APIs.

In the case of the Raspberry Pi that would be Broadcom’s [MMAL][MMAL] (Multi-Media Abstraction Layer API).

OpenMAX therefore allows a (sort of) portable implementation of software that utilizes such hardware. In this and following posts I will give an overview over the Raspberry Pi’s hardware media capabilities and how to access it using OpenMAX. The official [OpenMAX IL 1.1.2][OpenMAX-IL] documentation and the [IL Component][IL-Component] documentation provided by the Raspberry Pi Foundation form the basis for this posts. (Khronos declared OpenMAX as an inactive standard. Whatever that means.)


### How does OpenMAX work ###
OpenMAX provides an abstraction over hardware that is capable to perform operations with multi media (audio, images and video). Just like OpenGL (and now Vulkan - all of which have been specified by the same organization Khronos) which abstracts over hardware that is specialized for realtime 3D rendering like games. (Fun fact: Vulkan is a misspelled version of Vulcan, the home planet of Mr. Spock from Star Trek, just like Khronos which sounds to Qo’nos, the home planet of the Klingon empire). Hardware can only be in one state at a time so it resembles a state machine. OpenMAX maps to that state machine and provides an API to manipulate it. Because of its tight binding to hardware many commands can only be issued if the corresponding hardware is in a specific state.

Now this is from my observations on the Raspberry Pi and I don't know if they apply to other OpenMAX devices. So to be more precise. There are commands that raise an error when issued in the wrong state. Others don't raise an error but also don't perform the the action until the state requirement is met. This leads to the effect that (at least on the Raspberry Pi) some issued commands call the corresponding event handler (almost) instantaneously while others will wait until you have issued some more commands first. So there is a golden path the commands should be issued to overcome the asynchronous nature of the OpenMAX API. If you leave that path you have to deal with synchronization and wait for the callback handler to be called before issuing the next command.


### How to use OpenMAX ###
Every hardware capability resembles a _component_ in OpenMAX. You can get an overview over all the components provided by Broadcom in the [git repository][IL-Component] of the Raspberry Pi Foundation. Components can be seen as mathematical functions that has an input(s) and/or output(s). Also each component can be assigned to one of four domains: Audio, Image, Video and Other. The simple case is one (1) input port and one (1) output port. You can pass data in and processed data comes out. That can be for instance a video encoder. You pass the raw image stream to the input port and get an h.264 encoded stream on the output. You can also link the output port of one component to the input port of another component. That so called _tunneling_ allows you to chain components and only deal with one input port and one output port without having to deal with the intermediate results. So OpenMAX allows you to form a pipe-line and configure many aspects, so that you can stream data through it.

In the [VMCS-X OpenMAX IL Components][IL-Component] overview the input ports and the output ports have to be in the same domain (indicated by the color of the ports). This page shows all components available in Rasbian and not all available in VideoCore IV, as not all can be used as they would require VC to run as the main SoC with access to the file system like [image_read][noImageRead]. The documentation is still present but not linked in the index.html. All ports have some parameters to be configured. Those with a link point to parameters that are specific to Broadcoms VideoCore IV.

Lets say you have a webcam and you want to provide a multi-part JPEG image (also called MJPEG) on you website. Some webcams can do this automatically but some don't so in the latter case you would have to fetch the image and run an JPEG encode over and over again. That is a CPU heavy task which is repetitive as you usually use the same size and compression for each image. With OpenMAX you can set up a JPEG encoder component that does that for you and then the CPU only has to shovel raw image data in and pull encoded JPEG images out.



## Do some code ##

While I will keep the code on this post to a minimum an talk at a higher level. All the real vanilla C code can be found on my git repository [OMXPlayground][OMXPlayground]. Currently the implementations are not ideal as they have several busy loops. But the OpenMAX standard states that the calls must return in a specified ammount of milliseconds. The callbacks for instance are calls that block the component and therefore the client (this code) is supposed to return from the callback within 5ms (3.1.2.9.1 - 3.1.2.9.3). Calls to OpenMAX not processing any buffer are required to return within 20ms and 5ms otherwise (3.2). (Does that apply for the events?)


### image_encode ###

| input port | component name | output port |
|:----------:|:--------------:|:-----------:|
|     340    |  image_encode  |     341     |

This is one of the simpler cases as you will see later. It takes raw images in various formats ranging form 16/24/32-bit RGB with Alpha formats to variants of YUV and CrCbY. The output can be any of the supported compressed image formats listed below.

This component is fairly easy as you have total control over the input and the output formats and it supports a wide range of formats. So you basically only have to configure the input and output port and you are ready to go and throw data on it.

The basic steps:

* Get handle with callbacks for EmptyBufferDone and FillBufferDone.
* Disable ports 340 and 341.
* Switch handle to idle.
* Configure, enable and allocate bufferX for port 340.
* Configure, enable and allocate bufferY for port 341.
* Switch handle to executing.
* Loop over the data:
    * Copy compressed data from bufferY.
    * Fill bufferY.
    * Copy your image data to bufferX.
    * Empty bufferX.
* Switch handle to idle.
* Disable and deallocate bufferX for port 340.
* Disable and deallocate bufferY for port 341.
* Switch handle to loaded.
* Free handle.

The callbacks for `EmptyBufferDone` and `FillBufferDone` are required to drive the loop over your data. You cannot pass data into the component, if `OMX_EmptyThisBuffer` has not finished processing it. Because this call is not blocking one has to wait for the `EmptyBufferDone` callback. Similar for the `OMX_FillThisBuffer` call. You have wait for the `FillBufferDone` callback before you can get the resulting data from the component.

When setting up the output port, you can provide the desired color format if the selected data format supports that. Also you can set the compression ratio or quantization factor (Q-Factor) for the JPEG data format. For JPEG you also can define to use the IJG quantization tables from libjpeg (which version? - Does it have any significant quality or preformance implications?).

Ports are configured by setting their `OMX_IndexParamPortDefinition`. `nBufferSize` will change according to `format.image.nFrameWidth`, `format.image.nSliceHeight` and `format.image.eColorFormat` (the number of bytes per pixel). So after setting `OMX_IndexParamPortDefinition` you should query it to get the updated `nBufferSize`. Also most (all?) image components support image processing either in slices of 16px or the entire image at once.

|        Input color formats         |
|------------------------------------|
| OMX_COLOR_Format16bitRGB565        |
| OMX_COLOR_Format24bitBGR888        |
| OMX_COLOR_Format24bitRGB888        |
| OMX_COLOR_Format32bitABGR8888      |
| OMX_COLOR_Format32bitARGB8888      |
| OMX_COLOR_FormatYUV420PackedPlanar |
| OMX_COLOR_FormatYUV422PackedPlanar |
| OMX_COLOR_FormatYCbYCr             |
| OMX_COLOR_FormatYCrYCb             |
| OMX_COLOR_FormatCrYCbY             |
| OMX_COLOR_FormatCbYCrY             |

| Output image formats |        color formats        |
|----------------------|-----------------------------|
| OMX_IMAGE_CodingGIF  | OMX_COLOR_Format8bitPalette |
| OMX_IMAGE_CodingBMP  |              ?              |
| OMX_IMAGE_CodingJPEG |              ?              |
| OMX_IMAGE_CodingPNG  |              ?              |
| OMX_IMAGE_CodingPPM  |              ?              |
| OMX_IMAGE_CodingTGA  |              ?              |



### image_decode ###

| input port | component name | output port |
|:----------:|:--------------:|:-----------:|
|     320    |  image_decode  |     321     |

This component decodes images from various compressed data formats and returns a decompressed image. You have to tell the component which decoder to use and the output color format depends on that. There is no internal conversion so if you decode JPEG, you won't get RGB image data as the color format native to JPEG is YUV (420?).

If you want to change the output color format you have to run the resulting data through the `image_resize` component. You can establish a pipe between those two components so only have to deal with one input and one output and have not deal with the intermediate data. But this is more advanced as you will see later.

As this component does not provide conversions of the output color format you will have to deal with the image data as it comes out and thus cannot configure the output port. The output port will be configured automaticlaly accroding to the input. So you will have to pass in data into the component and let it change the output accordingly.

The basic steps:

<!-- ???? set `OMX_IndexParamImagePortFormat` get `OMX_IndexParamPortDefinition` -->

* Get handle with callbacks for EmptyBufferDone and FillBufferDone.
* Disable ports 320 and 321.
* Switch handle to idle.
* Configure, enable and allocate bufferX[3] for port 320.
* Configure port 321 for auto detect.
* Switch handle to executing.
* Copy your image data to bufferX[i = 0].
* Empty bufferX[i].
* Wait for `OMX_EventPortSettingsChanged`
* Enable and allocate bufferY for port 321.
* Loop over the data:
    * Copy decoded image data from bufferY
    * Fill bufferY
    * Copy your compressed image data to bufferX[i = (i + 1) % 3]
    * Empty bufferX[i]
* Switch handle to idle.
* Disable and deallocate bufferX[3] for port 340.
* Disable and deallocate bufferY for port 341.
* Switch handle to loaded.
* Free handle.

As you see the execution starts before the output is configured. The data passed into the input is not lost but buffered in the component until the output is completely set up. That is one of the reasons why you have to wait before passing in new data. As the output port is ready, it will call the `FillBufferDone` callback and let you grab the decompressed image data.

Supported input formats:
?????????

Supported output formats:
????????




### image_resize ###

| input port | component name | output port |
|:----------:|:--------------:|:-----------:|
|     60     |  image_resize  |     61      |

This component allows for cropping and resizing images and also changing the color format. You can configure a cropping on the input and specify an arbitrary size for the output. This component is as straight forward as the `image_encode` component as you have full control over both the input and the output port and can set them independently. One peculiarity in the implementation of Broadcom is that it only supports to process the input in either 16px slices or as whole.

The basic steps:

* Get handle with callbacks for EmptyBufferDone and FillBufferDone.
* Disable ports 60 and 61.
* Switch handle to idle.
* Configure including crop, enable and allocate bufferX for port 60.
* Configure, enable and allocate bufferY for port 61.
* Switch handle to executing.
* Loop over the data:
    * Copy transformed image data from bufferY.
    * Fill bufferY.
    * Copy your image data to bufferX.
    * Empty bufferX.
* Switch handle to idle.
* Disable and deallocate bufferX for port 60.
* Disable and deallocate bufferY for port 61.
* Switch handle to loaded.
* Free handle.

Except for the cropping the steps are identical to those for `image_encode`. Also the output resizing can be either defined by setting the width and height in `OMX_IndexParamPortDefinition` or by setting it via `OMX_IndexParamResize`.


Supported input formats:
?????????

Supported output formats:
????????






### Some basics ###
* Ports can only be configured if disabled.
* Buffers can only be allocated on enabled ports.
* Handle can only get to executing state if at least the input port is ready (configured, enabled and allocated).
* If you make a call to `OMX_SetParameter` don't use the data you passed in subsequent calls and calculations. The data will most likely change so call `OMX_GetParameter` before proceeding.




[OpenMAX]: https://www.khronos.org/openmax/
[MMAL]: https://github.com/raspberrypi/userland/blob/master/interface/mmal/mmal.h
[OpenMAX-IL]: https://www.khronos.org/registry/OpenMAX-IL/
[IL-Component]: https://github.com/raspberrypi/firmware/tree/master/documentation/ilcomponents
[OMXPlayground]: https://github.com/kwasmich/OMXPlayground
[noImageRead]: https://www.raspberrypi.org/forums/viewtopic.php?f=70&t=85992&sid=add8b3890836aa156a2a2d793eadde74



