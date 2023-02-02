CC      = /usr/bin/cc 
FAKEROOT = /usr/bin/fakeroot
RM      = /bin/rm
CFLAGS  =
TARGET  = gamelinux
SRCS    = gamelinux.c conio.c
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

deb: $(TARGET)
		cp $(TARGET) work/usr/bin/
		$(FAKEROOT) dpkg-deb --build work .

all: clean $(OBJS) $(TARGET) deb
