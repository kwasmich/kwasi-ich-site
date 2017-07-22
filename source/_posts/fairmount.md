---
title: Fairmount, VideoLAN Client and the Bagels
author: Markus Kwaśnicki
date: 2013-04-21
tags: [macOS, DVD, CSS]
photos: bagel.png
---

This article gives you a hint about how to get Fairmount to work in these days as it's not supported anymore.

<!-- more -->


### Introduction ###

Many Mac users appreciate to use the [Fairmount](http://www.metakine.com/products/fairmount/releasenotes.html) software to bypass the copy protection of a DVD. It's simple usage is one of the benefits of Fairmount. Avoiding the copy protection of a DVD using Fairmount is as simple as inserting a DVD into your DVD drive. Once inserted, Fairmount mounts the inserted DVD fairly ;-) Anyway to work properly Fairmount needs help from a library provided by the well-known and popular [VideoLAN Client](http://www.videolan.org/vlc/) media player. This teamwork between Fairmount and VLC worked for years.


### An old bagel ###

Unfortunately Fairmount has been acquired by another software company and has been merged into a new commercial software product. Metakine has stopped maintaining Fairmount, so there will be no new free releases. You are lucky if you download yourself a copy of the latest version of Fairmount 1.1 as long it is available as part of [DVDRemaster](http://www.metakine.com/products/dvdremaster/) v6.3.1 through several download services (e.g. [CNET](http://download.cnet.com/FairMount/3000-2139_4-97114.html)).

But there is still a problem: The last version of Fairmount does not work together with the latest version of VLC because the access for the required [libdvdcss](http://www.videolan.org/developers/libdvdcss.html) library by Fairmount now got restricted in VLC to external programs. And Fairmount keeps prompting for the missing VLC library (see article picture).


### Solution ###

Anyway there is a solution which helps you further using Fairmount on Mac OS X (Snow Leopard, Lion, Mountain Lion) because the libdvdcss library is still being maintained as a standalone project:

* ~~Download the latest version of the libdvdcss library from [its public releases](http://download.videolan.org/pub/libdvdcss/last/macosx/libdvdcss.2.dylib).~~
* Download the 32-Bit version 1.1.9 of the VideoLAN Client from [their public FTP archive](http://download.videolan.org/pub/videolan/vlc/1.1.9/macosx/vlc-1.1.9-intel.dmg) (the 64-Bit version won't work).
* ~~Copy the downloaded library to the application support folder of Fairmount~(create it if it does not exist): `~/Library/Application Support/Fairmount/` — Note that ~ points to your user's home directory.~~
* Mount the image and copy the **libdvdcss.2.dylib** file from `VLC.app/Contents/MacOS/lib/` to the application support folder of Fairmount (create it if it does not exist already): `~/Library/Application Support/Fairmount/` — Note that ~ points to your user's home directory.
* Rename the library from **libdvdcss.2.dylib** to **libdvdcss.dylib** because Fairmount is looking for a file with exact that name.

If all went well, Fairmount should no longer come up with the missing VLC dialogue. — That's all folks!