# $Log: Makefile,v $
# Revision 1.16  2003/12/11 03:03:51  rpantos
# Clean up mDNSPosix so that it builds on OS X again.
#
# Revision 1.15  2003/12/08 20:47:02  rpantos
# Add support for mDNSResponder on Linux.
#
# Revision 1.14  2003/11/14 20:59:09  cheshire
# Clients can't use AssignDomainName macro because mDNSPlatformMemCopy is defined in mDNSPlatformFunctions.h.
# Best solution is just to combine mDNSClientAPI.h and mDNSPlatformFunctions.h into a single file.
#
# Revision 1.13  2003/08/06 18:20:51  cheshire
# Makefile cleanup
#
# Revision 1.12  2003/08/01 02:20:02  cheshire
# Add mDNSIdentify tool, used to discover what version of mDNSResponder a particular host is running
#
# Revision 1.11  2003/07/14 18:11:54  cheshire
# Fix stricter compiler warnings
#
# Revision 1.10  2003/06/18 05:47:41  cheshire
# Enable stricter warnings on Jaguar and Panther builds
#
# Revision 1.9  2003/06/04 18:34:45  ksekar
# Bug #: <rdar://problem/3218120>: mDNSPosix does not build on Panther that has socklen_t
# Changed build targets "osx10.2" and "osx10.3" to "jaguar" and "panther".
#
# Revision 1.8  2003/06/04 00:23:12  ksekar
# Bug #: <rdar://problem/3218120>: mDNSPosix does not build on Panther that has socklen_t
# Created separate target OS's for 10.2 and 10.3.
#
# Revision 1.7  2003/04/16 02:11:37  cheshire
# Remove unnecessary $(CFLAGS) from linking rules
#
# Revision 1.6  2003/04/04 01:37:14  cheshire
# Added NetMonitor.c
#

# This Makefile builds an mDNSResponder daemon and a libmdns.so shared library 
# for Linux. It also builds several example programs for embedded systems.
#
# Make with no arguments to build all production targets.
# 'make DEBUG=1' to build debugging targets.
# 'make clean' or 'make clean DEBUG=1' to delete prod/debug objects & targets
# 'sudo make install [DEBUG=1]' to install mDNSResponderd daemon and libmdns.

LIBVERS = 1

COREDIR = ../mDNSCore
SHAREDDIR = ../mDNSShared

CC = cc
LD = ld
CP = cp
RM = rm
LN = ln -s -f
CFLAGS_COMMON = -I. -I$(COREDIR) -I$(SHAREDDIR) -W -Wall
LDFLAGS = -shared
LDSUFFIX = so

# Set up diverging paths for debug vs. prod builds
DEBUG=0
ifeq ($(DEBUG),1)
CFLAGS_DEBUG = -g -DMDNS_DEBUGMSGS=2 
OBJDIR = objects/debug
TARGDIR = build/debug
STRIP = echo 
else
CFLAGS_DEBUG = -Os -DMDNS_DEBUGMSGS=0 
OBJDIR = objects/prod
TARGDIR = build/prod
STRIP = strip -S 
endif

# Configure per-OS peculiarities
ifeq ($(os),solaris)
CFLAGS_OS =  -DNOT_HAVE_DAEMON -DNOT_HAVE_SA_LEN -D_XPG4_2 -D__EXTENSIONS__ -DHAVE_BROKEN_RECVIF_NAME -lsocket -lnsl
else
ifeq ($(os),linux)
CFLAGS_OS = -DNOT_HAVE_SA_LEN
else
ifeq ($(os),netbsd)
CFLAGS_OS =
else
ifeq ($(os),freebsd)
CFLAGS_OS =
else
ifeq ($(os),openbsd)
CFLAGS_OS = -DHAVE_BROKEN_RECVDSTADDR
else
ifeq ($(os),jaguar)
CFLAGS_OS = -DHAVE_IPV6 -no-cpp-precomp -DNOT_HAVE_SOCKLEN_T
LD = libtool
LDFLAGS = -dynamic -lSystem
LDSUFFIX = dylib
else
ifeq ($(os),panther)
CFLAGS_OS = -DHAVE_IPV6 -no-cpp-precomp
LD = libtool
LDFLAGS = -dynamic -lSystem
LDSUFFIX = dylib
else
cantbuild:
	@echo "Must specify target OS on command-line: \"make os={jaguar,panther,linux,netbsd,freebsd,openbsd,solaris}\""
endif
endif
endif
endif
endif
endif
endif

CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_OS) $(CFLAGS_DEBUG)

all: setup Daemon libmdns Client Responder ProxyResponder Identify NetMonitor

install: setup InstalledDaemon InstalledLib InstalledStartup


# 'setup' sets up the build directory structure the way we want
setup:
	if test ! -d objects ; then mkdir objects ; fi
	if test ! -d build   ; then mkdir build   ; fi
	if test ! -d $(OBJDIR) ; then mkdir $(OBJDIR) ; fi
	if test ! -d $(TARGDIR)   ; then mkdir $(TARGDIR)   ; fi


