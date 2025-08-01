# anet: a networking library for peer-to-peer games #
September 4, 2001<br>
Dan Kegel<br>
dank-anet@kegel.com<br>
http://www.kegel.com/anet

This is the source code to the library used by some multiplayer games 
released by Activision and others between 1995 and 2000.  
It is copyright 1995-2001 by Activision, and is now released under the [LGPL](LICENSE).
It is hoped that this will enable people still playing those games to set 
up and run their own game servers if desired.  
Also, the library may still be of some interest to game programmers looking for
a peer-to-peer game networking library to use or study.
Finally, some of the techniques used in the library (e.g. the NAT compatibility 
scheme, the crash logger, the reliable transport built on top of UDP, 
the strlkup internationalization code, etc.)
may also be of some interest to other programmers.  Portions of the library
(e.g. the score reporting system) were never really completed.  Other
portions (e.g. the automatic patch downloader) are not appropriate for use
with untrusted servers, so are no longer fully documented or supported.

The library currently builds on Linux and Windows 95 and higher.  It has been
compiled under MS-DOS and MacOS, but that has not been tested recently.  A
partial Java binding is also provided.  The library is byte-order-safe,
and interoperates properly between big- and little-endian machines.

For a packing list and links to documentation, see index.html.

## Building on Windows with nmake

To use nmake go to the Developer Command Prompt from your Visual Studio installation and then go to ..\anet\src and run there build.bat. The output files will be in ..\anet\win\.

## Building the Linux server and tools ##

Go to anet/src/linux/ and run ./build there.

## Dokumentation ##

Check out the html [documentation](./doc/index.html).