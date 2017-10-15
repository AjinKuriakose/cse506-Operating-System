CFLAGS =	-O -I. -DTCL_LIBRARY=\"${TCL_LIBRARY}\" ${NP} -O1 -std=c99 -fPIC -march=amdfam10 -g3 -Wno-deprecated-declarations -D_XOPEN_SOURCE=600
CFLAGS_FULL=-nostdinc -Iinclude -msoft-float -mno-sse -mno-red-zone -fno-builtin -fno-stack-protector

OBJS = tclBasic.o tclCkalloc.o tclGet.o tclHash.o tclParse.o tclProc.o tclUtil.o tclVar.o \
       tclGlob.o tclUnixAZ.o tclUnixStr.o tclUnixUtil.o \
			 strtod.o strtol.o strtoul.o strcmp.o strcpy.o strlen.o strncmp.o tclMalloc.o tclFree.o tcltest.o

all: libtcl.a 

libtcl.a: ${OBJS}
	rm -f libtcl.a
	ar cr libtcl.a ${OBJS}

clean:
	rm -f ${OBJS} libtcl.a

