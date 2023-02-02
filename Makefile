CC      = /usr/bin/cc 
FAKEROOT = /usr/bin/fakeroot
RM      = /bin/rm
CP      = /bin/cp
CFLAGS  =
TARGET  = gamelinux
SRCS    = gamelinux.c
OBJS    = $(SRCS:.c=.o)
INCDIR  =
LIBDIR  = 
LIBS    = 

$(TARGET): $(OBJS)
		$(CC) $(CFLAGS) $^ -o $@ $(LIBDIR) $(LIBS)

$(OBJS): $(SRCS)
		$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)

clean:
		$(RM) -rf $(TARGET) $(OBJS) gamelinux*.deb

delobj:
		$(RM) -rf $(OBJS)

install:
		sudo $(CP) $(TARGET) /usr/bin/

deb: $(TARGET)
		$(CP) $(TARGET) work/usr/bin/
		$(FAKEROOT) dpkg-deb --build work .

all: clean $(OBJS) $(TARGET) deb install
