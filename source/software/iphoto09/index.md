---
layout: page
title: Scripting Faces in iPhoto '09
author: Michael Kwaśnicki
date: 2009-06-05 00:00:00
photos: faces.jpeg
---

Apple's iPhoto '09 is a great tool to organize photos. With its new features faces and places it has even more intuitive approaches for organizing. But unfortunately it lacks scripting capability. You can not get the names out of a photo or the location. Starting in iPhoto '06 you also can not script events.

There are many good reasons for scripting photos. For instance one may want to get all the names of people on a photo as a list. Or maybe someone has a private homepage and wants to tag all the photos with the names and locations. So that is why I investigated into this and found a solution that works. But I have to admit that the performance is not as great (as for almost every complex AppleScript).

This AppleScript directly accesses the SQLite data-base of the iPhoto Library in a **read-only** way to get the information which is otherwise not accessible. If your iPhoto Library is not in the default place you will have to correct the paths at the top of the script.

Download: [iPhotoFacesPlacesEvents.applescript.gz](http://support.kwasi-ich.de/iPhotoFacesPlacesEvents.applescript.gz) (1.9 KiB — 2009-06-05)

**The functions in this script are as follows:**

<dl>
	<dt>{record, …} getFaces( photo )</dt>
	<dd>
		Input is an iPhoto photo-object. 
		<br />
		Output is a list of names in the form of records { faceName:string, fullName:string, eMail:string }.
	</dd>
	<dt>integer getEventFromPhoto( photo )</dt>
	<dd>
		Input is an iPhoto photo-object.
		<br />
		Returns the id of the event as an integer (this is not an object from iPhoto).
	</dd>
	<dt>{photo, …} getPhotosFromEvent( integer )</dt>
	<dd>
		Input is an event id integer (this is not an object from iPhoto).
		<br />
		Output is a list of photos in this event as iPhoto photo-objects.
	</dd>
	<dt>record getInfoForEvent( integer )</dt>
	<dd>
		Input is an event id integer (this is not an object from iPhoto).
		<br />
		Output is the key-photo, name and comment of that event as record { keyPhoto:photo, eventName:string, comment:string }.
	</dd>
	<dt>record getGPSForPhoto( photo )</dt>
	<dd>
		Input is an iPhoto photo-object.
		<br />
		Output is the GPS latitude and longitude as record { latitude:real, longitude:real }.
		<br />
		_In fact this function is obsolete since iPhoto can script GPS data. But it has been added for consist retrieving with the event's GPS information._
	</dd>
	<dt>record getGPSForEvent( integer )</dt>
	<dd>
		Input is an event id integer (this is not an object from iPhoto).
		<br />
		Output is the GPS latitude and longitude as record { latitude:real, longitude:real }.
	</dd>
	<dt>record getCustomPlaceForPhoto( photo )</dt>
	<dd>
		Input is an iPhoto photo-object.
		<br />
		Output is the GPS latitude and longitude as well as the name and description of a custom place which is associated with that photo as record { latitude:real, longitude:real, placeName:string, placeDescription:string }.
	</dd>
	<dt>record getGPSForEvent( integer )</dt>
	<dd>
		Input is an event id integer (this is not an object from iPhoto).
		<br />
		Output is the GPS latitude and longitude as well as the name and description of a custom place which is associated with that event as record { latitude:real, longitude:real, placeName:string, placeDescription:string }.
	</dd>
	<dt>record getLocationForPhoto( photo )</dt>
	<dd>
		Input is an iPhoto photo-object.
		<br />
		Output is the name of locations which describe the location of the photo's place including ocean, country, province, county, city, neighbourhood, area of interest and point of interest as record { ocean:string, country:string, province:string, county:string, city:string, neighborhood:string, aoi:string, poi:string }.
		<br />
		_Since not all location names are localized, this function extracts the German names and supplements them with the English names. To get the name of the location in another language just replace "de" in the mainLanguage property with the appropriate language. Languages that are known to be supported by iPhoto '09 are: da, de, en, es, fi, fr, it, ja, ko, nl, no, pl, pt, pt-br, ru, sv, zh, zh-tw._
	</dd>
	<dt>record getLocationForEvent( integer )</dt>
	<dd>
		Input is an event id integer (this is not an object from iPhoto).
		<br />
		Output is the name of locations which describe the location of the events's place including ocean, country, province, county, city, neighbourhood, area of interest and point of interest as record { ocean:string, country:string, province:string, county:string, city:string, neighborhood:string, aoi:string, poi:string }.
		<br />
		_Since not all location names are localized, this function extracts the German names and supplements them with the English names. To get the name of the location in another language just replace "de" in the mainLanguage property with the appropriate language. Languages that are known to be supported by iPhoto '09 are: da, de, en, es, fi, fr, it, ja, ko, nl, no, pl, pt, pt-br, ru, sv, zh, zh-tw._
	</dd>
</dl>

Since not every location is in the middle of an ocean or not every photo has GPS data or a custom location, those elements will have a _missing value_.
