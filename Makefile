#
# Top level makefile for Build & Integration.
# 
# This file is used to facilitate checking the mDNSResponder project
# directly out of CVS and submitting to B&I at Apple.
#
# The various platform directories contain makefiles or projects
# specific to that platform.
#
#    B&I builds must respect the following target:
#         install:
#         installsrc:
#         installhdrs:
#         clean:
#

include /Developer/Makefiles/pb_makefiles/platform.make

MVERS = "mDNSResponder-110"

install:
	cd "$(SRCROOT)/mDNSMacOSX"; xcodebuild install     OBJROOT=$(OBJROOT) SYMROOT=$(SYMROOT) DSTROOT=$(DSTROOT) MVERS=$(MVERS)
	# Make sure ddnswriteconfig is owned by root:wheel, then make it setuid root executable
	chown 0:80 "$(DSTROOT)/Library/Application Support/Bonjour/ddnswriteconfig"
	chmod 4555 "$(DSTROOT)/Library/Application Support/Bonjour/ddnswriteconfig"

installsrc:
	ditto . "$(SRCROOT)"

installhdrs::
	cd "$(SRCROOT)/mDNSMacOSX"; xcodebuild installhdrs OBJROOT=$(OBJROOT) SYMROOT=$(SYMROOT) DSTROOT=$(DSTROOT) MVERS=$(MVERS)

clean::
	echo clean
