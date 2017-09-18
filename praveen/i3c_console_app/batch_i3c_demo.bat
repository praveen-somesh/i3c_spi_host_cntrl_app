del *.exe
echo off
echo ----------------------------------------
echo sample application to build and run

echo ----------------------------------------
copy include\libMPSSE_spi.h .
copy include\device_table.h
copy include\windows\ftd2xx.h .
copy lib\windows\x64\*.* .
copy lib\windows\i386\libMPSSE.a .
echo on
windres resources.rc -o resources.o
gcc -c i3c_demo.c  -o i3c_demo.o
gcc -o i3c_demo.exe i3c_demo.o resources.o -L. -lMPSSE
dir *.exe
echo cleaning project
del *.h
del *.dll
del *.lib
del *.o
del *.a
echo on
echo Running the application ...
i3c_demo
echo off
pause