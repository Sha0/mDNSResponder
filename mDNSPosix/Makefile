# $Log: Makefile,v $
# Revision 1.6  2003/04/04 01:37:14  cheshire
# Added NetMonitor.c
#

# I assume that cc will be in your path. If not, you have to change the following to point to it.
CC = cc
CFLAGS_COMMON = -g -I../mDNSCore -I. -DHAVE_MSGHDR_MSG_CONTROL -DMDNS_POSIX -DMDNS_DEBUGMSGS=2

ifeq ($(os),solaris)
CFLAGS_OS = -D_XPG4_2 -D__EXTENSIONS__ -DHAVE_SOCKLEN_T -DHAVE_BROKEN_RECVIF_NAME -lsocket -lnsl
else
ifeq ($(os),linux)
CFLAGS_OS = -DHAVE_DAEMON -DHAVE_SOCKLEN_T
else
ifeq ($(os),netbsd)
CFLAGS_OS = -DHAVE_SOCKADDR_SA_LEN -DHAVE_DAEMON -DHAVE_SOCKLEN_T
else
ifeq ($(os),freebsd)
CFLAGS_OS = -DHAVE_SOCKADDR_SA_LEN -DHAVE_DAEMON -DHAVE_SOCKLEN_T
else
ifeq ($(os),openbsd)
CFLAGS_OS = -DHAVE_SOCKADDR_SA_LEN -DHAVE_DAEMON -DHAVE_SOCKLEN_T -DHAVE_BROKEN_RECVDSTADDR
else
ifeq ($(os),osx)
CFLAGS_OS = -DHAVE_SOCKADDR_SA_LEN -DHAVE_DAEMON -DHAVE_IPV6 -Wall -no-cpp-precomp
else
cantbuild:
	echo "Error: Must specify target OS on command-line, e.g. \"make os=osx\" or \"make os=linux\""
endif
endif
endif
endif
endif
endif

CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_OS)

COMMONOBJ = objects/mDNSPosix.c.o objects/mDNSUNP.c.o objects/ExampleClientApp.c.o

HEADERS = Makefile mDNSUNP.h mDNSPosix.h   \
../mDNSCore/mDNSDebug.h                    \
../mDNSCore/mDNSClientAPI.h                \
../mDNSCore/mDNSPlatformFunctions.h

all: setup Client Responder ProxyResponder NetMonitor

setup:
	if test ! -d objects ; then mkdir objects ; fi
	if test ! -d build   ; then mkdir build   ; fi

Client: setup build/mDNSClientPosix
	echo "Client done"

Responder: setup build/mDNSResponderPosix
	echo "Responder done"

ProxyResponder: setup build/mDNSProxyResponderPosix
	echo "ProxyResponder done"

NetMonitor: setup build/mDNSNetMonitor
	echo "NetMonitor done"

build/mDNSClientPosix: $(COMMONOBJ) objects/mDNS.c.o objects/Client.c.o
	$(CC) $(CFLAGS) $+ -o $@

build/mDNSResponderPosix: $(COMMONOBJ) objects/mDNS.c.o objects/Responder.c.o
	$(CC) $(CFLAGS) $+ -o $@

build/mDNSProxyResponderPosix: $(COMMONOBJ) objects/mDNS.c.o objects/ProxyResponder.c.o
	$(CC) $(CFLAGS) $+ -o $@

build/mDNSNetMonitor: $(COMMONOBJ) objects/NetMonitor.c.o
	$(CC) $(CFLAGS) $+ -o $@

objects/%.c.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

objects/mDNS.c.o: ../mDNSCore/mDNS.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	-rm -rf objects build .gdb_history
