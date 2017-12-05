#
# GPL 2
# Copyright 2017 (c) TiCaN <tican@protonmail.com> github.com/TiberiusCN
#

Release=0.002

#win=i686-w64-mingw32-
CC=
FMT=
LFLAGS=
CFLAGS=
#WIN = -lgdi32 -lopengl
GLFLAGS=-lX11 -lGL
DEFL=
DEFC=-O2 -Wall -mfpmath=sse -msse2 -Wextra -pedantic

#call
all: iso2pvf ogllaf vmcut20p secultor
	

iso2pvf: bin/iso2pvf$(FMT)
	

ogllaf: bin/ogllaf$(FMT)
	

vmcut20p: bin/vmcut20p$(FMT)
	

secultor: bin/secultor$(FMT)
	

#prog
bin/iso2pvf$(FMT): bin/iso2pvf.o
	$(CC)gcc bin/iso2pvf.o $(LFLAGS) $(DEFL) -o bin/iso2pvf$(FMT)
	
bin/ogllaf$(FMT): bin/ogllaf.o bin/glworks.o bin/lineut.o
	$(CC)gcc bin/ogllaf.o bin/glworks.o bin/lineut.o -lm $(LFLAGS) $(DEFL) $(GLFLAGS) -o bin/ogllaf$(FMT)

bin/vmcut20p$(FMT): bin/vmcut20p.o bin/lineut.o
	$(CC)gcc bin/vmcut20p.o bin/lineut.o -lm $(LFLAGS) $(DEFL) -o bin/vmcut20p$(FMT)

bin/secultor$(FMT): bin/secultor.o
	$(CC)gcc bin/secultor.o $(DEFL) $(LFLAGS) `$(CC)pkg-config glib-2.0 gtk+-2.0 --libs` -o bin/secultor$(FMT)

#obj
bin/lineut.o: lineut.c lineut.h
	$(CC)gcc -c lineut.c $(DEFC) $(CFLAGS) -o bin/lineut.o

bin/glworks.o: glworks.h glworks.c
	$(CC)gcc -c glworks.c $(DEFC) $(CFLAGS) -o bin/glworks.o

bin/iso2pvf.o: iso2pvf.h iso2pvf.c
	$(CC)gcc -c iso2pvf.c $(DEFC) $(CFLAGS) -o bin/iso2pvf.o

bin/vmcut20p.o: ogllaf.h pvf.h vmcut20p.c vmcut20p.h
	$(CC)gcc -c vmcut20p.c $(DEFC) $(CFLAGS) -o bin/vmcut20p.o

bin/secultor.o: secultor.h secultor.c
	$(CC)gcc -c secultor.c $(DEFC) $(CFLAGS) `$(CC)pkg-config glib-2.0 gtk+-2.0 --cflags` -o bin/secultor.o

bin/ogllaf.o: ogllaf.h ogllaf.c
	$(CC)gcc -c ogllaf.c $(DEFC) $(CFLAGS) -o bin/ogllaf.o

#util
clean:
	rm -f ./bin/*.o
	rm -f ./bin/*$(FMT)

bkp:
	7za a "../bkp/secultor_v$(Release).zip" ./*.c *.h Makefile change.log LICENSE

cyginstall: all
	cp bin/*.exe /bin/
