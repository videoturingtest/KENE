DHOME		= ./

DBIN		= $(DHOME)bin/
DLIB		= $(DHOME)lib/
DSRC		= $(DHOME)src/
DOBJ		= $(DHOME)obj/

KENE		= $(DBIN)kene

CXX			= g++
CFLAGS		= -g -c -Wall -O5 -m64
LFLAGS      = -m64 -O5 -L$(DLIB)pcre -L$(DLIB) -L.

OBJS_KENE	= \
	$(DOBJ)kene.o \
	$(DOBJ)kene-sys.o \
	$(DOBJ)kene-tool.o

all : $(KENE)

clean :
	rm -f $(OBJS_KENE)

$(DOBJ)kene.o : $(DSRC)kene.cpp \
	$(DSRC)kene-sys.h $(DSRC)kene-tool.h $(DSRC)kt-lib.h
	$(CXX) $(CFLAGS) -o $@ $<

$(DOBJ)kene-sys.o : $(DSRC)kene-sys.cpp \
	$(DSRC)kene-sys.h $(DSRC)kene-tool.h $(DSRC)kt-lib.h
	$(CXX) $(CFLAGS) -o $@ $<

$(DOBJ)kene-tool.o : $(DSRC)kene-tool.cpp \
	$(DSRC)kene-tool.h $(DSRC)kt-lib.h
	$(CXX) $(CFLAGS) -o $@ $<


$(KENE) : $(OBJS_KENE)
	$(CXX) $(LFLAGS) -o $@ $^ -lmodkept -lmodkpat -lmodkema -lpcre

