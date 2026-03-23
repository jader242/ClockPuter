Just a simple clock for the M5Stack Cardputer. Takes in user input each boot for network SSID, password, and UTC time offset. Also displays the current battery percentage in pretty colors depending on the level. Still a work in progress, probably need to fix some things here and there

3/13/26 commit: did a big ol refactor, no longer am using NTPClient and instead am using the built in ESP Time library. Changed all strings to character arrays. Added a date function that can be viewed by scrolling on the M5Stack Unit Scroll (SKU: U186). Plus a few other things. Plan on making the date viewable with the arrows on the cardputer keyboard soon, but baby steps ya know lol

DISCLAIMER: This is not currently usable without the M5Stack Unit Scroll plugged in, it just switches back and forth between both screens crazy fast. 
