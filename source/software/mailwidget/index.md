---
layout: page
title: MailWidget
author: Michael Kwaśnicki
date: 2009-01-32 00:00:00
photos: mailwidget.png
---

This widget for the Apple Dashboard announces the arrival of new mail. It is a direct extension to Apple Mail. But Apple Mail does not have to run all the time in background. The usage is very simple and intuitive. Download it and try it yourself.



## Donate ##

MailWidget is available for Dashboard for four years now and is still being improved constantly with commitment. If MailWidget is an indispensable Widget for you and you would like to express your esteem to me, the author, then consider whether you want to support this project with a small donation. However you decide, MailWidget remains free for everyone.



## Download ##



## Information ##

Version 3 of MailWidget was completely rewritten using Dashcode in order to fulfil the demands of the users. If you experience problems with MailWidget or you want to give feedback, don't hesitate to contact me.


### NOTE! ###

**In Yosemite: if your are encountering problems verifying your account's password, then try going in to Mail's preferences > Accounts > (select your Account) > Advanced (tab) and uncheck the SSL setting and check it back to update the port number.**

**If your company or institute upgraded to Windows Server 2008 R2 you will run into trouble using Exchange unless you update curl. Microsoft changed the NTLM authentication protocol which is implemented in curl through reverse engineering. To update curl I recommend to install [MacPorts](http://www.macports.org/) and get the latest version of curl.**

**If you experience problems after updating, you need to log off and log in again or reboot your computer to make all changes effective.**

**The support for Mac OS X 10.5 Leopard has been dropped. The latest version that will support Leopard is 3.56.**

That was not an easy decision for me, especially as there is a bug in Dashcode for Snow Leopard which limits the developer. But the decision came while updating the Growl-Plugin. Growl has dropped support for PowerPC and ultimately supports only intel CPUs. There is no way to get around. So I complied with Growl and the bug in Dashcode making MailWidget now intel-only. That allows to maintain compatibility with OS X Lion and finally take advantage of newer versions of WebKit. All changes made from version 3.56 to 3.57 only affect Snow Leopard and Lion and do not change the functionality.



## Version history ##

[New in version 3.61](http://support.kwasi-ich.de/MailWidget361.zip)
* Resolved issue iCloud accounts not being shown in Mavericks and Yosemite.

[New in version 3.60](http://support.kwasi-ich.de/MailWidget360.zip)
* Resolved issue with Exchange accounts not being shown in Mountain Lion.

[New in version 3.59](http://support.kwasi-ich.de/MailWidget359.zip)
* Fixed issue with accounts not showing up in OS X 10.8 Mountain Lion
* Added support for Notification Center (which is used instead of Growl)
* Resolved issue with iCloud accounts not working properly

[New in version 3.58](http://support.kwasi-ich.de/MailWidget358.zip)
* Fixed communication problem with some IMAP servers

[New in version 3.57](http://support.kwasi-ich.de/MailWidget357.zip)
* Updated the Growl-Plugin to the latest version to maintain compatibility in OS X Lion
* Introduced ne Growl settings tab to control the new Growl Mist messages
* Using intrinsic methods of WebKit instead of custom code for IMAP accounts
* Fixed iCloud accounts not getting verified

[New in version 3.56](http://support.kwasi-ich.de/MailWidget356.zip) (if you are using Mac OS X 10.5 Leopard then you want this one)
* Fixed issue where MailWidget did not list any accounts on Lion
* Fixed issue with wrong mail count shown up for POP accounts on Lion

[New in version 3.55](http://support.kwasi-ich.de/MailWidget355.zip)
* Fixed compatibility issue with Mac OS X 10.7 Lion
* Minor changes on the Auto-Update feature
* Updated Growl component to version 1.2.2

[New in version 3.54](http://support.kwasi-ich.de/MailWidget354.zip)
* Fixed issue with MailWidget counting unread mails that have been marked as "deleted" on an IMAP server
* Fixed issue affecting mails without subject on an Exchange 2007 server
* MailWidget now checks wether Apple Mail is running at startup
* Updated Growl component to version 1.2.1
* Growl notifications now also display the count of new mails
* Improved behaviour when adding custom audio files

[New in version 3.53](http://support.kwasi-ich.de/MailWidget353.zip)
* Fixed issue with IMAP accounts where passwords containing parentheses
* Fixed issue with POP accounts where MailWidget crashes if the server unexpectedly closes the connection
* Fixed issue with MailWidget not announcing new eMails from POP accounts
* Added indicator for missing network connection
* Improved communication with Apple Mail
* Catching problems with Exchange 2007 on Windows Server 2008 R2 by using MacPorts' curl if available

[New in version 3.52](http://support.kwasi-ich.de/MailWidget352.zip)
* Fixed issue where MailWidget was ignoring the authentication settings
* MailWidget no longer opens multiple message view windows when launching Mail in Mac OS X 10.5.x
* Auto-Update checks more frequently for updates and allsows a manual check
* Switched from telnet to netcat for handling of unsecured connections
* Improved handling of empty subjects and emails with no date sent

[New in version 3.51](http://support.kwasi-ich.de/MailWidget351.zip)
* Added version number on the backside so you know the version without going to Finder
* Fixed failing checks if an eMail did not contain the date it was sent
* Long texts which are truncated with an ellipsis, have now a mouse over bubble containing the full text
* Fixed a performance issue with the communication with Mail that could lead to high CPU usage by Mail

[New in version 3.50](http://support.kwasi-ich.de/MailWidget350.zip)
* Support for Microsoft Exchange 2007 accounts
* Betters integration of Growl allows to launch Mail and receive mails by clicking on the Growl bubble
* More reliable audio feedback (avoids the QuickTime issue described above)
* Scaling MailWidget to ¼ of its original size
* Communication with Apple Mail allows to synchronise the displayed count
* Allowing to set MailWidget to check on showing Dashboard additionally to the interval setting
* Passwords are now stored in Keychain
* Countless other improvements and changes

[New in version 3.08](http://support.kwasi-ich.de/MailWidget308.zip) (if you are using Mac OS X 10.4 Tiger then you want this one)
* Changed behaviour so MailWidget does no longer check for mails after waking up when it was set to check only when Dashboard is being activated
* Fixed problem with playback of the default announcement sounds in Snow Leopard
* Fixed displaying problem where account names contained Unicode characters in the account list
* Improved reliability with IMAP servers
* Fixed compatibility to IMAP servers that do not stick tight to the IMAP protocol
* Improved reliability on fetching header information (sender and subject)

[New in version 3.07](http://support.kwasi-ich.de/MailWidget307.zip)
* Auto update now quits the Dock with a different command so the Dock does not crash on restart
* Improved reliability on fetching header information (sender and subject)
* Fixed issue where MailWidget did not verify IMAP, Exchange and MobileMe accounts which were completely empty
* MailWidget now quits all connections to mail servers when it is being removed from Dashboard
* Improved checking reliability
* Workaround an issue where no account list is available

[New in version 3.06](http://support.kwasi-ich.de/MailWidget306.zip)
* Fixed the bug, which prevented MailWidget 3.05 to initialise completely if custom audio feedback has been set up
* Reversed assumption about the line breaks a server required to improve compatibility (now CRLF by default and not LF)

[New in version 3.05](http://support.kwasi-ich.de/MailWidget305.zip)
* Fixed issue where Sender and/or Subject of a mail was truncated or undefined
* Fixed issue with no Growl notification showing up even the header information was displayed correctly on the widget
* Updated third party source code
* Added Copy & Paste functionality to the front side for mail composing with attachment and to the audio settings for customising sound files

[New in version 3.04](http://support.kwasi-ich.de/MailWidget304.zip)
* Auto Update setting is now honoured
* Unicode characters like Japanese are now displayed correctly (but not in Growl notifications on Tiger)

[New in version 3.03](http://support.kwasi-ich.de/MailWidget303.zip)
* Solved an issue with very full POP3 servers
* Changed update procedure to work around the Dashboard (WebKit) cache problem
* MailWidget now shows a changeable name on the front to distinguish multiple instances

[New in version 3.02](http://support.kwasi-ich.de/MailWidget302.zip)
* Improved compatibility to Apple Mail 2 in Tiger

[New in version 3.01](http://support.kwasi-ich.de/MailWidget301.zip)
* Solved an issue with GMail IMAP accounts

[New in version 3.00](http://support.kwasi-ich.de/MailWidget300.zip)
* Complete rewrite and port into Dashcode
* One instance of MailWidget is able to check all your accounts
* Customisable audio feedback
* Adjustable volume
* Drop files on MailWidget to send them as attachment
