set OLDINC=%INCLUDE%
set INCLUDE=%INCLUDE%;..\..\..\h;..\..\..\demo\utils;..\..\dp;..\..\tca;..\..\unix\server;..\..\score
cl /W3 /Zi /D_DEBUG /DDPRNT /DWIN32 /D_WINDOWS /DDYNALINK /Ddp_ANET2 /Ddpscoret_CLIENT_ONLY /Ddpscoret_DPOPEN /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" dpscoret.c ..\..\score\scorerep.c ..\..\dp\assoctab.c ..\..\dp\dynatab.c ..\..\..\demo\utils\rawwin.c ..\..\..\demo\utils\eclock.c ..\..\..\win\lib\anet2d.lib /MDd version.lib advapi32.lib user32.lib /Fedpscort.exe
set INCLUDE=%OLDINC%

