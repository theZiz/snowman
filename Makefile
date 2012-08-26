#==PC==
CPP = gcc -march=native -g -lm -DX86CPU
SDL = `sdl-config --cflags --libs`
#==Consoles==
#==GP2X/WIZ==
#ORIGINALFW = -static  -lsmpeg -lpng -ljpeg -lm -lz  -s
#CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DF100
#SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags --libs`
#INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
#LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -lz
#==Caanoo==
#CPP = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-gcc -DMOBILE_DEVICE -DARMCPU -DCAANOO
#SDL = `/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/bin/sdl-config --cflags --libs`
#INCLUDE = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include
#LIB = -L/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib -Wl,-rpath=/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib
#==Dingoo==
#CPP = /opt/mipsel-linux-uclibc/usr/bin/mipsel-linux-gcc -DDINGOO
#SDL = `/opt/mipsel-linux-uclibc/usr/bin/sdl-config --cflags --libs`
#INCLUDE = -I/opt/mipsel-linux-uclibc/usr/include
#LIB = -L/opt/mipsel-linux-uclibc/usr/lib -Wl,-rpath=/opt/mipsel-linux-uclibc/usr/lib
#==Pandora==
#CPP = /opt/pandora/arm-2010.09/bin/arm-none-linux-gnueabi-gcc -DARMCPU -DPANDORA -g
#SDL = `/opt/pandora/arm-2010.09/usr/bin/sdl-config --cflags --libs`
#INCLUDE = -I/opt/pandora/arm-2010.09/usr/include
#LIB = -L/opt/pandora/arm-2010.09/usr/lib -Wl,-rpath=/opt/pandora/arm-2010.09/usr/lib -lpnd

all: snowman

snowman: snowman.c
	$(CPP) -O3 snowman.c ../3dengine/graphicstuff-asm.o ../3dengine/graphicstuff.o ../3dengine/3dengine.o ../3dengine/meshloader.o $(SDL) $(INCLUDE) $(LIB) -lSDL_mixer -lSDL_image -lSDL $(ORIGINALFW) -o snowman

clean:
	rm snowman