# clean removes targets and objects
clean:
	if test -d $(OBJDIR) ; then rm -r $(OBJDIR) ; fi
	if test -d $(TARGDIR)   ; then rm -r $(TARGDIR)   ; fi


# $@ means "The file name of the target of the rule"
# $< means "The name of the first prerequisite"
# $+ means "The names of all the prerequisites, with spaces between them, exactly as given"
# For more magic automatic variables, see
# <http://www.gnu.org/manual/make-3.80/html_chapter/make_10.html#SEC111>


# daemon target builds the daemon
DAEMONOBJS = $(OBJDIR)/PosixDaemon.c.o $(OBJDIR)/mDNSPosix.c.o $(OBJDIR)/mDNSUNP.c.o $(OBJDIR)/mDNS.c.o \
			$(OBJDIR)/uds_daemon.c.o $(OBJDIR)/mDNSDebug.c.o $(OBJDIR)/dnssd_ipc.c.o $(OBJDIR)/GenLinkedList.c.o 

Daemon: setup $(TARGDIR)/mDNSResponderd
	@echo "Responder daemon done"

$(TARGDIR)/mDNSResponderd: $(DAEMONOBJS)
	$(CC) -o $@ $+
	$(STRIP) $@


# libmdns target builds the client library
libmdns: setup $(TARGDIR)/libmdns.$(LDSUFFIX)
	@echo "Client library done"

$(TARGDIR)/libmdns.$(LDSUFFIX): $(OBJDIR)/dnssd_clientstub.c.so.o $(OBJDIR)/dnssd_ipc.c.so.o
	$(LD) $(LDFLAGS) -o $@ $+
	$(STRIP) $@


# The Install targets place built stuff in their proper places
InstalledDaemon: /usr/sbin/mDNSResponderd
	@echo $< " installed"

/usr/sbin/mDNSResponderd: $(TARGDIR)/mDNSResponderd
	$(CP) $(TARGDIR)/mDNSResponderd $@

InstalledLib: /usr/lib/libmdns.so.$(LIBVERS)
	@echo $< " installed"

/usr/lib/libmdns.so.$(LIBVERS): $(TARGDIR)/libmdns.so
	$(CP) $(TARGDIR)/libmdns.so $@
	$(LN) $@ /usr/lib/libmdns.so

InstalledStartup: /etc/init.d/mDNS
	@echo $< " installed"

/etc/init.d/mDNS: mDNSResponderd.sh
	$(CP) mDNSResponderd.sh $@
	chmod ugo+x $@
	$(LN) $@ /etc/rc2.d/S52mDNS
	$(LN) $@ /etc/rc3.d/S52mDNS
	$(LN) $@ /etc/rc4.d/S52mDNS
	$(LN) $@ /etc/rc5.d/S52mDNS
	$(LN) $@ /etc/rc2.d/K16mDNS
	$(LN) $@ /etc/rc3.d/K16mDNS
	$(LN) $@ /etc/rc4.d/K16mDNS
	$(LN) $@ /etc/rc5.d/K16mDNS

# The following targets build embedded example programs
COMMONOBJ = $(OBJDIR)/mDNSPosix.c.o $(OBJDIR)/mDNSUNP.c.o $(OBJDIR)/mDNSDebug.c.o $(OBJDIR)/GenLinkedList.c.o

Client: setup $(TARGDIR)/mDNSClientPosix
	@echo "Client done"

Responder: setup $(TARGDIR)/mDNSResponderPosix
	@echo "Responder done"

ProxyResponder: setup $(TARGDIR)/mDNSProxyResponderPosix
	@echo "ProxyResponder done"

Identify: setup $(TARGDIR)/mDNSIdentify
	@echo "Identify done"

NetMonitor: setup $(TARGDIR)/mDNSNetMonitor
	@echo "NetMonitor done"

$(TARGDIR)/mDNSClientPosix: $(COMMONOBJ) $(OBJDIR)/mDNS.c.o $(OBJDIR)/Client.c.o $(OBJDIR)/ExampleClientApp.c.o 
	$(CC) $+ -o $@

$(TARGDIR)/mDNSResponderPosix: $(COMMONOBJ) $(OBJDIR)/mDNS.c.o $(OBJDIR)/Responder.c.o
	$(CC) $+ -o $@

$(TARGDIR)/mDNSProxyResponderPosix: $(COMMONOBJ) $(OBJDIR)/mDNS.c.o $(OBJDIR)/ProxyResponder.c.o
	$(CC) $+ -o $@

$(TARGDIR)/mDNSIdentify: $(COMMONOBJ) $(OBJDIR)/Identify.c.o
	$(CC) $+ -o $@

$(TARGDIR)/mDNSNetMonitor: $(COMMONOBJ) $(OBJDIR)/NetMonitor.c.o
	$(CC) $+ -o $@


# implicit rules
$(OBJDIR)/%.c.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.c.o:	$(COREDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.c.o:	$(SHAREDDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.c.so.o:	%.c
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

$(OBJDIR)/%.c.so.o:	$(SHAREDDIR)/%.c
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

