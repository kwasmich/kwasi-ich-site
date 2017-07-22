---
title: {{ title }}
author: xxx Kwaśnicki
date: {{ date }}
tags: [new, post]
photos: image.png
---

Teaser…

<!-- more -->

Content…



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


Asset image: {% asset_img image.png %}


Asset path: {% asset_path image.png %}

