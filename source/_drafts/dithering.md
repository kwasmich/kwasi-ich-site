---
title: Enhancing image quality using dithering
author: Michael Kwaśnicki
date: 2011-12-18
tags: [Image, Quality]
photos:
    - image.png
    - /images/dummy2.png
---

Dithering is often used to reduce data size from higher depth resolution to lower. But it can be used the other way around and emulate a higher depth resolution than there is.

<!-- more -->

Most people see dithering as a method to improve visual quality of images when reducing color-depth. But this isn't actually the intention of dithering. One should see it the other way around. It is intended to simulate higher color-depth in a color-depth limited environment. This can have different reasons.

The currently most obvious and widely distributed field of application is the use of e-paper like in the Amazon Kindle. It has a monochrome display with only black and white. Dithering can be used here to simulate different shades of gray. The display used in the Kindle has still space for improvements and would benefit of higher retina-like resolutions allowing for smoother gray shades. If it could reach 600dpi then it will gain quality of laser printed documents.

The next obvious field is the use in real time 3D graphics on memory bandwidth limited devices like the iPhone. While the device is capable to display what everybody calls "true-color", it does not really need to. Using 16-bits per pixel instead of 32-bits may be a question of doubling the frame rate or not. So the use of dithering here is to simulate 32-bit color depth with only 16-bit color in the resulting image. As pixels get smaller like on the retina display in the iPhone 4, one won't even be able to tell whether there is dithering or not as the human eye cannot distinguish pixels and therefore sees the averaged color of the dithering pattern. The visual result is higher than those 16-bit per pixel.

The resolution is also a key factor on desktop computers. Dithering and 16-bit per pixel has been available in games for decades. But the screen resolution was too small compared to the screen size so the artifacts of dithering have been pretty obvious. This might change as there have been rumors of retina displays for desktop computers.

Now as we talk about games. I wan't to mention my second reason why I'm writing all this. I was playing RAGE, the latest piece of id Software. It was fun but at some point I thought they could have done better than that. Any modern game uses compressed textures that reduce the memory bandwidth and foot print to offer even higher resolution assets. The most commonly used compression is called S3TC on OpenGL and DXTC on Direct-X which is the same and was invented by S3 but Microsoft likes its naming everywhere. When compressing a texture the image is being cut into chunks of 4x4 pixels. Then two 16-bit colors are determined such that all or most or at least few of the 16 pixels can be constructed as linear composition of those two base colors with as few error as possible. The next limiting factor is that there can be only one or two intermediate colors from those two 16-bit base colors. This is because the other pixels are encoded in a 2-bit per pixel matrix allowing only one of four possible states. Without getting too deep into the matter the point is that those 4x4 chunks with the very limited color palette are ideal to use dithering. Ordered dithering is the best choice as 4x4 pixels are too small to use other techniques like error diffusion and it does not make sense to propagate the error to the next chunk as it probably has a completely different color palette and would only disturb the image. If id Software had used ordered dithering when implementing their Mega-Texture technology, then I might have been spared of such a view.

[Screenshot of RAGE]

Let's get to the main reason for this article. While almost all the above is about 16-bit per pixel I want to point out that in some cases even 32-bit per pixel are not enough. So I'm discussing here to use dithering to simulate colors with a much higher depth. It is all about the dark adaption of the human eye and its sensitivity to discontinuities. A grayscale image like the gradient below consists of black, white and 254 intermediate gray shaded which are linearly distributed. When viewing the image in whole, then there is no problem visible. Also when limiting ourselves to the 32 brightest gray values and scale them up to fill the same area most people won't see any discontinuities. This is because The eye adapts to the brightest point and the image ranges in brightness values from 223 to 255. So seen to the absolute brightness the steps between the previous and the next gray makes a relative step size of 1/255 = 0.003921568627. Those steps are pretty small and barely visible. The opposite happens when we limit ourselves to the 32 darkest gray values and fade away all surrounding brighter pixels. Then the human eye adapts to the new brightest point and the relative step size gets 1/31 =  
0,03225806452 which is much larger. One can see the steps between the gray values. Actually the human eye even amplifies those steps to make those discontinuities in the brightness more visible which appears as a gradient from one step to the next in the opposite direction as the gradient of the image.

[Bilder]

That is something I found very frustrating when I was creating the gradient for the background of this page. Now to overcome this one has to create the gradient using a higher color-depth than true color or for my case higher than 8-bit grayscale and then reduce the color the bit-depth a display is able to show. An even better approach would be to use floating-point in content creation as it has a higher resolution around 0 and therefore in darker areas. But most graphic software doesn't support that. If you magnify the portion of the screen and look at the background gradient you will see the dithering pattern clearly. The pattern needs to have a specific size to be noticed by the human eye otherwise it sees the averaged brightness or color of the dithered values. This is also why dithering only makes sense at native screen resolutions and will improve with retina displays.



## Header 2 (separate different sections; keep three spaces to top) ##


### Header 3 (; keep two spaces to top) ###


Separating paragraphs. Paragraph 1.

Paragraph 2.


[Link](http://example.com/)


** Strong **


_ Emphasized _ 


~~Strike through~~ (must not have a gap!)


Unordered list:
* Item 1
* Item 2

    with second paragraph
* Item 3
        with code block
* Item 4
* Item 5


Inline code: `printf("%d\n", 42)`


Code block (without any syntax):
    #include <stdio.h>
    #include <string.h>


Code syntax block:
``` objc title http://kwasi-ich.de kwasi-ich
[rectangle setX: 10 y: 10 width: 20 height: 20];
```
supports bahs, objc, js, css and so on


Image: ![img](image.png)

Image: ![img](/images/dummy2.png)

Image: {% asset_img image.png %}

Image: {% img /images/dummy2.png %}


Asset path: {% asset_path image.png %}
