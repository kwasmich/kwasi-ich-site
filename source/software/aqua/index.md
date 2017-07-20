---
layout: page
title: Aqua for everyone
author: Michael Kwaśnicki
date: 2009-09-10 00:00:00
photos: aqua.png
---

The Aqua user interface of Mac OS X has seen many imitations but none of them do it right. On many internet sites webmaster try to create user interfaces that try to look like Mac OS X or many Linux users install themes that barely remind on Mac OS X.  For all those who are interested in the Mac OS X user interface I've written a program which takes apart the complete Aqua user interface and delivers you all its graphics files. With those one can create user interfaces for the web like shown below.



## What you need ##

* [ResCafé](http://gbsmith.freeshell.org/ResCafe/)
* [Mac OS X](http://www.apple.com/macosx/) (10.4 or higher — possibly works also with prior versions)
* my [pxm2tga](http://support.kwasi-ich.de/pxm2tga.zip) command line utility (10,7 KiB)



## How it is done ##

* At first you need to compile the command line tool pxm2tga yourself. Therefore you need to install the Xcode developer tools that ship with every copy of Mac OS X.
* After unzipping ResCafé you will notice that this is a Java-Application which cannot be run by double clicking it. To run it you have to utilise Terminal and go into the ResCafé folder. Therefore launch Terminal, type
    
        cd
    
    with a white space and dragging the unzipped ResCafé folder onto the Terminal window. Hit return and you are there. Now type

        ./ResCafe.sh

    , hit return and the application is running.
* Click on the **File** menu within the application window and select **Open**. In that dialog open the file

        /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/Resources/Extras.rsrc

* On the left hand side you see some squares with question marks in it. Select **pmx#** and a bunch of files appears on the right hand side. (Don't click on anything!)
* From the menu **File** select now **Save Current…** and in that file dialog give the folder to be created a significant name. After saving it you will find inside a folder named pxm# containing hundreds of files with the suffix raw.
* Now it is show time for the command line utility pxm2tga. Therefore again open terminal and change into the pxm# folder using the

        cd

    command. By executing the following command line all raw files will be converted into TGA images.

        find . -iname "*.raw" -exec /path/to/pxm2tga {} {}.tga \;

    Of course you need to adapt the path to pxm2tga. The easiest way is again performing a drag & drop operation with the Terminal window.
* After that one can wipe all the raw files by executing

        rm *.raw

    so the folder only contains images.

Now you have got all the graphical elements of the Aqua user interface as TGA images. Have fun with that.
