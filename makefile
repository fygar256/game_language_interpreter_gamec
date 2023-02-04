.RECIPEPREFIX +=

# for debian package
FAKEROOT = /usr/bin/fakeroot
VERSION = 0.9.1-1
TARGET  = gamelinux
INSTDIR = /usr/bin/
DEBFILE = $(TARGET)_$(VERSION)_amd64.deb
WORKDIR = work
CONTROL = $(WORKDIR)/DEBIAN/control
TARGDIR = $(WORKDIR)/$(shell echo $(INSTDIR)|sed -e 's/\///')
PACKAGE = Package: $(TARGET)
MAINTAIN= Maintainer: Taisuke Maekawa \<fygar256@gmail.com\>
ARCH    = Architecture: amd64
VER     = Version: $(VERSION)
DESC    = Description: GAME language for linux
DEPENDS = Depends:



# bin
CC      = /usr/bin/cc 
RM      = /bin/rm
CP      = /bin/cp

# file description
SRCS    = gamelinux.c
OBJS    = $(SRCS:.c=.o)

# for compiler
INCDIR  =
LIBDIR  = 
LIBS    = 
CFLAGS  =

$(TARGET): $(OBJS)
   $(CC) $(CFLAGS) $^ -o $@ $(LIBDIR) $(LIBS)

$(OBJS): $(SRCS)
   $(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)

deb: $(TARGET)
     mkdir -p $(WORKDIR)/DEBIAN/ -p $(TARGDIR)
     echo $(PACKAGE) >$(CONTROL)
     echo $(MAINTAIN) >>$(CONTROL)
     echo $(ARCH) >>$(CONTROL)
     echo $(VER) >>$(CONTROL)
     echo $(DESC) >>$(CONTROL)
     echo $(DEPENDS) >>$(CONTROL)
     echo >>$(CONTROL)
    $(CP) $(TARGET) $(TARGDIR)
    $(FAKEROOT) dpkg-deb --build $(WORKDIR) .

clean:
   $(RM) -rf $(TARGET) $(OBJS) $(DEBFILE)

delobj:
   $(RM) -rf $(OBJS)

install: $(TARGET) deb
    sudo $(CP) $(TARGET) $(INSTDIR)
    sudo dpkg -i $(DEBFILE) 

all: clean $(OBJS) $(TARGET) deb install
