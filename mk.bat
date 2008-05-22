move object >nul
if not exist import.obj wcc386 import.c /zw /d2
if not exist export.obj wcc386 export.c /zw /d2
if not exist pe.obj wcc386 pe.c /zw /d2
if not exist vb.obj wcc386 vb.c /zw /d2
if not exist config.obj wcc386 config.c /zw /d2
if not exist index.obj wcc386 index.c /zw /d2
wcl386 vbref pe.obj vb.obj import.obj export.obj config.obj index.obj c:\watcom\lib386\nt\comctl32.lib /l=win95 /zw /d2
call crc
move *.obj object >nul
