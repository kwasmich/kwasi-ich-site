---
title: Camera RAW Compatibility Updates in Snow Leopard
author: Michael Kwaśnicki
date: 2012-12-16
tags: [macOS, RAW, Photos]
photos: raw.png
---

Many updates are tied to certain versions of software that needs to be present. But in some cases it is ridiculous because the required software is also no longer provided by Apple. Buying a new computer just to be able to use a cameras RAW files is also no option.

<!-- more -->


### What's wrong with it? ###

Apple provides a list of supported digital camera RAW formats for Mac OS X 10.6 Snow Leopard. I recently bought a Canon PowerShot S100 which is supported according to the list. After downloading the latest version of the Camera RAW Compatibility Update I was unable to install it because it requires iPhoto '11 or Aperture 3 and I own neither.

Since my computer is such old that it is not supported by Mac OS X 10.7 Lion or OS X 10.8 Mountain Lion, I cannot just go and buy iPhoto '11 from the AppStore as it requires 10.7.5. Then I thought a bit and it came to my mind that being locked out from the update makes no sense at all. Even if the Update does not work with the older iPhoto '09 which I own. One can simply open camera RAW files in Preview.

### Fixing the update ###

At first make sure your camera is supported by looking onto the compatibility list ([Mac OS X v10.6: Supported digital camera RAW formats](http://support.apple.com/kb/HT3825)). If it is not listed there then you're screwed.

Get the latest Update that officially runs on Mac OS X 10.6 Snow Leopard ([Digital Camera RAW Compatibility Update 3.14](http://support.apple.com/kb/DL1552)). If you try to install it without owning Aperture 3 or iPhoto '11 respectively, you will have no luck. Now keep the disk image mounted.

Open the Terminal and enter the following commands:

* First create a working directory inside your home directory.

        mkdir ~/raw

* Extract the contents of the installation package.

        sudo xar -xPC ~/raw -f /Volumes/Digital\ Camera\ Raw\ Compatibility\ Update\ 3.14/RAWCameraUpdate3.pkg

* Change into the working directory.

        cd ~/raw

* Change the version check from '9.0' (iPhoto '11) to '8.0' (iPhoto '09).

        sudo sed -i .bak "s/system.compareVersions(bundleKeyValue, '9.0')/system.compareVersions(bundleKeyValue, '8.0')/" Distribution

* Create a new installation package.

        xar --compression none --distribution -cvf ~/Desktop/RAWCameraUpdate3.pkg *

* Remove the working directory.
    
        cd ~
        sudo rm -rf ~/raw

Now you will find the modified update installation package on your desktop. This time the installation will work. But you will need to restart all applications that are supposed to take advantage of the new camera RAW formats even you do not need to restart the computer.
