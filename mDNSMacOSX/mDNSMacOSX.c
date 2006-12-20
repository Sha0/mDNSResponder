/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

    Change History (most recent first):

$Log: mDNSMacOSX.c,v $
Revision 1.356  2006/12/20 23:15:53  mkrochma
Fix the private domain list code so that it actually works

Revision 1.355  2006/12/20 23:04:36  mkrochma
Fix crash when adding private domain list to Dynamic Store

Revision 1.354  2006/12/19 22:43:55  cheshire
Fix compiler warnings

Revision 1.353  2006/12/14 22:08:29  cheshire
Fixed memory leak: need to call SecKeychainItemFreeAttributesAndData()
to release data allocated by SecKeychainItemCopyAttributesAndData()

Revision 1.352  2006/12/14 02:33:26  cheshire
<rdar://problem/4841422> uDNS: Wide-area registrations sometimes fail

Revision 1.351  2006/11/28 21:37:51  mkrochma
Tweak where the private DNS data is written

Revision 1.350  2006/11/28 07:55:02  herscher
<rdar://problem/4742743> dnsextd has a slow memory leak

Revision 1.349  2006/11/28 07:45:58  herscher
<rdar://problem/4787010> Daemon: Need to write list of private domain names to the DynamicStore

Revision 1.348  2006/11/16 21:47:20  mkrochma
<rdar://problem/4841422> uDNS: Wide-area registrations sometimes fail

Revision 1.347  2006/11/10 00:54:16  cheshire
<rdar://problem/4816598> Changing case of Computer Name doesn't work

Revision 1.346  2006/10/31 02:34:58  cheshire
<rdar://problem/4692130> Stop creating HINFO records

Revision 1.345  2006/09/21 20:04:38  mkrochma
Accidently changed function name while checking in previous fix

Revision 1.344  2006/09/21 19:04:13  mkrochma
<rdar://problem/4733803> uDNS: Update keychain format of DNS key to include prefix

Revision 1.343  2006/09/15 21:20:16  cheshire
Remove uDNS_info substructure from mDNS_struct

Revision 1.342  2006/08/16 00:31:50  mkrochma
<rdar://problem/4386944> Get rid of NotAnInteger references

Revision 1.341  2006/08/14 23:24:40  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.340  2006/07/29 19:11:13  mkrochma
Change GetUserSpecifiedDDNSConfig LogMsg to debugf

Revision 1.339  2006/07/27 03:24:35  cheshire
<rdar://problem/4049048> Convert mDNSResponder to use kqueue
Further refinement: Declare KQueueEntry parameter "const"

Revision 1.338  2006/07/27 02:59:25  cheshire
<rdar://problem/4049048> Convert mDNSResponder to use kqueue
Further refinements: CFRunLoop thread needs to explicitly wake the kqueue thread
after releasing BigMutex, in case actions it took have resulted in new work for the
kqueue thread (e.g. NetworkChanged events may result in the kqueue thread having to
add new active interfaces to its list, and consequently schedule queries to be sent).

Revision 1.337  2006/07/22 06:11:37  cheshire
<rdar://problem/4049048> Convert mDNSResponder to use kqueue

Revision 1.336  2006/07/15 02:01:32  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Fix broken "empty string" browsing

Revision 1.335  2006/07/14 05:25:11  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder
Fixed crash in mDNSPlatformGetDNSConfig() reading BrowseDomains array

Revision 1.334  2006/07/05 23:42:00  cheshire
<rdar://problem/4472014> Add Private DNS client functionality to mDNSResponder

Revision 1.333  2006/06/29 05:33:30  cheshire
<rdar://problem/4607043> mDNSResponder conditional compilation options

Revision 1.332  2006/06/28 09:10:36  cheshire
Extra debugging messages

Revision 1.331  2006/06/21 22:29:42  cheshire
Make _CFCopySystemVersionDictionary() call more defensive on systems that have no build information set

Revision 1.330  2006/06/20 23:06:00  cheshire
Fix some keychain API type mismatches (was mDNSu32 instead of UInt32)

Revision 1.329  2006/06/08 23:22:33  cheshire
Comment changes

Revision 1.328  2006/03/19 03:27:49  cheshire
<rdar://problem/4118624> Suppress "interface flapping" logic for loopback

Revision 1.327  2006/03/19 02:00:09  cheshire
<rdar://problem/4073825> Improve logic for delaying packets after repeated interface transitions

Revision 1.326  2006/03/08 22:42:23  cheshire
Fix spelling mistake: LocalReverseMapomain -> LocalReverseMapDomain

Revision 1.325  2006/01/10 00:39:17  cheshire
Add comments explaining how IPv6 link-local addresses sometimes have an embedded scope_id

Revision 1.324  2006/01/09 19:28:59  cheshire
<rdar://problem/4403128> Cap number of "sendto failed" messages we allow mDNSResponder to log

Revision 1.323  2006/01/05 21:45:27  cheshire
<rdar://problem/4400118> Fix uninitialized structure member in IPv6 code

Revision 1.322  2006/01/05 21:41:50  cheshire
<rdar://problem/4108164> Reword "mach_absolute_time went backwards" dialog

Revision 1.321  2006/01/05 21:35:06  cheshire
Add (commented out) trigger value for testing "mach_absolute_time went backwards" notice

Revision 1.320  2005/12/03 01:39:28  cheshire
<rdar://problem/4363411> Improve diagnostic message to indicate that message will not appear to customers

Revision 1.319  2005/12/02 00:02:15  cheshire
Include recvmsg return value in error message

Revision 1.318  2005/10/20 00:10:34  cheshire
<rdar://problem/4290265> Add check to avoid crashing NAT gateways that have buggy DNS relay code

Revision 1.317  2005/09/24 01:10:26  cheshire
Fix comment typos

Revision 1.316  2005/07/29 18:04:22  ksekar
<rdar://problem/4137930> Hostname registration should register IPv6 AAAA record with DNS Update

Revision 1.315  2005/07/22 21:50:55  ksekar
Fix GCC 4.0/Intel compiler warnings

Revision 1.314  2005/07/11 02:12:09  cheshire
<rdar://problem/4147774> Be defensive against invalid UTF-8 in dynamic host names
Fix copy-and-paste error: "CFRelease(StatusVals[0]);" should be "CFRelease(StateVals[0]);"

Revision 1.313  2005/07/04 23:52:25  cheshire
<rdar://problem/3923098> Things are showing up with a bogus interface index

Revision 1.312  2005/07/04 22:24:36  cheshire
Export NotifyOfElusiveBug() so other files can call it

Revision 1.311  2005/06/15 13:20:43  cheshire
<rdar://problem/4147774> Be defensive against invalid UTF-8 in dynamic host names

Revision 1.310  2005/04/07 00:49:58  cheshire
<rdar://problem/4080074> PPP connection disables Bonjour ".local" lookups

Revision 1.309  2005/03/23 05:53:29  cheshire
Fix %s where it should have been %##s in debugf & LogMsg calls

Revision 1.308  2005/03/09 00:48:44  cheshire
<rdar://problem/4015157> QU packets getting sent too early on wake from sleep
Move "m->p->NetworkChanged = 0;" line from caller to callee

Revision 1.307  2005/03/03 03:12:02  cheshire
Add comment about mDNSMacOSXSystemBuildNumber()

Revision 1.306  2005/03/02 22:18:00  cheshire
<rdar://problem/3930171> mDNSResponder requires AppleInternal packages to build on Tiger

Revision 1.305  2005/02/26 05:08:28  cheshire
<rdar://problem/3930171> mDNSResponder requires AppleInternal packages to build on Tiger
Added dnsinfo.h to project directory

Revision 1.304  2005/02/25 23:51:22  cheshire
<rdar://problem/4021868> SendServiceRegistration fails on wake from sleep
Return mStatus_UnknownErr instead of -1

Revision 1.303  2005/02/25 17:47:45  ksekar
<rdar://problem/4021868> SendServiceRegistration fails on wake from sleep

Revision 1.302  2005/02/25 02:34:14  cheshire
<rdar://problem/4017292> Should not indicate successful dynamic update if no network connection
Show status as 1 (in progress) while we're trying

Revision 1.301  2005/02/24 21:55:57  ksekar
<rdar://problem/4017292> Should not indicate successful dynamic update if no network connection

Revision 1.300  2005/02/15 20:03:13  ksekar
<rdar://problem/4005868> Crash when SCPreferences contains empty array

Revision 1.299  2005/02/15 02:46:53  cheshire
<rdar://problem/3967876> Don't log ENETUNREACH errors for unicast destinations

Revision 1.298  2005/02/10 00:41:59  cheshire
Fix compiler warning

Revision 1.297  2005/02/09 23:38:51  ksekar
<rdar://problem/3993508> Reregister hostname when DNS server changes but IP address does not

Revision 1.296  2005/02/01 21:06:52  ksekar
Avoid spurious log message

Revision 1.295  2005/02/01 19:33:30  ksekar
<rdar://problem/3985239> Keychain format too restrictive

Revision 1.294  2005/01/27 21:30:23  cheshire
<rdar://problem/3952067> "Can't assign requested address" message after AirPort turned off
Don't write syslog messages for EADDRNOTAVAIL if we know network configuration changes are happening

Revision 1.293  2005/01/27 19:15:41  cheshire
Remove extraneous LogMsg() call

Revision 1.292  2005/01/27 17:48:38  cheshire
Added comment about CFSocketInvalidate closing the underlying socket

Revision 1.291  2005/01/27 00:10:58  cheshire
<rdar://problem/3967867> Name change log messages every time machine boots

Revision 1.290  2005/01/25 23:18:30  ksekar
fix for <rdar://problem/3971467> requires that local-only ".local" registration record be created

Revision 1.289  2005/01/25 18:08:31  ksekar
Removed redundant debug output

Revision 1.288  2005/01/25 17:42:26  ksekar
Renamed FoundDefBrowseDomain -> FoundLegacyBrowseDomain,
cleaned up duplicate log messages when adding/removing browse domains

Revision 1.287  2005/01/25 16:59:23  ksekar
<rdar://problem/3971138> sa_len not set checking reachability for TCP connections

Revision 1.286  2005/01/25 02:02:37  cheshire
<rdar://problem/3970673> mDNSResponder leaks
GetSearchDomains() was not calling dns_configuration_free().

Revision 1.285  2005/01/22 00:07:54  ksekar
<rdar://problem/3960546> mDNSResponder should look at all browse domains in SCPreferences

Revision 1.284  2005/01/21 23:07:17  ksekar
<rdar://problem/3960795> mDNSResponder causes Dial on Demand

Revision 1.283  2005/01/19 21:16:16  cheshire
Make sure when we set NetworkChanged that we don't set it to zero

Revision 1.282  2005/01/19 19:19:21  ksekar
<rdar://problem/3960191> Need a way to turn off domain discovery

Revision 1.281  2005/01/18 18:10:55  ksekar
<rdar://problem/3954575> Use 10.4 resolver API to get search domains

Revision 1.280  2005/01/17 22:48:52  ksekar
No longer need to call MarkSearchListElem for registration domain

Revision 1.279  2005/01/17 20:40:34  ksekar
SCPreferences changes should remove exactly one browse and one legacy browse domain for each remove event

Revision 1.278  2005/01/17 19:53:34  ksekar
Refinement to previous fix - register _legacy._browse records for SCPreference domains to achieve correct reference counting

Revision 1.277  2005/01/12 00:17:50  ksekar
<rdar://problem/3933573> Update LLQs *after* setting DNS

Revision 1.276  2005/01/10 17:39:10  ksekar
Refinement to 1.272 - avoid spurious warnings when registration and browse domains are set to same value and toggled on/off

Revision 1.275  2005/01/10 04:02:22  ksekar
Refinement to <rdar://problem/3891628> - strip trailing dot before writing hostname status to dynamic store

Revision 1.274  2005/01/10 03:41:36  ksekar
Correction to checkin 1.272 - check that registration domain is set
before trying to remove it as an implicit browse domain

Revision 1.273  2005/01/08 00:42:18  ksekar
<rdar://problem/3922758> Clean up syslog messages

Revision 1.272  2005/01/07 23:21:42  ksekar
<rdar://problem/3891628> Clean up SCPreferences format

Revision 1.271  2004/12/20 23:18:12  cheshire
<rdar://problem/3485365> Guard against repeating wireless dissociation/re-association
One more refinement: When an interface with a v6LL address gets a v4 address too, that's not a flap

Revision 1.270  2004/12/20 21:28:14  cheshire
<rdar://problem/3485365> Guard against repeating wireless dissociation/re-association
Additional refinements to handle sleep/wake better

Revision 1.269  2004/12/20 20:48:11  cheshire
Only show "mach_absolute_time went backwards" notice on 10.4 (build 8xxx) or later

Revision 1.268  2004/12/18 03:19:04  cheshire
Show netmask in error log

Revision 1.267  2004/12/18 00:51:52  cheshire
Use symbolic constant kDNSServiceInterfaceIndexLocalOnly instead of (mDNSu32) ~0

Revision 1.266  2004/12/17 23:49:38  cheshire
<rdar://problem/3922754> Computer Name change is slow
Also treat changes to "Setup:/Network/DynamicDNS" the same way

Revision 1.265  2004/12/17 23:37:47  cheshire
<rdar://problem/3485365> Guard against repeating wireless dissociation/re-association
(and other repetitive configuration changes)

Revision 1.264  2004/12/17 19:03:05  cheshire
Update debugging messages to show netmask a simple CIDR-style numeric value (0-128)

Revision 1.263  2004/12/17 05:25:46  cheshire
<rdar://problem/3925163> Shorten DNS-SD queries to avoid NAT bugs

Revision 1.262  2004/12/17 04:48:32  cheshire
<rdar://problem/3922754> Computer Name change is slow

Revision 1.261  2004/12/17 02:40:08  cheshire
Undo last change -- it was too strict

Revision 1.260  2004/12/16 22:17:16  cheshire
Only accept multicast packets on interfaces that have McastTxRx set

Revision 1.259  2004/12/16 20:13:01  cheshire
<rdar://problem/3324626> Cache memory management improvements

Revision 1.258  2004/12/14 00:18:05  cheshire
Don't log dns_configuration_copy() failures in the first three minutes after boot

Revision 1.257  2004/12/10 19:45:46  cheshire
<rdar://problem/3915074> Reduce egregious stack space usage
Reduced myCFSocketCallBack() stack frame from 9K to 512 bytes

Revision 1.256  2004/12/10 04:35:43  cheshire
<rdar://problem/3907233> Show "Note: Compiled without Apple-specific split DNS support" only once

Revision 1.255  2004/12/10 04:12:54  ksekar
<rdar://problem/3890764> Need new DefaultBrowseDomain key

Revision 1.254  2004/12/10 01:55:31  ksekar
<rdar://problem/3899067> Keychain lookups should be in lower case.

Revision 1.253  2004/12/09 03:15:41  ksekar
<rdar://problem/3806610> use _legacy instead of _default to find "empty string" browse domains

Revision 1.252  2004/12/07 01:32:42  cheshire
Don't log dns_configuration_copy() failure when running on 10.3

Revision 1.251  2004/12/06 22:30:31  cheshire
Added debugging log message

Revision 1.250  2004/12/06 06:59:08  ksekar
RegisterSplitDNS should return Unsupported error when compiled on Panther

Revision 1.249  2004/12/04 00:29:46  cheshire
Add "#ifdef MAC_OS_X_VERSION_10_4" around split-DNS code that can't be compiled on 10.3 systems
(When compiled on 10.3, code will not include split-DNS support.)

Revision 1.248  2004/12/01 20:57:20  ksekar
<rdar://problem/3873921> Wide Area Service Discovery must be split-DNS aware

Revision 1.247  2004/12/01 03:26:58  cheshire
Remove unused variables

Revision 1.246  2004/12/01 01:51:34  cheshire
Move ReadDDNSSettingsFromConfFile() from mDNSMacOSX.c to PlatformCommon.c

Revision 1.245  2004/11/30 03:24:04  cheshire
<rdar://problem/3854544> Defer processing network configuration changes until configuration has stabilized

Revision 1.244  2004/11/30 02:59:35  cheshire
For debugging diagnostics, added identifying strings in SCDynamicStoreCreate() calls

Revision 1.243  2004/11/29 19:17:29  ksekar
<rdar://problem/3878195> Unnecessary GetUserSpecifiedDDNSConfig log messages

Revision 1.242  2004/11/29 18:37:38  ksekar
<rdar://problem/3889341> Buffer overflow in GetConfigOption

Revision 1.241  2004/11/25 01:37:04  ksekar
<rdar://problem/3894854> Config file and SCPreferences don't play well together

Revision 1.240  2004/11/25 01:29:42  ksekar
Remove unnecessary log messages

Revision 1.239  2004/11/25 01:27:19  ksekar
<rdar://problem/3885859> Don't try to advertise link-local IP addresses via dynamic update

Revision 1.238  2004/11/24 22:00:59  cheshire
Move definition of mDNSAddressIsAllDNSLinkGroup() from mDNSMacOSX.c to mDNSEmbeddedAPI.h

Revision 1.237  2004/11/24 21:54:44  cheshire
<rdar://problem/3894475> mDNSCore not receiving unicast responses properly

Revision 1.236  2004/11/23 03:39:46  cheshire
Let interface name/index mapping capability live directly in JNISupport.c,
instead of having to call through to the daemon via IPC to get this information.

Revision 1.235  2004/11/17 01:45:35  cheshire
<rdar://problem/3847435> mDNS buddy list frequently becomes empty if you let the machine sleep
Refresh our interface list on receiving kIOMessageSystemHasPoweredOn,
in case we get no System Configuration Framework "network changed" event.

Revision 1.234  2004/11/17 00:32:56  ksekar
<rdar://problem/3880773> mDNSResponder will not discover zones contained both in Search Domains and DHCP Domain option

Revision 1.233  2004/11/12 03:16:45  rpantos
rdar://problem/3809541 Add mDNSPlatformGetInterfaceByName, mDNSPlatformGetInterfaceName

Revision 1.232  2004/11/10 20:40:54  ksekar
<rdar://problem/3868216> LLQ mobility fragile on non-primary interface

Revision 1.231  2004/11/06 00:59:33  ksekar
Don't log ENETDOWN errors for unicast destinations (pollutes log on
wake from sleep)

Revision 1.230  2004/11/05 01:04:10  ksekar
<rdar://problem/3774577> LegacyNATDestroy() called too enthusiastically

Revision 1.229  2004/11/03 03:45:16  cheshire
<rdar://problem/3863627> mDNSResponder does not inform user of Computer Name collisions

Revision 1.228  2004/11/02 23:47:32  cheshire
<rdar://problem/3863214> Default hostname and Computer Name should be unique

Revision 1.227  2004/11/02 04:23:03  cheshire
Change to more informative name "GetUserSpecifiedLocalHostName()"

Revision 1.226  2004/11/01 20:36:19  ksekar
<rdar://problem/3802395> mDNSResponder should not receive Keychain Notifications

Revision 1.225  2004/10/28 19:03:04  cheshire
Remove \n from LogMsg() calls

Revision 1.224  2004/10/28 17:47:34  cheshire
Oops. Forgot the %d in the log message.

Revision 1.223  2004/10/28 17:24:28  cheshire
Updated "bad ifa_netmask" log message to give more information

Revision 1.222  2004/10/28 03:36:34  cheshire
<rdar://problem/3856535> Share the same port for both multicast and unicast receiving

Revision 1.221  2004/10/28 03:24:41  cheshire
Rename m->CanReceiveUnicastOn as m->CanReceiveUnicastOn5353

Revision 1.220  2004/10/28 00:53:57  cheshire
Export mDNSMacOSXNetworkChanged() so it's callable from outside this mDNSMacOSX.c;
Add LogOperation() call to record when we get network change events

Revision 1.219  2004/10/27 20:42:20  cheshire
Clean up debugging messages

Revision 1.218  2004/10/27 02:03:59  cheshire
Update debugging messages

Revision 1.217  2004/10/26 20:48:21  cheshire
Improve logging messages

Revision 1.216  2004/10/26 01:02:37  cheshire
Update comment

Revision 1.215  2004/10/25 20:09:00  ksekar
Cleaned up config file parsing.

Revision 1.214  2004/10/25 19:30:53  ksekar
<rdar://problem/3827956> Simplify dynamic host name structures

Revision 1.213  2004/10/23 01:16:01  cheshire
<rdar://problem/3851677> uDNS operations not always reliable on multi-homed hosts

Revision 1.212  2004/10/22 20:52:08  ksekar
<rdar://problem/3799260> Create NAT port mappings for Long Lived Queries

Revision 1.211  2004/10/22 01:07:11  cheshire
<rdar://problem/3375328> select() says data is waiting; recvfrom() says there is no data
Log error message if socket() ever returns file descriptors 0, 1 or 2 (stdin/stdout/stderr).
These are all supposed to be remapped to /dev/null

Revision 1.210  2004/10/20 02:19:54  cheshire
Eliminate "SetupAddr invalid sa_family" warning from RegisterSearchDomains()

Revision 1.209  2004/10/16 00:17:00  cheshire
<rdar://problem/3770558> Replace IP TTL 255 check with local subnet source address check

Revision 1.208  2004/10/15 23:00:18  ksekar
<rdar://problem/3799242> Need to update LLQs on location changes

Revision 1.207  2004/10/13 22:45:23  cheshire
<rdar://problem/3438392> Ten-second delay before kIOMessageSystemHasPoweredOn message

Revision 1.206  2004/10/13 22:11:46  cheshire
Update debugging messages

Revision 1.205  2004/10/12 21:10:11  cheshire
<rdar://problem/3438376> mach_absolute_time() not monotonically increasing
Do a NotifyOfElusiveBug() if we see mach_absolute_time() go backwards

Revision 1.204  2004/10/12 03:20:52  ksekar
<rdar://problem/3835614> Incorrect LogMsg produces garbage on errors

Revision 1.203  2004/10/08 04:29:25  ksekar
<rdar://problem/3831842> Allow default search domains to be set via hint from DHCP

Revision 1.202  2004/10/04 05:56:04  cheshire
<rdar://problem/3824730> mDNSResponder doesn't respond to certain AirPort changes

Revision 1.201  2004/09/30 00:24:59  ksekar
<rdar://problem/3695802> Dynamically update default registration domains on config change

Revision 1.200  2004/09/26 23:20:35  ksekar
<rdar://problem/3813108> Allow default registrations in multiple wide-area domains

Revision 1.199  2004/09/24 23:54:55  cheshire
<rdar://problem/3787102> Don't use kCFSocketCloseOnInvalidate

Revision 1.198  2004/09/24 23:47:49  cheshire
Correct comment and error message

Revision 1.197  2004/09/24 23:39:27  cheshire
<rdar://problem/3733705> Only IPv6 loopback address advertised on laptop w/no networking

Revision 1.196  2004/09/24 20:53:04  cheshire
Revise error message to say "Setsockopt SO_REUSEPORT failed" instead of "Flaw in Kernel"

Revision 1.195  2004/09/24 19:21:45  cheshire
<rdar://problem/3671626> Report "Address already in use" errors

Revision 1.194  2004/09/24 19:16:54  cheshire
Remove "mDNS *const m" parameter from NotifyOfElusiveBug();
Refine error message to say "Flaw in Kernel (select/recvfrom mismatch)"

Revision 1.193  2004/09/22 00:41:59  cheshire
Move tcp connection status codes into the legal range allocated for mDNS use

Revision 1.192  2004/09/21 21:02:55  cheshire
Set up ifname before calling mDNS_RegisterInterface()

Revision 1.191  2004/09/21 19:19:36  cheshire
<rdar://problem/3760923> Combine WatchForDynDNSChanges() into WatchForNetworkChanges()

Revision 1.190  2004/09/21 19:04:45  cheshire
Strip trailing white space from the ends of lines

Revision 1.189  2004/09/21 00:13:28  cheshire
Fix build failure (io_connect_t and io_object_t are integers, not pointers)

Revision 1.188  2004/09/20 23:52:02  cheshire
CFSocket{Puma}.c renamed to mDNSMacOSX{Puma}.c

Revision 1.187  2004/09/18 01:37:01  cheshire
Update comment

Revision 1.186  2004/09/18 01:11:57  ksekar
<rdar://problem/3806734> Add a user's default domain to empty-string browse list

Revision 1.185  2004/09/17 01:08:52  cheshire
Renamed mDNSClientAPI.h to mDNSEmbeddedAPI.h
  The name "mDNSClientAPI.h" is misleading to new developers looking at this code. The interfaces
  declared in that file are ONLY appropriate to single-address-space embedded applications.
  For clients on general-purpose computers, the interfaces defined in dns_sd.h should be used.

Revision 1.184  2004/09/17 00:19:10  cheshire
For consistency with AllDNSLinkGroupv6, rename AllDNSLinkGroup to AllDNSLinkGroupv4

Revision 1.183  2004/09/17 00:15:56  cheshire
Rename mDNSPlatformInit_ReceiveUnicast to mDNSPlatformInit_CanReceiveUnicast

Revision 1.182  2004/09/16 21:36:36  cheshire
<rdar://problem/3803162> Fix unsafe use of mDNSPlatformTimeNow()
Changes to add necessary locking calls around unicast DNS operations

Revision 1.181  2004/09/16 02:03:42  cheshire
<rdar://problem/3802944> Change address to notify user of kernel flaw

Revision 1.180  2004/09/16 01:58:22  cheshire
Fix compiler warnings

Revision 1.179  2004/09/16 00:24:49  cheshire
<rdar://problem/3803162> Fix unsafe use of mDNSPlatformTimeNow()

Revision 1.178  2004/09/15 21:51:34  cheshire
<rdar://problem/3387020> mDNSResponder should notify user of kernel flaw
Calling CFUserNotificationDisplayNotice too early in the boot process seems to kill
the machine, so make sure we don't do this until at least three minutes after boot.

Revision 1.177  2004/09/15 01:16:29  cheshire
<rdar://problem/3387020> mDNSResponder should notify user of kernel flaw

Revision 1.176  2004/09/14 23:42:36  cheshire
<rdar://problem/3801296> Need to seed random number generator from platform-layer data

Revision 1.175  2004/09/14 21:35:46  cheshire
Minor code tidying, and added comments about CFRetainCounts

Revision 1.174  2004/09/14 19:14:57  ksekar
<rdar://problem/3192531> DynDNS: Discovery of DynDNS Zones via Reverse-Map PTR

Revision 1.173  2004/08/25 23:35:22  ksekar
<rdar://problem/3770615>: Error converting shared secret from base-64 to binary

Revision 1.172  2004/08/25 02:01:45  cheshire
<rdar://problem/3774777> Need to be able to get status of Dynamic DNS Host Name Update

Revision 1.171  2004/08/25 01:04:42  cheshire
Don't need to CFRelease name and array

Revision 1.170  2004/08/25 00:37:28  ksekar
<rdar://problem/3774635>: Cleanup DynDNS hostname registration code

Revision 1.169  2004/08/18 17:35:41  ksekar
<rdar://problem/3651443>: Feature #9586: Need support for Legacy NAT gateways

Revision 1.168  2004/08/17 03:16:24  ksekar
Fixed checkin 1.166 - enumeration type changed for wrong invocation of mDNS_GetDomains

Revision 1.167  2004/08/17 00:52:43  ksekar
Fix config file parse error, make semantics match SCPreferences
configuration input.

Revision 1.166  2004/08/16 19:55:07  ksekar
Change enumeration type to BrowseDefault to construct empty-string
browse list as result of checking 1.161.

Revision 1.165  2004/08/16 19:52:40  ksekar
Pass computer name + zone for FQDN after keychain notification,
setting global default service registration domain to the zone.

Revision 1.164  2004/08/16 16:52:37  ksekar
Pass in zone read from keychain to mDNS_SetFQDNs.

Revision 1.163  2004/08/14 03:22:42  cheshire
<rdar://problem/3762579> Dynamic DNS UI <-> mDNSResponder glue
Add GetUserSpecifiedDDNSName() routine
Convert ServiceRegDomain to domainname instead of C string
Replace mDNS_GenerateFQDN/mDNS_GenerateGlobalFQDN with mDNS_SetFQDNs

Revision 1.162  2004/08/12 22:34:00  cheshire
All strings should be read as kCFStringEncodingUTF8, not kCFStringEncodingASCII

Revision 1.161  2004/08/11 00:17:46  ksekar
<rdar://problem/3757662>: 8A227: Need Lighthouse configred machines to
set default bit for their domains

Revision 1.160  2004/07/29 19:27:16  ksekar
NAT-PMP Support - minor fixes and cleanup

Revision 1.159  2004/07/26 22:49:31  ksekar
<rdar://problem/3651409>: Feature #9516: Need support for NAT-PMP in client

Revision 1.158  2004/07/13 21:24:24  rpantos
Fix for <rdar://problem/3701120>.

Revision 1.157  2004/06/08 18:54:48  ksekar
<rdar://problem/3681378>: mDNSResponder leaks after exploring in Printer Setup Utility

Revision 1.156  2004/06/05 00:04:26  cheshire
<rdar://problem/3668639>: wide-area domains should be returned in reg. domain enumeration

Revision 1.155  2004/06/04 08:58:30  ksekar
<rdar://problem/3668624>: Keychain integration for secure dynamic update

Revision 1.154  2004/05/31 22:22:28  ksekar
<rdar://problem/3668639>: wide-area domains should be returned in
reg. domain enumeration

Revision 1.153  2004/05/26 17:06:33  cheshire
<rdar://problem/3668515>: Don't rely on CFSocketInvalidate() to remove RunLoopSource

Revision 1.152  2004/05/18 23:51:26  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.151  2004/05/17 21:46:34  cheshire
<rdar://problem/3616426>: When interface is turned off, browse "remove" events are delivered with interface index zero
Take care to correctly update InterfaceIDs when a dormant interface comes back to life

Revision 1.150  2004/05/13 04:54:20  ksekar
Unified list copy/free code.  Added symetric list for

Revision 1.149  2004/05/13 03:55:14  ksekar
Fixed list traversal bug in FoundDefSearchDomain.

Revision 1.148  2004/05/12 22:03:08  ksekar
Made GetSearchDomainList a true platform-layer call (declaration moved
from mDNSMacOSX.h to mDNSEmbeddedAPI.h), impelemted to return "local"
only on non-OSX platforms.  Changed call to return a copy of the list
to avoid shared memory issues.  Added a routine to free the list.

Revision 1.147  2004/05/12 02:03:25  ksekar
Non-local domains will only be browsed by default, and show up in
_browse domain enumeration, if they contain an _browse._dns-sd ptr record.

Revision 1.146  2004/04/27 02:49:15  cheshire
<rdar://problem/3634655>: mDNSResponder leaks sockets on bind() error

Revision 1.145  2004/04/21 03:08:03  cheshire
Rename 'alias' to more descriptive name 'primary'

Revision 1.144  2004/04/21 03:04:35  cheshire
Minor cleanup for clarity

Revision 1.143  2004/04/21 03:03:30  cheshire
Preparation work: AddInterfaceToList() should return pointer to structure it creates

Revision 1.142  2004/04/21 02:49:11  cheshire
To reduce future confusion, renamed 'TxAndRx' to 'McastTxRx'

Revision 1.141  2004/04/21 02:20:47  cheshire
Rename interface field 'CurrentlyActive' to more descriptive 'Exists'

Revision 1.140  2004/04/14 23:09:29  ksekar
Support for TSIG signed dynamic updates.

Revision 1.139  2004/04/09 17:40:26  cheshire
Remove unnecessary "Multicast" field -- it duplicates the semantics of the existing McastTxRx field

Revision 1.138  2004/04/09 16:37:16  cheshire
Suggestion from Bob Bradley:
Move NumCacheRecordsForInterfaceID() to DNSCommon.c so it's available to all platform layers

Revision 1.137  2004/04/08 00:59:55  cheshire
<rdar://problem/3609972> When interface turned off, browse "remove" events delivered with interface index zero
Unify use of the InterfaceID field, and make code that walks the list respect the 'Exists' flag

Revision 1.136  2004/04/07 01:08:57  cheshire
<rdar://problem/3609972> When interface turned off, browse "remove" events delivered with interface index zero

Revision 1.135  2004/03/19 01:01:03  ksekar
Fixed config file parsing to chop newline

Revision 1.134  2004/03/13 01:57:34  ksekar
<rdar://problem/3192546>: DynDNS: Dynamic update of service records

Revision 1.133  2004/02/02 22:46:56  cheshire
Move "CFRelease(dict);" inside the "if (dict)" check

Revision 1.132  2004/01/28 02:30:08  ksekar
Added default Search Domains to unicast browsing, controlled via
Networking sharing prefs pane.  Stopped sending unicast messages on
every interface.  Fixed unicast resolving via mach-port API.

Revision 1.131  2004/01/27 22:57:48  cheshire
<rdar://problem/3534352>: Need separate socket for issuing unicast queries

Revision 1.130  2004/01/27 22:28:40  cheshire
<rdar://problem/3541288>: Time to prune obsolete code for listening on port 53
Additional lingering port 53 code deleted

Revision 1.129  2004/01/27 20:15:23  cheshire
<rdar://problem/3541288>: Time to prune obsolete code for listening on port 53

Revision 1.128  2004/01/24 23:58:17  cheshire
Change to use mDNSVal16() instead of shifting and ORing

Revision 1.127  2004/01/24 04:59:16  cheshire
Fixes so that Posix/Linux, OS9, Windows, and VxWorks targets build again

Revision 1.126  2004/01/23 23:23:15  ksekar
Added TCP support for truncated unicast messages.

Revision 1.125  2004/01/22 03:43:09  cheshire
Export constants like mDNSInterface_LocalOnly so that the client layers can use them

Revision 1.124  2004/01/21 21:53:19  cheshire
<rdar://problem/3448144>: Don't try to receive unicast responses if we're not the first to bind to the UDP port

Revision 1.123  2004/01/20 03:18:25  cheshire
Removed "LogMsg("Hey There!");" that evidently got checked in my mistake

Revision 1.122  2003/12/17 20:43:59  cheshire
<rdar://problem/3496728>: Syslog messages saying "sendto failed"

Revision 1.121  2003/12/13 03:05:28  ksekar
<rdar://problem/3192548>: DynDNS: Unicast query of service records

Revision 1.120  2003/12/08 21:00:46  rpantos
Changes to support mDNSResponder on Linux.

Revision 1.119  2003/12/03 02:35:15  cheshire
Also report value of m->timenow when logging sendto() failure

Revision 1.118  2003/11/14 20:59:09  cheshire
Clients can't use AssignDomainName macro because mDNSPlatformMemCopy is defined in mDNSPlatformFunctions.h.
Best solution is just to combine mDNSEmbeddedAPI.h and mDNSPlatformFunctions.h into a single file.

Revision 1.117  2003/11/08 22:18:29  cheshire
<rdar://problem/3477870>: Don't need to show process ID in *every* mDNSResponder syslog message

Revision 1.116  2003/09/23 16:39:49  cheshire
When LogAllOperations is set, also report registration and deregistration of interfaces

Revision 1.115  2003/09/10 00:45:55  cheshire
<rdar://problem/3412328> Don't log "sendto failed" errors during the first two minutes of startup

Revision 1.114  2003/08/27 02:55:13  cheshire
<rdar://problem/3387910>: Bug: Don't report mDNSPlatformSendUDP sendto errno 64 (Host is down)

Revision 1.113  2003/08/19 22:20:00  cheshire
<rdar://problem/3376721> Don't use IPv6 on interfaces that have a routable IPv4 address configured
More minor refinements

Revision 1.112  2003/08/19 03:04:43  cheshire
<rdar://problem/3376721> Don't use IPv6 on interfaces that have a routable IPv4 address configured

Revision 1.111  2003/08/18 22:53:37  cheshire
<rdar://problem/3382647> mDNSResponder divide by zero in mDNSPlatformRawTime()

Revision 1.110  2003/08/16 03:39:00  cheshire
<rdar://problem/3338440> InterfaceID -1 indicates "local only"

Revision 1.109  2003/08/15 02:19:49  cheshire
<rdar://problem/3375225> syslog messages: myCFSocketCallBack recvfrom skt 6 error -1 errno 35
Also limit number of messages to at most 100

Revision 1.108  2003/08/12 22:24:52  cheshire
<rdar://problem/3375225> syslog messages: myCFSocketCallBack recvfrom skt 6 error -1 errno 35
This message indicates a kernel bug, but still we don't want to flood syslog.
Do a sleep(1) after writing this log message, to limit the rate.

Revision 1.107  2003/08/12 19:56:25  cheshire
Update to APSL 2.0

Revision 1.106  2003/08/12 13:48:32  cheshire
Add comment explaining clockdivisor calculation

Revision 1.105  2003/08/12 13:44:14  cheshire
<rdar://problem/3370229> mDNSResponder *VERY* unhappy if time goes backwards
Use mach_absolute_time() (which is guaranteed to always go forwards, resetting only on reboot)
instead of gettimeofday() (which can jump back if the user manually changes their time/date)

Revision 1.104  2003/08/12 13:12:07  cheshire
Textual search/replace: Indicate local functions using "mDNSlocal" instead of "static"

Revision 1.103  2003/08/08 18:36:04  cheshire
<rdar://problem/3344154> Only need to revalidate on interface removal on platforms that have the PhantomInterfaces bug

Revision 1.102  2003/08/06 00:14:52  cheshire
<rdar://problem/3330324> Need to check IP TTL on responses
Also add corresponding checks in the IPv6 code path

Revision 1.101  2003/08/05 22:20:16  cheshire
<rdar://problem/3330324> Need to check IP TTL on responses

Revision 1.100  2003/08/05 21:18:50  cheshire
<rdar://problem/3363185> mDNSResponder should ignore 6to4
Only use interfaces that are marked as multicast-capable (IFF_MULTICAST)

Revision 1.99  2003/08/05 20:13:52  cheshire
<rdar://problem/3294080> mDNSResponder using IPv6 interfaces before they are ready
Ignore interfaces with the IN6_IFF_NOTREADY flag set

Revision 1.98  2003/07/20 03:38:51  ksekar
<rdar://problem/3320722>
Completed support for Unix-domain socket based API.

Revision 1.97  2003/07/19 03:15:16  cheshire
Add generic MemAllocate/MemFree prototypes to mDNSPlatformFunctions.h,
and add the obvious trivial implementations to each platform support layer

Revision 1.96  2003/07/18 00:30:00  cheshire
<rdar://problem/3268878> Remove mDNSResponder version from packet header and use HINFO record instead

Revision 1.95  2003/07/12 03:15:20  cheshire
<rdar://problem/3324848> After SCDynamicStore notification, mDNSResponder updates
m->hostlabel even if user hasn't actually actually changed their dot-local hostname

Revision 1.94  2003/07/03 00:51:54  cheshire
<rdar://problem/3287213> When select() and recvmgs() disagree, get more info from kernel about the socket state

Revision 1.93  2003/07/03 00:09:14  cheshire
<rdar://problem/3286004> New APIs require a mDNSPlatformInterfaceIDfromInterfaceIndex() call
Additional refinement suggested by Josh: Use info->scope_id instead of if_nametoindex(info->ifa_name);

Revision 1.92  2003/07/02 21:19:51  cheshire
<rdar://problem/3313413> Update copyright notices, etc., in source code comments

Revision 1.91  2003/06/24 01:53:51  cheshire
Minor update to comments

Revision 1.90  2003/06/24 01:51:47  cheshire
<rdar://problem/3303118> Oops: Double-dispose of sockets
Don't need to close sockets: CFSocketInvalidate() does that for us

Revision 1.89  2003/06/21 18:12:47  cheshire
<rdar://problem/3296061> mDNSResponder cannot handle interfaces whose total name is >3 chars
One-line change: should say "IF_NAMESIZE", not sizeof(ifname)

Revision 1.88  2003/06/12 23:38:37  cheshire
<rdar://problem/3291162> mDNSResponder doesn't detect some configuration changes
Also check that scope_id matches before concluding that two interfaces are the same

Revision 1.87  2003/06/10 01:14:11  cheshire
<rdar://problem/3286004> New APIs require a mDNSPlatformInterfaceIDfromInterfaceIndex() call

Revision 1.86  2003/05/28 02:41:52  cheshire
<rdar://problem/3034346> Time to remove Mac OS 9 UDP Port 53 legacy support

Revision 1.85  2003/05/28 02:39:47  cheshire
Minor change to debugging messages

Revision 1.84  2003/05/27 22:29:40  cheshire
Remove out-dated comment

Revision 1.83  2003/05/26 03:21:29  cheshire
Tidy up address structure naming:
mDNSIPAddr         => mDNSv4Addr (for consistency with mDNSv6Addr)
mDNSAddr.addr.ipv4 => mDNSAddr.ip.v4
mDNSAddr.addr.ipv6 => mDNSAddr.ip.v6

Revision 1.82  2003/05/26 03:01:27  cheshire
<rdar://problem/3268904> sprintf/vsprintf-style functions are unsafe; use snprintf/vsnprintf instead

Revision 1.81  2003/05/24 02:06:42  cheshire
<rdar://problem/3268480> IPv6 Multicast Loopback doesn't work
Tried setting IPV6_MULTICAST_LOOP; it doesn't help.
However, it is probably wise to have the code explicitly set this socket
option anyway, in case the default changes in later versions of Unix.

Revision 1.80  2003/05/24 02:02:24  cheshire
<rdar://problem/3221880> if_indextoname consumes a lot of CPU
Fix error in myIfIndexToName; was returning prematurely

Revision 1.79  2003/05/23 23:07:44  cheshire
<rdar://problem/3268199> Must not write to stderr when running as daemon

Revision 1.78  2003/05/23 01:19:04  cheshire
<rdar://problem/3267085> mDNSResponder needs to signal type of service to AirPort
Mark packets as high-throughput/low-delay (i.e. lowest reliability) to get maximum 802.11 multicast rate

Revision 1.77  2003/05/23 01:12:05  cheshire
Minor code tidying

Revision 1.76  2003/05/22 01:26:01  cheshire
Tidy up log messages

Revision 1.75  2003/05/22 00:07:09  cheshire
<rdar://problem/3264366> myCFSocketCallBack recvfrom(5) error 1, errno 35
Extra logging to determine whether there is a bug in CFSocket

Revision 1.74  2003/05/21 20:20:12  cheshire
Fix warnings (mainly printf format string warnings, like using "%d" where
it should say "%lu", etc.) and improve error logging (use strerror()
to include textual error message as well as numeric error in log messages).

Revision 1.73  2003/05/21 17:56:29  ksekar
<rdar://problem/3191277>: mDNSResponder doesn't watch for IPv6 address changes

Revision 1.72  2003/05/14 18:48:41  cheshire
<rdar://problem/3159272> mDNSResponder should be smarter about reconfigurations
More minor refinements:
mDNSMacOSX.c needs to do *all* its mDNS_DeregisterInterface calls before freeing memory
mDNS_DeregisterInterface revalidates cache record when *any* representative of an interface goes away

Revision 1.71  2003/05/14 07:08:37  cheshire
<rdar://problem/3159272> mDNSResponder should be smarter about reconfigurations
Previously, when there was any network configuration change, mDNSResponder
would tear down the entire list of active interfaces and start again.
That was very disruptive, and caused the entire cache to be flushed,
and caused lots of extra network traffic. Now it only removes interfaces
that have really gone, and only adds new ones that weren't there before.

Revision 1.70  2003/05/07 18:30:24  cheshire
Fix signed/unsigned comparison warning

Revision 1.69  2003/05/06 20:14:44  cheshire
Change "tp" to "tv"

Revision 1.68  2003/05/06 00:00:49  cheshire
<rdar://problem/3248914> Rationalize naming of domainname manipulation functions

Revision 1.67  2003/04/29 00:43:44  cheshire
Fix compiler warnings

Revision 1.66  2003/04/26 02:41:58  cheshire
<rdar://problem/3241281> Change timenow from a local variable to a structure member

Revision 1.65  2003/04/26 02:34:01  cheshire
Add missing mDNSexport

Revision 1.64  2003/04/15 16:48:06  jgraessl
<rdar://problem/3228833>
Modified code in CFSocket notifier function to read all packets on the socket
instead of reading only one packet every time the notifier was called.

Revision 1.63  2003/04/15 16:33:50  jgraessl
<rdar://problem/3221880>
Switched to our own copy of if_indextoname to improve performance.

Revision 1.62  2003/03/28 01:55:44  cheshire
Minor improvements to debugging messages

Revision 1.61  2003/03/27 03:30:56  cheshire
<rdar://problem/3210018> Name conflicts not handled properly, resulting in memory corruption, and eventual crash
Problem was that HostNameCallback() was calling mDNS_DeregisterInterface(), which is not safe in a callback
Fixes:
1. Make mDNS_DeregisterInterface() safe to call from a callback
2. Make HostNameCallback() use mDNS_DeadvertiseInterface() instead
   (it never really needed to deregister the interface at all)

Revision 1.60  2003/03/15 04:40:38  cheshire
Change type called "mDNSOpaqueID" to the more descriptive name "mDNSInterfaceID"

Revision 1.59  2003/03/11 01:23:26  cheshire
<rdar://problem/3194246> mDNSResponder socket problems

Revision 1.58  2003/03/06 01:43:04  cheshire
<rdar://problem/3189097> Additional debugging code in mDNSResponder
Improve "LIST_ALL_INTERFACES" output

Revision 1.57  2003/03/05 22:36:27  cheshire
<rdar://problem/3186338> Loopback doesn't work with mDNSResponder-27
Temporary workaround: Skip loopback interface *only* if we found at least one v4 interface to use

Revision 1.56  2003/03/05 01:50:38  cheshire
<rdar://problem/3189097> Additional debugging code in mDNSResponder

Revision 1.55  2003/02/21 01:54:09  cheshire
<rdar://problem/3099194> mDNSResponder needs performance improvements
Switched to using new "mDNS_Execute" model (see "Implementer Notes.txt")

Revision 1.54  2003/02/20 06:48:35  cheshire
<rdar://problem/3169535> Xserve RAID needs to do interface-specific registrations
Reviewed by: Josh Graessley, Bob Bradley

Revision 1.53  2003/01/29 02:21:23  cheshire
Return mStatus_Invalid if can't send packet because socket not available

Revision 1.52  2003/01/28 19:39:43  jgraessl
Enabling AAAA over IPv4 support.

Revision 1.51  2003/01/28 05:11:23  cheshire
Fixed backwards comparison in SearchForInterfaceByName

Revision 1.50  2003/01/13 23:49:44  jgraessl
Merged changes for the following fixes in to top of tree:
<rdar://problem/3086540>  computer name changes not handled properly
<rdar://problem/3124348>  service name changes are not properly handled
<rdar://problem/3124352>  announcements sent in pairs, failing chattiness test

Revision 1.49  2002/12/23 22:13:30  jgraessl
Reviewed by: Stuart Cheshire
Initial IPv6 support for mDNSResponder.

Revision 1.48  2002/11/22 01:37:52  cheshire
<rdar://problem/3108426> mDNSResponder is monitoring ServiceEntities instead of InterfaceEntities

Revision 1.47  2002/09/21 20:44:51  zarzycki
Added APSL info

Revision 1.46  2002/09/19 21:25:35  cheshire
mDNS_snprintf() doesn't need to be in a separate file

Revision 1.45  2002/09/17 01:45:13  cheshire
Add LIST_ALL_INTERFACES symbol for debugging

Revision 1.44  2002/09/17 01:36:23  cheshire
Move Puma support to mDNSMacOSXPuma.c

Revision 1.43  2002/09/17 01:05:28  cheshire
Change mDNS_AdvertiseLocalAddresses to be an Init parameter instead of a global

Revision 1.42  2002/09/16 23:13:50  cheshire
Minor code tidying

 */

// ***************************************************************************
// mDNSMacOSX.c:
// Supporting routines to run mDNS on a CFRunLoop platform
// ***************************************************************************

// For debugging, set LIST_ALL_INTERFACES to 1 to display all found interfaces,
// including ones that mDNSResponder chooses not to use.
#define LIST_ALL_INTERFACES 0

// For debugging, being able to identify software versions is useful.
// Some people are concerned that this information could be exploited by hackers.
// I'm not totally convinced by that argument, but we don't want to cause our users distress,
// so for shipping code, define "NO_HINFO" to suppress the generation of HINFO records. -- SC
#define NO_HINFO 1

// For enabling AAAA records over IPv4. Setting this to 0 sends only
// A records over IPv4 and AAAA over IPv6. Setting this to 1 sends both
// AAAA and A records over both IPv4 and IPv6.
#define AAAA_OVER_V4 1

#include "mDNSEmbeddedAPI.h"          // Defines the interface provided to the client layer above
#include "DNSCommon.h"
#include "uDNS.h"
#include "mDNSMacOSX.h"               // Defines the specific types needed to run mDNS on this platform
#include "../mDNSShared/uds_daemon.h" // Defines communication interface from platform layer up to UDS daemon
#include "PlatformCommon.h"


#include <stdio.h>
#include <stdarg.h>                 // For va_list support
#include <net/if.h>
#include <net/if_types.h>			// For IFT_ETHER
#include <net/if_dl.h>
#include <sys/uio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/event.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>                   // platform support for UTC time
#include <arpa/inet.h>              // for inet_aton
#include <pthread.h>

#include <netinet/in.h>             // For IP_RECVTTL
#ifndef IP_RECVTTL
#define IP_RECVTTL 24               // bool; receive reception TTL w/dgram
#endif

#include <netinet/in_systm.h>       // For n_long, required by <netinet/ip.h> below
#include <netinet/ip.h>             // For IPTOS_LOWDELAY etc.
#include <netinet6/in6_var.h>       // For IN6_IFF_NOTREADY etc.

#ifndef NO_SECURITYFRAMEWORK
#include <Security/SecureTransport.h>
#include <Security/Security.h>
#endif /* NO_SECURITYFRAMEWORK */

#include <DebugServices.h>
#include "dnsinfo.h"

// Code contributed by Dave Heller:
// Define RUN_ON_PUMA_WITHOUT_IFADDRS to compile code that will
// work on Mac OS X 10.1, which does not have the getifaddrs call.
#define RUN_ON_PUMA_WITHOUT_IFADDRS 0
#if RUN_ON_PUMA_WITHOUT_IFADDRS
#include "mDNSMacOSXPuma.c"
#else
#include <ifaddrs.h>
#endif

#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <mach/mach_time.h>

typedef struct SearchListElem
	{
    struct SearchListElem *next;
    domainname domain;
    int flag;
    DNSQuestion BrowseQ;
    DNSQuestion DefBrowseQ;
    DNSQuestion LegacyBrowseQ;
    DNSQuestion RegisterQ;
    DNSQuestion DefRegisterQ;
    ARListElem *AuthRecs;
	} SearchListElem;


// ***************************************************************************
// Globals

static mDNSu32 clockdivisor = 0;
static mDNSBool DomainDiscoveryDisabled = mDNSfalse;

mDNSexport int KQueueFD;

#ifndef NO_SECURITYFRAMEWORK
static CFArrayRef ServerCerts;
static SecKeychainRef ServerKC;
#endif /* NO_SECURITYFRAMEWORK */

#define CONFIG_FILE "/etc/mDNSResponder.conf"
#define DYNDNS_KEYCHAIN_SERVICE "DynDNS Shared Secret"
#define SYSTEM_KEYCHAIN_PATH "/Library/Keychains/System.keychain"

#define TLS_IO_TIMEOUT 10

// ***************************************************************************
// Functions

// We only attempt to send and receive multicast packets on interfaces that are
// (a) flagged as multicast-capable
// (b) *not* flagged as point-to-point (e.g. modem)
// Typically point-to-point interfaces are modems (including mobile-phone pseudo-modems), and we don't want
// to run up the user's bill sending multicast traffic over a link where there's only a single device at the
// other end, and that device (e.g. a modem bank) is probably not answering Multicast DNS queries anyway.
#define MulticastInterface(i) ((i->ifa_flags & IFF_MULTICAST) && !(i->ifa_flags & IFF_POINTOPOINT))

#ifndef NO_CFUSERNOTIFICATION
mDNSexport void NotifyOfElusiveBug(const char *title, const char *msg)	// Both strings are UTF-8 text
	{
	static int notifyCount = 0;
	if (notifyCount) return;
	
	// If we display our alert early in the boot process, then it vanishes once the desktop appears.
	// To avoid this, we don't try to display alerts in the first three minutes after boot.
	if ((mDNSu32)(mDNSPlatformRawTime()) < (mDNSu32)(mDNSPlatformOneSecond * 180)) return;
	
	// Unless ForceAlerts is defined, we only show these bug report alerts on machines that have a 17.x.x.x address
	#if !ForceAlerts
		{
		// Determine if we're at Apple (17.*.*.*)
		extern mDNS mDNSStorage;
		NetworkInterfaceInfoOSX *i;
		for (i = mDNSStorage.p->InterfaceList; i; i = i->next)
			if (i->ifinfo.ip.type == mDNSAddrType_IPv4 && i->ifinfo.ip.ip.v4.b[0] == 17)
				break;
		if (!i) return;	// If not at Apple, don't show the alert
		}
	#endif

	LogMsg("%s", title);
	LogMsg("%s", msg);
	// Display a notification to the user
	notifyCount++;
	static const char footer[] = "(Note: This message only appears on machines with 17.x.x.x IP addresses — i.e. at Apple — not on customer machines.)";
	CFStringRef alertHeader  = CFStringCreateWithCString(NULL, title,  kCFStringEncodingUTF8);
	CFStringRef alertBody    = CFStringCreateWithCString(NULL, msg,    kCFStringEncodingUTF8);
	CFStringRef alertFooter  = CFStringCreateWithCString(NULL, footer, kCFStringEncodingUTF8);
	CFStringRef alertMessage = CFStringCreateWithFormat(NULL, NULL, CFSTR("%@\r\r%@"), alertBody, alertFooter);
	CFUserNotificationDisplayNotice(0.0, kCFUserNotificationStopAlertLevel, NULL, NULL, NULL, alertHeader, alertMessage, NULL);
	}
#else
mDNSexport void NotifyOfElusiveBug(__unused const char *title, __unused const char *msg)
	{
	}
#endif /* NO_CFUSERNOTIFICATION */

mDNSlocal struct ifaddrs* myGetIfAddrs(int refresh)
	{
	static struct ifaddrs *ifa = NULL;
	
	if (refresh && ifa)
		{
		freeifaddrs(ifa);
		ifa = NULL;
		}
	
	if (ifa == NULL) getifaddrs(&ifa);
	
	return ifa;
	}

mDNSlocal NetworkInterfaceInfoOSX *SearchForInterfaceByName(mDNS *const m, const char *ifname, int type)
	{
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		if (i->Exists && !strcmp(i->ifa_name, ifname) &&
			((AAAA_OVER_V4                                              ) ||
			 (type == AF_INET  && i->ifinfo.ip.type == mDNSAddrType_IPv4) ||
			 (type == AF_INET6 && i->ifinfo.ip.type == mDNSAddrType_IPv6))) return(i);
	return(NULL);
	}

mDNSlocal int myIfIndexToName(u_short index, char *name)
	{
	struct ifaddrs *ifa;
	for (ifa = myGetIfAddrs(0); ifa; ifa = ifa->ifa_next)
		if (ifa->ifa_addr->sa_family == AF_LINK)
			if (((struct sockaddr_dl*)ifa->ifa_addr)->sdl_index == index)
				{ strlcpy(name, ifa->ifa_name, IF_NAMESIZE); return 0; }
	return -1;
	}

mDNSexport mDNSInterfaceID mDNSPlatformInterfaceIDfromInterfaceIndex(mDNS *const m, mDNSu32 index)
	{
	NetworkInterfaceInfoOSX *i;
	if (index == kDNSServiceInterfaceIndexLocalOnly) return(mDNSInterface_LocalOnly);
	if (index == kDNSServiceInterfaceIndexAny      ) return(mDNSNULL);

	// Don't get tricked by inactive interfaces with no InterfaceID set
	for (i = m->p->InterfaceList; i; i = i->next)
		if (i->ifinfo.InterfaceID && i->scope_id == index) return(i->ifinfo.InterfaceID);

	// Not found. Make sure our interface list is up to date, then try again.
	LogOperation("InterfaceID for interface index %d not found; Updating interface list", index);
	mDNSMacOSXNetworkChanged(m);
	for (i = m->p->InterfaceList; i; i = i->next)
		if (i->ifinfo.InterfaceID && i->scope_id == index) return(i->ifinfo.InterfaceID);

	return(mDNSNULL);
	}

mDNSexport mDNSu32 mDNSPlatformInterfaceIndexfromInterfaceID(mDNS *const m, mDNSInterfaceID id)
	{
	NetworkInterfaceInfoOSX *i;
	if (id == mDNSInterface_LocalOnly) return(kDNSServiceInterfaceIndexLocalOnly);
	if (id == mDNSInterface_Any      ) return(0);

	// Don't use i->ifinfo.InterfaceID here, because we DO want to find inactive interfaces, which have no InterfaceID set
	for (i = m->p->InterfaceList; i; i = i->next)
		if ((mDNSInterfaceID)i == id) return(i->scope_id);

	// Not found. Make sure our interface list is up to date, then try again.
	LogOperation("Interface index for InterfaceID %p not found; Updating interface list", id);
	mDNSMacOSXNetworkChanged(m);
	for (i = m->p->InterfaceList; i; i = i->next)
		if ((mDNSInterfaceID)i == id) return(i->scope_id);

	return(0);
	}

mDNSlocal mDNSBool AddrRequiresPPPConnection(const struct sockaddr *addr)
	{
	mDNSBool result = mDNSfalse;
	SCNetworkConnectionFlags flags;
	SCNetworkReachabilityRef ReachRef = NULL;

	ReachRef = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, addr);
	if (!ReachRef) { LogMsg("ERROR: RequiresConnection - SCNetworkReachabilityCreateWithAddress"); goto end; }
	if (!SCNetworkReachabilityGetFlags(ReachRef, &flags)) { LogMsg("ERROR: AddrRequiresPPPConnection - SCNetworkReachabilityGetFlags"); goto end; }
	result = flags & kSCNetworkFlagsConnectionRequired;

	end:
	if (ReachRef) CFRelease(ReachRef);
	return result;
	}

// NOTE: If InterfaceID is NULL, it means, "send this packet through our anonymous unicast socket"
// NOTE: If InterfaceID is non-NULL it means, "send this packet through our port 5353 socket on the specified interface"
// OR send via our primary v4 unicast socket
mDNSexport mStatus mDNSPlatformSendUDP(const mDNS *const m, const void *const msg, const mDNSu8 *const end,
	mDNSInterfaceID InterfaceID, const mDNSAddr *dst, mDNSIPPort dstPort)
	{
	#pragma unused(m)
	
	// Note: For this platform we've adopted the convention that InterfaceIDs are secretly pointers
	// to the NetworkInterfaceInfoOSX structure that holds the active sockets. The mDNSCore code
	// doesn't know that and doesn't need to know that -- it just treats InterfaceIDs as opaque identifiers.
	NetworkInterfaceInfoOSX *info = (NetworkInterfaceInfoOSX *)InterfaceID;
	char *ifa_name = info ? info->ifa_name : "unicast";
	struct sockaddr_storage to;
	int s = -1, err;

	// Sanity check: Make sure that if we're sending a query via unicast, we're sending it using our
	// anonymous socket created for this purpose, so that we'll receive the response.
	// If we use one of the many multicast sockets bound to port 5353 then we may not receive responses reliably.
	if (info && !mDNSAddrIsDNSMulticast(dst))
		{
		const DNSMessage *const m = (DNSMessage *)msg;
		if ((m->h.flags.b[0] & kDNSFlag0_QR_Mask) == kDNSFlag0_QR_Query)
			LogMsg("mDNSPlatformSendUDP: ERROR: Sending query OP from mDNS port to non-mDNS destination %#a:%d", dst, mDNSVal16(dstPort));
		}
	
	if (dst->type == mDNSAddrType_IPv4)
		{
		struct sockaddr_in *sin_to = (struct sockaddr_in*)&to;
		sin_to->sin_len            = sizeof(*sin_to);
		sin_to->sin_family         = AF_INET;
		sin_to->sin_port           = dstPort.NotAnInteger;
		sin_to->sin_addr.s_addr    = dst->ip.v4.NotAnInteger;
		s = info ? info->ss.sktv4 : m->p->unicastsockets.sktv4;
		}
	else if (dst->type == mDNSAddrType_IPv6)
		{
		struct sockaddr_in6 *sin6_to = (struct sockaddr_in6*)&to;
		sin6_to->sin6_len            = sizeof(*sin6_to);
		sin6_to->sin6_family         = AF_INET6;
		sin6_to->sin6_port           = dstPort.NotAnInteger;
		sin6_to->sin6_flowinfo       = 0;
		sin6_to->sin6_addr           = *(struct in6_addr*)&dst->ip.v6;
		sin6_to->sin6_scope_id       = info ? info->scope_id : 0;
		s = info ? info->ss.sktv6 : m->p->unicastsockets.sktv6;
		}
	else
		{
		LogMsg("mDNSPlatformSendUDP: dst is not an IPv4 or IPv6 address!");
		return mStatus_BadParamErr;
		}

	// Don't send if it would cause dial-on-demand connection initiation.
	// As an optimization, don't bother consulting reachability API / routing
	// table when sending Multicast DNS since we ignore PPP interfaces for mDNS traffic.
	if (!mDNSAddrIsDNSMulticast(dst) && AddrRequiresPPPConnection((struct sockaddr *)&to))
		{
		debugf("mDNSPlatformSendUDP: Surpressing sending to avoid dial-on-demand connection");
		return mStatus_NoError;
		}
		
	if (s >= 0)
		verbosedebugf("mDNSPlatformSendUDP: sending on InterfaceID %p %5s/%ld to %#a:%d skt %d",
			InterfaceID, ifa_name, dst->type, dst, mDNSVal16(dstPort), s);
	else
		verbosedebugf("mDNSPlatformSendUDP: NOT sending on InterfaceID %p %5s/%ld (socket of this type not available)",
			InterfaceID, ifa_name, dst->type, dst, mDNSVal16(dstPort));

	// Note: When sending, mDNSCore may often ask us to send both a v4 multicast packet and then a v6 multicast packet
	// If we don't have the corresponding type of socket available, then return mStatus_Invalid
	if (s < 0) return(mStatus_Invalid);

	err = sendto(s, msg, (UInt8*)end - (UInt8*)msg, 0, (struct sockaddr *)&to, to.ss_len);
	if (err < 0)
		{
		static int MessageCount = 0;
        // Don't report EHOSTDOWN (i.e. ARP failure), ENETDOWN, or no route to host for unicast destinations
		if (!mDNSAddressIsAllDNSLinkGroup(dst))
			if (errno == EHOSTDOWN || errno == ENETDOWN || errno == EHOSTUNREACH || errno == ENETUNREACH) return(mStatus_TransientErr);
		// Don't report EHOSTUNREACH in the first three minutes after boot
		// This is because mDNSResponder intentionally starts up early in the boot process (See <rdar://problem/3409090>)
		// but this means that sometimes it starts before configd has finished setting up the multicast routing entries.
		if (errno == EHOSTUNREACH && (mDNSu32)(mDNSPlatformRawTime()) < (mDNSu32)(mDNSPlatformOneSecond * 180)) return(mStatus_TransientErr);
		// Don't report EADDRNOTAVAIL ("Can't assign requested address") if we're in the middle of a network configuration change
		if (errno == EADDRNOTAVAIL && m->p->NetworkChanged) return(mStatus_TransientErr);
		if (MessageCount < 1000)
			{
			MessageCount++;
			LogMsg("mDNSPlatformSendUDP sendto failed to send packet on InterfaceID %p %5s/%ld to %#a:%d skt %d error %d errno %d (%s) %lu",
				InterfaceID, ifa_name, dst->type, dst, mDNSVal16(dstPort), s, err, errno, strerror(errno), (mDNSu32)(m->timenow));
			}
		return(mStatus_UnknownErr);
		}
	
	return(mStatus_NoError);
	}

mDNSlocal ssize_t myrecvfrom(const int s, void *const buffer, const size_t max,
	struct sockaddr *const from, size_t *const fromlen, mDNSAddr *dstaddr, char ifname[IF_NAMESIZE], mDNSu8 *ttl)
	{
	static unsigned int numLogMessages = 0;
	struct iovec databuffers = { (char *)buffer, max };
	struct msghdr   msg;
	ssize_t         n;
	struct cmsghdr *cmPtr;
	char            ancillary[1024];

	*ttl = 255;  // If kernel fails to provide TTL data (e.g. Jaguar doesn't) then assume the TTL was 255 as it should be

	// Set up the message
	msg.msg_name       = (caddr_t)from;
	msg.msg_namelen    = *fromlen;
	msg.msg_iov        = &databuffers;
	msg.msg_iovlen     = 1;
	msg.msg_control    = (caddr_t)&ancillary;
	msg.msg_controllen = sizeof(ancillary);
	msg.msg_flags      = 0;
	
	// Receive the data
	n = recvmsg(s, &msg, 0);
	if (n<0)
		{
		if (errno != EWOULDBLOCK && numLogMessages++ < 100) LogMsg("mDNSMacOSX.c: recvmsg(%d) returned error %d errno %d", s, n, errno);
		return(-1);
		}
	if (msg.msg_controllen < (int)sizeof(struct cmsghdr))
		{
		if (numLogMessages++ < 100) LogMsg("mDNSMacOSX.c: recvmsg(%d) returned %d msg.msg_controllen %d < sizeof(struct cmsghdr) %lu",
			s, n, msg.msg_controllen, sizeof(struct cmsghdr));
		return(-1);
		}
	if (msg.msg_flags & MSG_CTRUNC)
		{
		if (numLogMessages++ < 100) LogMsg("mDNSMacOSX.c: recvmsg(%d) msg.msg_flags & MSG_CTRUNC", s);
		return(-1);
		}
	
	*fromlen = msg.msg_namelen;
	
	// Parse each option out of the ancillary data.
	for (cmPtr = CMSG_FIRSTHDR(&msg); cmPtr; cmPtr = CMSG_NXTHDR(&msg, cmPtr))
		{
		// debugf("myrecvfrom cmsg_level %d cmsg_type %d", cmPtr->cmsg_level, cmPtr->cmsg_type);
		if (cmPtr->cmsg_level == IPPROTO_IP && cmPtr->cmsg_type == IP_RECVDSTADDR)
			{
			dstaddr->type = mDNSAddrType_IPv4;
			dstaddr->ip.v4.NotAnInteger = *(u_int32_t*)CMSG_DATA(cmPtr);
			}
		if (cmPtr->cmsg_level == IPPROTO_IP && cmPtr->cmsg_type == IP_RECVIF)
			{
			struct sockaddr_dl *sdl = (struct sockaddr_dl *)CMSG_DATA(cmPtr);
			if (sdl->sdl_nlen < IF_NAMESIZE)
				{
				mDNSPlatformMemCopy(sdl->sdl_data, ifname, sdl->sdl_nlen);
				ifname[sdl->sdl_nlen] = 0;
				// debugf("IP_RECVIF sdl_index %d, sdl_data %s len %d", sdl->sdl_index, ifname, sdl->sdl_nlen);
				}
			}
		if (cmPtr->cmsg_level == IPPROTO_IP && cmPtr->cmsg_type == IP_RECVTTL)
			{
			*ttl = *(u_char*)CMSG_DATA(cmPtr);
			}
		if (cmPtr->cmsg_level == IPPROTO_IPV6 && cmPtr->cmsg_type == IPV6_PKTINFO)
			{
			struct in6_pktinfo *ip6_info = (struct in6_pktinfo*)CMSG_DATA(cmPtr);
			dstaddr->type = mDNSAddrType_IPv6;
			dstaddr->ip.v6 = *(mDNSv6Addr*)&ip6_info->ipi6_addr;
			myIfIndexToName(ip6_info->ipi6_ifindex, ifname);
			}
		if (cmPtr->cmsg_level == IPPROTO_IPV6 && cmPtr->cmsg_type == IPV6_HOPLIMIT)
			{
			*ttl = *(int*)CMSG_DATA(cmPtr);
			}
		}

	return(n);
	}

// On entry, context points to our KQSocketSet
// If ss->info is NULL, we received this packet on our anonymous unicast socket
// If ss->info is non-NULL, we received this packet on port 5353 on the indicated interface
mDNSlocal void myKQSocketCallBack(int s1, short filter, __unused u_int fflags, __unused intptr_t data, void *context)
	{
	const KQSocketSet *const ss = (const KQSocketSet *)context;
	mDNS *const m = ss->m;
	int err, count = 0;
	
	if (filter != EVFILT_READ)
		LogMsg("myKQSocketCallBack: Why is filter %d not EVFILT_READ (%d)?", filter, EVFILT_READ);

	if (s1 != ss->sktv4 && s1 != ss->sktv6)
		{
		LogMsg("myKQSocketCallBack: native socket %d", s1);
		LogMsg("myKQSocketCallBack: sktv4 %d", ss->sktv4);
		LogMsg("myKQSocketCallBack: sktv6 %d", ss->sktv6);
 		}

	while (1)
		{
		// NOTE: When handling multiple packets in a batch, MUST reset InterfaceID before handling each packet
		mDNSInterfaceID InterfaceID = ss->info ? ss->info->ifinfo.InterfaceID : mDNSNULL;
		mDNSAddr senderAddr, destAddr;
		mDNSIPPort senderPort, destPort = MulticastDNSPort;
		struct sockaddr_storage from;
		size_t fromlen = sizeof(from);
		char packetifname[IF_NAMESIZE] = "";
		mDNSu8 ttl;
		err = myrecvfrom(s1, &m->imsg, sizeof(m->imsg), (struct sockaddr *)&from, &fromlen, &destAddr, packetifname, &ttl);
		if (err < 0) break;
		
		count++;
		if (from.ss_family == AF_INET)
			{
			struct sockaddr_in *sin = (struct sockaddr_in*)&from;
			senderAddr.type = mDNSAddrType_IPv4;
			senderAddr.ip.v4.NotAnInteger = sin->sin_addr.s_addr;
			senderPort.NotAnInteger = sin->sin_port;
			}
		else if (from.ss_family == AF_INET6)
			{
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)&from;
			senderAddr.type = mDNSAddrType_IPv6;
			senderAddr.ip.v6 = *(mDNSv6Addr*)&sin6->sin6_addr;
			senderPort.NotAnInteger = sin6->sin6_port;
			//LogOperation("myKQSocketCallBack received IPv6 packet from %#a to %#a", &senderAddr, &destAddr);
			}
		else
			{
			LogMsg("myKQSocketCallBack from is unknown address family %d", from.ss_family);
			return;
			}

		if (mDNSAddrIsDNSMulticast(&destAddr))
			{
			// Even though we indicated a specific interface in the IP_ADD_MEMBERSHIP call, a weirdness of the
			// sockets API means that even though this socket has only officially joined the multicast group
			// on one specific interface, the kernel will still deliver multicast packets to it no matter which
			// interface they arrive on. According to the official Unix Powers That Be, this is Not A Bug.
			// To work around this weirdness, we use the IP_RECVIF option to find the name of the interface
			// on which the packet arrived, and ignore the packet if it really arrived on some other interface.
			if (!ss->info || !ss->info->Exists)
				{
				verbosedebugf("myKQSocketCallBack got multicast packet from %#a to %#a on unicast socket (Ignored)", &senderAddr, &destAddr);
				return;
				}
			else if (strcmp(ss->info->ifa_name, packetifname))
				{
				verbosedebugf("myKQSocketCallBack got multicast packet from %#a to %#a on interface %#a/%s (Ignored -- really arrived on interface %s)",
					&senderAddr, &destAddr, &ss->info->ifinfo.ip, ss->info->ifa_name, packetifname);
				return;
				}
			else
				verbosedebugf("myKQSocketCallBack got multicast packet from %#a to %#a on interface %#a/%s",
					&senderAddr, &destAddr, &ss->info->ifinfo.ip, ss->info->ifa_name);
			}
		else
			{
			// Note: Unicast packets are delivered to *one* of our listening sockets,
			// not necessarily the one bound to the physical interface where the packet arrived.
			// To sort this out we search our interface list and update InterfaceID to reference
			// the mDNSCore interface object for the interface where the packet was actually received.
			NetworkInterfaceInfo *intf = m->HostInterfaces;
			while (intf && strcmp(intf->ifname, packetifname)) intf = intf->next;
			if (intf) InterfaceID = intf->InterfaceID;
			}

		mDNSCoreReceive(m, &m->imsg, (unsigned char*)&m->imsg + err, &senderAddr, senderPort, &destAddr, destPort, InterfaceID);
		}

	if (err < 0 && (errno != EWOULDBLOCK || count == 0))
		{
		// Something is busted here.
		// kqueue says there is a packet, but myrecvfrom says there is not.
		// Try calling select() to get another opinion.
		// Find out about other socket parameter that can help understand why select() says the socket is ready for read
		// All of this is racy, as data may have arrived after the call to select()
		static unsigned int numLogMessages = 0;
		int save_errno = errno;
		int so_error = -1;
		int so_nread = -1;
		int fionread = -1;
		socklen_t solen = sizeof(int);
		fd_set readfds;
		struct timeval timeout;
		int selectresult;
		FD_ZERO(&readfds);
		FD_SET(s1, &readfds);
		timeout.tv_sec  = 0;
		timeout.tv_usec = 0;
		selectresult = select(s1+1, &readfds, NULL, NULL, &timeout);
		if (getsockopt(s1, SOL_SOCKET, SO_ERROR, &so_error, &solen) == -1)
			LogMsg("myKQSocketCallBack getsockopt(SO_ERROR) error %d", errno);
		if (getsockopt(s1, SOL_SOCKET, SO_NREAD, &so_nread, &solen) == -1)
			LogMsg("myKQSocketCallBack getsockopt(SO_NREAD) error %d", errno);
		if (ioctl(s1, FIONREAD, &fionread) == -1)
			LogMsg("myKQSocketCallBack ioctl(FIONREAD) error %d", errno);
		if (numLogMessages++ < 100)
			LogMsg("myKQSocketCallBack recvfrom skt %d error %d errno %d (%s) select %d (%spackets waiting) so_error %d so_nread %d fionread %d count %d",
				s1, err, save_errno, strerror(save_errno), selectresult, FD_ISSET(s1, &readfds) ? "" : "*NO* ", so_error, so_nread, fionread, count);
		if (numLogMessages > 5)
			NotifyOfElusiveBug("Flaw in Kernel (select/recvfrom mismatch)",
				"Congratulations, you've reproduced an elusive bug.\r"
				"Please contact the current assignee of <rdar://problem/3375328>.\r"
				"Alternatively, you can send email to radar-3387020@group.apple.com. (Note number is different.)\r"
				"If possible, please leave your machine undisturbed so that someone can come to investigate the problem.");

		sleep(1);		// After logging this error, rate limit so we don't flood syslog
		}
	}

// TCP socket support

struct uDNS_TCPSocket_struct
	{
    TCPConnectionCallback callback;
	int fd;
	KQueueEntry kqEntry;
	uDNS_TCPSocketFlags flags;
#ifndef NO_SECURITYFRAMEWORK
	SSLContextRef tlsContext;
#endif /* NO_SECURITYFRAMEWORK */
	void *context;
    mDNSBool connected;
	mDNSBool handshake;
	};

#ifndef NO_SECURITYFRAMEWORK
mDNSlocal OSStatus tlsWriteSock(SSLConnectionRef connection, const void * data, size_t * dataLength)
	{
	UInt32			bytesSent = 0;
	uDNS_TCPSocket	sock = (uDNS_TCPSocket) connection;
	int 			length;
	UInt32			dataLen = *dataLength;
	const UInt8	*	dataPtr = (UInt8 *)data;
	OSStatus		ortn;
	
	*dataLength = 0;

    do
		{
		int				selectresult;
		fd_set			fds;
		struct timeval	tv;

		FD_ZERO(&fds);
		FD_SET(sock->fd, &fds);
		tv.tv_sec = TLS_IO_TIMEOUT;
		tv.tv_usec = 0;
		
		selectresult = select(sock->fd + 1, NULL, &fds, NULL, &tv);

		if (selectresult == 1)
			{
			length = send(sock->fd, (char*) dataPtr + bytesSent, dataLen - bytesSent, 0);
			}
		else if (selectresult == 0)
			{
			length = 0;
			errno = EAGAIN;
			break;
			}
		else
			{
			length = 0;
			break;
			}
    	}
	while ((length > 0) && ((bytesSent += length) < dataLen));
	
	if (length <= 0)
		{
		if (errno == EAGAIN)
			{
			ortn = errSSLWouldBlock;
			}
		else
			{
			ortn = ioErr;
			}
		}
	else
		{
		ortn = noErr;
		}

	*dataLength = bytesSent;
	return ortn;
	}


mDNSlocal OSStatus tlsReadSock(SSLConnectionRef	connection, void * data, size_t * dataLength)
	{
	UInt32			bytesToGo = *dataLength;
	UInt32 			initLen = bytesToGo;
	UInt8		*	currData = (UInt8 *)data;
	uDNS_TCPSocket	sock = (uDNS_TCPSocket) connection;
	OSStatus		rtn = noErr;
	UInt32			bytesRead;
	int				rrtn;
	
	*dataLength = 0;

	for (;;)
		{
		int				selectresult;
		fd_set			fds;
		struct timeval	tv;

		bytesRead = 0;

		FD_ZERO(&fds);
		FD_SET(sock->fd, &fds);
		tv.tv_sec  = TLS_IO_TIMEOUT;
		tv.tv_usec = 0;

		selectresult = select(sock->fd + 1, &fds, NULL, NULL, &tv);

		if (selectresult == 1)
			{
			rrtn = recv(sock->fd, currData, bytesToGo, 0);

			if (rrtn <= 0)
				{
				switch (errno)
					{
					case ENOENT:
						/* connection closed */
						rtn = errSSLClosedGraceful;
						break;
					case ECONNRESET:
						rtn = errSSLClosedAbort;
						break;
					case EAGAIN:
						rtn = errSSLWouldBlock;
						break;
					default:
						LogMsg("ERROR: tlsSockRead: read(%d) error %d\n", (int)bytesToGo, errno);
						rtn = ioErr;
						break;
					}
				break;
				}
			else
				{
				bytesRead = rrtn;
				}

			bytesToGo -= bytesRead;
			currData  += bytesRead;
		
			if (!bytesToGo)
				{
				break;
				}
			}
		else if (selectresult == 0)
			{
			rtn = errSSLWouldBlock;
			break;
			}
		else
			{
			LogMsg("ERROR: tlsSockRead: select(%d) error %d\n", (int)bytesToGo, errno);
			rtn = ioErr;
			break;
			}
		}

	*dataLength = initLen - bytesToGo;

	return rtn;
	}


mDNSlocal OSStatus tlsSetupSock(uDNS_TCPSocket sock, mDNSBool server)
	{
	mStatus err = mStatus_NoError;

	if ((sock->flags & kTCPSocketFlags_UseTLS) == 0)
		{
		LogMsg("ERROR: tlsSetupSock: socket is not a TLS socket");
		err = mStatus_UnknownErr;
		goto exit;
		}

	err = SSLNewContext(server, &sock->tlsContext);

	if (err)
		{
		LogMsg("ERROR: tlsSetupSock: SSLNewContext failed with error code: %d", err);
		goto exit;
		}

	err = SSLSetIOFuncs(sock->tlsContext, tlsReadSock, tlsWriteSock);

	if (err)
		{
		LogMsg("ERROR: tlsSetupSock: SSLSetIOFuncs failed with error code: %d", err);
		goto exit;
		}

	err = SSLSetConnection(sock->tlsContext, (SSLConnectionRef) sock);

	if (err)
		{
		LogMsg("ERROR: tlsSetupSock: SSLSetConnection failed with error code: %d", err);
		goto exit;
		}

	exit:

	return err;
	}
#endif /* NO_SECURITYFRAMEWORK */


mDNSlocal void tcpKQSocketCallback(__unused int fd, __unused short filter, __unused u_int fflags, __unused intptr_t data, void *context)
	{
	#pragma unused(cfs, CallbackType, address, data)
	struct uDNS_TCPSocket_struct * sock = context;
	mDNSBool connect = mDNSfalse;
	mStatus err = mStatus_NoError;
	
	if (!sock->connected)
		{
		connect			= mDNStrue;
		sock->connected = mDNStrue;  // prevent connected flag from being set in future callbacks

		if (sock->flags & kTCPSocketFlags_UseTLS)
			{
#ifndef NO_SECURITYFRAMEWORK
			err = tlsSetupSock(sock, mDNSfalse);

			if (err)
				{
				LogMsg("ERROR: tcpKQSocketCallback: tlsSetupSock failed with error code: %d", err);
				goto exit;
				}

			err = SSLHandshake(sock->tlsContext);
			
			sock->handshake = mDNStrue;
			
			if (err)
				{
				LogMsg("ERROR: tcpKQSocketCallback: SSLHandshake failed with error code: %d", err);
				SSLDisposeContext(sock->tlsContext);
				sock->tlsContext = NULL;
				goto exit;
				}
#else
			err = mStatus_UnsupportedErr;
			goto exit;
#endif /* NO_SECURITYFRAMEWORK */
			}
		}

	exit:

	sock->callback(sock, sock->context, connect, err);
	// NOTE: the callback may call CloseConnection here, which frees the context structure!
	}
	

mDNSexport void KQueueWake(mDNS * const m)
	{
	char wake = 1;
	if (send(m->p->WakeKQueueLoopFD, &wake, sizeof(wake), 0) == -1)
		LogMsg("ERROR: KQueueWake: send failed with error code: %d - %s", errno, strerror(errno));
	}

mDNSexport int KQueueAdd(int fd, short filter, u_int fflags, intptr_t data, const KQueueEntry *const entryRef)
	{
	struct kevent	new_event;
	int				result = 0;
	
	EV_SET(&new_event, fd, filter, EV_ADD, fflags, data, (void*)entryRef);
	
	result = kevent(KQueueFD, &new_event, 1, NULL, 0, NULL);
	if (result == -1) return errno;
	return 0;
	}

mDNSexport uDNS_TCPSocket mDNSPlatformTCPSocket(mDNS * const m, uDNS_TCPSocketFlags flags, mDNSIPPort * port)
	{
	struct uDNS_TCPSocket_struct	*	sock = mDNSNULL;
	struct sockaddr_in				addr;
	socklen_t						len;
	int								on = 1;  // "on" for setsockopt
	mStatus							err = mStatus_NoError;

	(void) m;

	sock = mallocL("mDNSPlatformTCPSocket", sizeof(struct uDNS_TCPSocket_struct));

	if (!sock)
		{
		LogMsg("mDNSPlatformTCPSocket: memory allocation failure");
		err = mStatus_NoMemoryErr;
		goto exit;
		}

	bzero(sock, sizeof(struct uDNS_TCPSocket_struct));
	sock->flags = flags;
	sock->fd = -1;
		
	// Create the socket

	sock->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock->fd == -1)
		{
		LogMsg("mDNSPlatformTCPSocket: socket error %d errno %d (%s)", sock->fd, errno, strerror(errno));
		err = mStatus_UnknownErr;
		goto exit;
		}
		
	// Bind it
	
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = port->NotAnInteger;

	if (bind(sock->fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
		{
		LogMsg("ERROR: bind %s", strerror(errno));
		err = mStatus_UnknownErr;
		goto exit;
		}

	// Receive interface identifiers

	if (setsockopt(sock->fd, IPPROTO_IP, IP_RECVIF, &on, sizeof(on)) < 0)
		{
		LogMsg("setsockopt IP_RECVIF - %s", strerror(errno));
		err = mStatus_UnknownErr;
		goto exit;
		}
		
	memset(&addr, 0, sizeof(addr));
	len = sizeof(addr);
	
	if (getsockname(sock->fd, (struct sockaddr*) &addr, &len) < 0)
		{
		LogMsg("getsockname - %s", strerror(errno));
		err = mStatus_UnknownErr;
		goto exit;
		}

	port->NotAnInteger = addr.sin_port;

	exit:

	if (err && sock)
		{
		if (sock->fd != -1)
			{
			close(sock->fd);
			}
			
		freeL("mDNSPlatformTCPSocket", sock);
		
		sock = NULL;
		}

	return sock;
	}


mDNSexport mStatus mDNSPlatformTCPConnect(uDNS_TCPSocket sock, const mDNSAddr * dst, mDNSOpaque16 dstport, mDNSInterfaceID InterfaceID,
                                      TCPConnectionCallback callback, void * context)
	{
	struct sockaddr_in	saddr;
	mStatus				err = mStatus_NoError;

	sock->callback = callback;
	sock->context = context;

	(void) InterfaceID;	//!!!KRS use this if non-zero!!!

	if (dst->type != mDNSAddrType_IPv4)
		{
		LogMsg("ERROR: mDNSPlatformTCPConnect - attempt to connect to an IPv6 address: opperation not supported");
		return mStatus_UnknownErr;
		}

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = dstport.NotAnInteger;
	saddr.sin_len = sizeof(saddr);
	memcpy(&saddr.sin_addr, &dst->ip.v4.NotAnInteger, sizeof(saddr.sin_addr));

	// Don't send if it would cause dial-on-demand connection initiation.
	if (AddrRequiresPPPConnection((struct sockaddr *)&saddr))
		{
		debugf("mDNSPlatformTCPConnect: Surpressing sending to avoid dial-on-demand connection");
		return mStatus_UnknownErr;
		}

	sock->kqEntry.context = sock;
	sock->kqEntry.callback = tcpKQSocketCallback;

	// Watch for connect complete (write is ready)
	if (KQueueAdd(sock->fd, EVFILT_WRITE | EV_ONESHOT, 0, 0, &sock->kqEntry) == -1)
		{
		LogMsg("ERROR: mDNSPlatformTCPConnect - KQueueAdd failed");
		close(sock->fd);
		return errno;
		}

	// Watch for incoming data
	if (KQueueAdd(sock->fd, EVFILT_READ, 0, 0, &sock->kqEntry) == -1)
		{
		LogMsg("ERROR: mDNSPlatformTCPConnect - KQueueAdd failed");
		close(sock->fd); // Closing the descriptor removes all filters from the kqueue
		return errno;
 		}

	// Set non-blocking

	if (fcntl(sock->fd, F_SETFL, O_NONBLOCK) < 0)
		{
		LogMsg("ERROR: setsockopt O_NONBLOCK - %s", strerror(errno));
		return mStatus_UnknownErr;
		}
	
	// initiate connection wth peer

	if (connect(sock->fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
		{
		if (errno == EINPROGRESS)
			{
			sock->connected = mDNSfalse;
			return mStatus_ConnPending;
			}
		LogMsg("ERROR: mDNSPlatformTCPConnect - connect failed: %s", strerror(errno));
		close(sock->fd);
		return mStatus_ConnFailed;
		}
	
	sock->connected = mDNStrue;
	
	if (err == mStatus_ConnEstablished)  // manually invoke callback if connection completes
		{
		if (sock->flags & kTCPSocketFlags_UseTLS)
			{
#ifndef NO_SECURITYFRAMEWORK
			mStatus tlsErr;

			tlsErr = tlsSetupSock(sock, mDNSfalse);

			if (tlsErr)
				{
				LogMsg("ERROR: mDNSPlatformTCPConnect: tlsSetupSock failed with error code: %d", err);
				err = tlsErr;
				goto exit;
				}

			tlsErr = SSLHandshake(sock->tlsContext);
			
			sock->handshake = mDNStrue;
			
			if (tlsErr)
				{
				LogMsg("ERROR: mDNSPlatformTCPConnect: SSLHandshake failed with error code: %d", err);
				err = tlsErr;
				goto exit;
				}
#else
			err = mStatus_UnsupportedErr;
			goto exit;
#endif /* NO_SECURITYFRAMEWORK */
			}
		}
	
	exit:

	return err;
	}


mDNSexport mDNSBool mDNSPlatformTCPIsConnected(uDNS_TCPSocket sock)
	{
	return sock->connected;
	}


mDNSexport uDNS_TCPSocket mDNSPlatformTCPAccept(uDNS_TCPSocketFlags flags, int fd)
	{
	struct uDNS_TCPSocket_struct	*	sock;
	mStatus							err = mStatus_NoError;

	sock = mallocL("mDNSPlatformTCPAccept", sizeof(struct uDNS_TCPSocket_struct));
	
	if (!sock)
		{
		err = mStatus_NoMemoryErr;
		goto exit;
		}

	memset(sock, 0, sizeof(*sock));

	sock->fd = fd;
	sock->flags = flags;

	if (flags & kTCPSocketFlags_UseTLS)
		{
#ifndef NO_SECURITYFRAMEWORK
		if (!ServerCerts)
			{
			LogMsg("ERROR: mDNSPlatformTCPAccept: unable to find TLS certificates");
			err = mStatus_UnknownErr;
			goto exit;
			}

		err = tlsSetupSock(sock, mDNStrue);

		if (err)
			{
			LogMsg("ERROR: mDNSPlatformTCPAccept: tlsSetupSock failed with error code: %d", err);
			goto exit;
			}

		err = SSLSetCertificate(sock->tlsContext, ServerCerts);

		if (err)
			{
			LogMsg("ERROR: mDNSPlatformTCPAccept: SSLSetCertificate failed with error code: %d", err);
			goto exit;
			}
			
		check(!sock->handshake);
#else
		err = mStatus_UnsupportedErr;
#endif /* NO_SECURITYFRAMEWORK */
		}
	exit:

	if (err && sock)
	{
		freeL("mDNSPlatformTCPAccept", sock);
		sock = NULL;
	}

	return (uDNS_TCPSocket) sock;
	}


mDNSexport void mDNSPlatformTCPCloseConnection(uDNS_TCPSocket sock)
	{
	if (sock)
		{
#ifndef NO_SECURITYFRAMEWORK
		if (sock->tlsContext)
			{
			SSLClose(sock->tlsContext);
			SSLDisposeContext(sock->tlsContext);
			sock->tlsContext = NULL;
			}
#endif /* NO_SECURITYFRAMEWORK */
		if (sock->fd != -1)
			{
			shutdown(sock->fd, 2);
			close(sock->fd);
			sock->fd = -1;
			}

		freeL("mDNSPlatformTCPCloseConnection", sock);
		}
	}


mDNSexport long mDNSPlatformReadTCP(uDNS_TCPSocket sock, void * buf, unsigned long buflen, mDNSBool * closed)
	{
	int nread;

	*closed = mDNSfalse;

	if (sock->flags & kTCPSocketFlags_UseTLS)
		{
#ifndef NO_SECURITYFRAMEWORK
		if (!sock->handshake)
			{
			mStatus err;

			err = SSLHandshake(sock->tlsContext);

			if (err)
				{
				LogMsg("ERROR: mDNSPlatformReadTCP: SSLHandshake failed with error code: %d", err);
				}
				
			sock->handshake = mDNStrue;
			nread = 0;
			}
		else
			{
			size_t	processed;
			mStatus	err;

			err = SSLRead(sock->tlsContext, buf, buflen, &processed);

			if (!err)
				{
				nread = (int) processed;
				
				if (!nread)
					{
					*closed = mDNStrue;
					}
				}
			else if (err == errSSLClosedGraceful)
				{
				nread = 0;
				*closed = mDNStrue;
				}
			else if (err == errSSLWouldBlock)
				{
				nread = 0;
				}
			else
				{
				LogMsg("ERROR: mDNSPlatformReadTCP - SSLRead: %d", err);
				nread = -1;
				*closed = mDNStrue;
				}
			}
#else
		nread = -1;
		*closed = mDNStrue;
#endif /* NO_SECURITYFRAMEWORK */
		}
	else
		{
		nread = recv(sock->fd, buf, buflen, 0);

		if (nread < 0)
			{
			if (errno == EAGAIN)
				{
				nread = 0;  // no data available (call would block)
				}
			else
				{
				LogMsg("ERROR: mDNSPlatformReadTCP - recv: %s", strerror(errno));
				nread = -1;
				}
			}
		else if (!nread)
			{
			*closed = mDNStrue;
			}
		}

	return nread;
	}


mDNSexport long mDNSPlatformWriteTCP(uDNS_TCPSocket sock, const char * msg, unsigned long len)
	{
	int nsent;

	if (sock->flags & kTCPSocketFlags_UseTLS)
		{
#ifndef NO_SECURITYFRAMEWORK
		size_t	processed;
		mStatus	err;

		err = SSLWrite(sock->tlsContext, msg, len, &processed);

		if (!err)
			{
			nsent = (int) processed;
			}
		else if (err == errSSLWouldBlock)
			{
			nsent = 0;
			}
		else
			{
			LogMsg("ERROR: mDNSPlatformWriteTCP - SSLWrite returned %d", err);
			nsent = -1;
			}
#else
		nsent = -1;
#endif /* NO_SECURITYFRAMEWORK */
		}
	else
		{
		nsent = send(sock->fd, msg, len, 0);

		if (nsent < 0)
			{
			if (errno == EAGAIN)
				{
				nsent = 0;
				}
			else
				{
				LogMsg("ERROR: mDNSPlatformWriteTCP - send %s", strerror(errno));
				nsent = -1;
				}
			}
		}

	return nsent;
	}


mDNSexport int mDNSPlatformTCPGetFlags(uDNS_TCPSocket sock)
	{
	return sock->flags;
	}
	
	
mDNSexport int mDNSPlatformTCPGetFD(uDNS_TCPSocket sock)
	{
	return sock->fd;
	}
	

// If mDNSIPPort port is non-zero, then it's a multicast socket on the specified interface
// If mDNSIPPort port is zero, then it's a randomly assigned port number, used for sending unicast queries
mDNSlocal mStatus SetupSocket(mDNS *const m, KQSocketSet *cp, mDNSBool mcast, const mDNSAddr *ifaddr, mDNSIPPort * inPort, u_short sa_family)
	{
	const int ip_tosbits = IPTOS_LOWDELAY | IPTOS_THROUGHPUT;
	int         *s        = (sa_family == AF_INET) ? &cp->sktv4 : &cp->sktv6;
	KQueueEntry	*k        = (sa_family == AF_INET) ? &cp->kqsv4 : &cp->kqsv6;
	const int on = 1;
	const int twofivefive = 255;
	mStatus err = mStatus_NoError;
	char *errstr = mDNSNULL;
	int skt;

	mDNSIPPort port;

	if (inPort)
		{
		port = *inPort;
		}
	else
		{
		port = (mcast | m->CanReceiveUnicastOn5353) ? MulticastDNSPort : zeroIPPort;
		}

	if (*s >= 0) { LogMsg("SetupSocket ERROR: socket %d is already set", *s); return(-1); }

	// Open the socket...
	skt = socket(sa_family, SOCK_DGRAM, IPPROTO_UDP);
	if (skt < 3) { LogMsg("SetupSocket: socket error %d errno %d (%s)", skt, errno, strerror(errno)); return(skt); }

	// ... with a shared UDP port, if it's for multicast receiving
	if (port.NotAnInteger) err = setsockopt(skt, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	if (err < 0) { errstr = "setsockopt - SO_REUSEPORT"; goto fail; }

	if (sa_family == AF_INET)
		{
		// We want to receive destination addresses
		err = setsockopt(skt, IPPROTO_IP, IP_RECVDSTADDR, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IP_RECVDSTADDR"; goto fail; }
		
		// We want to receive interface identifiers
		err = setsockopt(skt, IPPROTO_IP, IP_RECVIF, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IP_RECVIF"; goto fail; }
		
		// We want to receive packet TTL value so we can check it
		err = setsockopt(skt, IPPROTO_IP, IP_RECVTTL, &on, sizeof(on));
		// We ignore errors here -- we already know Jaguar doesn't support this, but we can get by without it
		
		// Add multicast group membership on this interface, if it's for multicast receiving
		if (mcast)
			{
			struct in_addr addr = { ifaddr->ip.v4.NotAnInteger };
			struct ip_mreq imr;
			imr.imr_multiaddr.s_addr = AllDNSLinkGroup_v4.ip.v4.NotAnInteger;
			imr.imr_interface        = addr;
			err = setsockopt(skt, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr));
			if (err < 0) { errstr = "setsockopt - IP_ADD_MEMBERSHIP"; goto fail; }
			
			// Specify outgoing interface too
			err = setsockopt(skt, IPPROTO_IP, IP_MULTICAST_IF, &addr, sizeof(addr));
			if (err < 0) { errstr = "setsockopt - IP_MULTICAST_IF"; goto fail; }
			}
		
		// Send unicast packets with TTL 255
		err = setsockopt(skt, IPPROTO_IP, IP_TTL, &twofivefive, sizeof(twofivefive));
		if (err < 0) { errstr = "setsockopt - IP_TTL"; goto fail; }
		
		// And multicast packets with TTL 255 too
		err = setsockopt(skt, IPPROTO_IP, IP_MULTICAST_TTL, &twofivefive, sizeof(twofivefive));
		if (err < 0) { errstr = "setsockopt - IP_MULTICAST_TTL"; goto fail; }

		// Mark packets as high-throughput/low-delay (i.e. lowest reliability) to get maximum 802.11 multicast rate
		err = setsockopt(skt, IPPROTO_IP, IP_TOS, &ip_tosbits, sizeof(ip_tosbits));
		if (err < 0) { errstr = "setsockopt - IP_TOS"; goto fail; }

		// And start listening for packets
		struct sockaddr_in listening_sockaddr;
		listening_sockaddr.sin_family      = AF_INET;
		listening_sockaddr.sin_port        = port.NotAnInteger;
		listening_sockaddr.sin_addr.s_addr = 0; // Want to receive multicasts AND unicasts on this socket
		err = bind(skt, (struct sockaddr *) &listening_sockaddr, sizeof(listening_sockaddr));
		if (err) { errstr = "bind"; goto fail; }
		}
	else if (sa_family == AF_INET6)
		{
		// We want to receive destination addresses and receive interface identifiers
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_PKTINFO, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IPV6_PKTINFO"; goto fail; }
		
		// We want to receive packet hop count value so we can check it
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_HOPLIMIT, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IPV6_HOPLIMIT"; goto fail; }
		
		// We want to receive only IPv6 packets. Without this option we get IPv4 packets too,
		// with mapped addresses of the form 0:0:0:0:0:FFFF:xxxx:xxxx, where xxxx:xxxx is the IPv4 address
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IPV6_V6ONLY"; goto fail; }
		
		if (mcast)
			{
			// Add multicast group membership on this interface, if it's for multicast receiving
			int interface_id = if_nametoindex(cp->info->ifa_name);
			struct ipv6_mreq i6mr;
			//LogOperation("SetupSocket: v6 %#a %s %d", ifaddr, cp->info->ifa_name, interface_id);
			i6mr.ipv6mr_interface = interface_id;
			i6mr.ipv6mr_multiaddr = *(struct in6_addr*)&AllDNSLinkGroup_v6.ip.v6;
			err = setsockopt(skt, IPPROTO_IPV6, IPV6_JOIN_GROUP, &i6mr, sizeof(i6mr));
			if (err < 0) { errstr = "setsockopt - IPV6_JOIN_GROUP"; goto fail; }
			
			// Specify outgoing interface too
			err = setsockopt(skt, IPPROTO_IPV6, IPV6_MULTICAST_IF, &interface_id, sizeof(interface_id));
			if (err < 0) { errstr = "setsockopt - IPV6_MULTICAST_IF"; goto fail; }
			}
		
		// Send unicast packets with TTL 255
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &twofivefive, sizeof(twofivefive));
		if (err < 0) { errstr = "setsockopt - IPV6_UNICAST_HOPS"; goto fail; }
		
		// And multicast packets with TTL 255 too
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &twofivefive, sizeof(twofivefive));
		if (err < 0) { errstr = "setsockopt - IPV6_MULTICAST_HOPS"; goto fail; }
		
		// Note: IPV6_TCLASS appears not to be implemented on OS X right now (or indeed on ANY version of Unix?)
		#ifdef IPV6_TCLASS
		// Mark packets as high-throughput/low-delay (i.e. lowest reliability) to get maximum 802.11 multicast rate
		int tclass = IPTOS_LOWDELAY | IPTOS_THROUGHPUT; // This may not be right (since tclass is not implemented on OS X, I can't test it)
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_TCLASS, &tclass, sizeof(tclass));
		if (err < 0) { errstr = "setsockopt - IPV6_TCLASS"; goto fail; }
		#endif
		
		// Want to receive our own packets
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IPV6_MULTICAST_LOOP"; goto fail; }
		
		// And start listening for packets
		struct sockaddr_in6 listening_sockaddr6;
		bzero(&listening_sockaddr6, sizeof(listening_sockaddr6));
		listening_sockaddr6.sin6_len         = sizeof(listening_sockaddr6);
		listening_sockaddr6.sin6_family      = AF_INET6;
		listening_sockaddr6.sin6_port        = port.NotAnInteger;
		listening_sockaddr6.sin6_flowinfo    = 0;
		listening_sockaddr6.sin6_addr        = in6addr_any; // Want to receive multicasts AND unicasts on this socket
		listening_sockaddr6.sin6_scope_id    = 0;
		err = bind(skt, (struct sockaddr *) &listening_sockaddr6, sizeof(listening_sockaddr6));
		if (err) { errstr = "bind"; goto fail; }
		}
	
	fcntl(skt, F_SETFL, fcntl(skt, F_GETFL, 0) | O_NONBLOCK); // set non-blocking
	*s = skt;
	k->callback = myKQSocketCallBack;
	k->context = cp;
	KQueueAdd(*s, EVFILT_READ, 0, 0, k);
	
	return(err);

	fail:
	LogMsg("%s error %ld errno %d (%s)", errstr, err, errno, strerror(errno));
	if (!strcmp(errstr, "bind") && errno == EADDRINUSE)
		NotifyOfElusiveBug("Setsockopt SO_REUSEPORT failed",
			"Congratulations, you've reproduced an elusive bug.\r"
			"Please contact the current assignee of <rdar://problem/3814904>.\r"
			"Alternatively, you can send email to radar-3387020@group.apple.com. (Note number is different.)\r"
			"If possible, please leave your machine undisturbed so that someone can come to investigate the problem.");
	close(skt);
	return(err);
	}

mDNSexport uDNS_UDPSocket mDNSPlatformUDPSocket(mDNS * const m, mDNSIPPort port)
	{
	KQSocketSet * ss = mDNSNULL;
	mStatus err = 0;

	ss = mallocL("mDNSPlatformUDPSocket", sizeof(KQSocketSet));

	if (!ss)
		{
		LogMsg("mDNSPlatformUDPSocket: memory exhausted");
		err = mStatus_NoMemoryErr;
		goto exit;
		}

	memset(ss, 0, sizeof(KQSocketSet));

	ss->m     = m;
	ss->sktv4 = -1;
	ss->sktv6 = -1;

	err = SetupSocket(m, ss, mDNSfalse, &zeroAddr, &port, AF_INET);

	if (err)
		{
		LogMsg("mDNSPlatformUDPSocket: SetupSocket failed");
		goto exit;
		}

	exit:

	if (err && ss)
		{
		freeL("KQSocketSet", ss);
		ss = mDNSNULL;
		}

	return (uDNS_UDPSocket) ss;
	}
	
	
mDNSlocal void CloseSocketSet(KQSocketSet *ss)
	{
	if (ss->sktv4 != -1)
		{
		close(ss->sktv4);
		ss->sktv4 = -1;
		}
	if (ss->sktv6 != -1)
		{
		close(ss->sktv6);
		ss->sktv6 = -1;
		}
	}

mDNSexport void mDNSPlatformUDPClose(uDNS_UDPSocket sock)
	{
	CloseSocketSet((KQSocketSet*) sock);
	freeL("uDNS_UDPSocket", (KQSocketSet*) sock);
	}
		

#ifndef NO_SECURITYFRAMEWORK
mDNSlocal CFArrayRef GetCertChain
	(
	SecIdentityRef identity
	)
	{
	SecCertificateRef				cert			= NULL;
	SecPolicySearchRef				searchRef		= NULL;
	SecPolicyRef					policy			= NULL;
	CFArrayRef						wrappedCert		= NULL;
	SecTrustRef						trust			= NULL;
	CFArrayRef						rawCertChain	= NULL;
    CFMutableArrayRef				certChain		= NULL;
    CSSM_TP_APPLE_EVIDENCE_INFO	*	statusChain		= NULL;
	OSStatus						err				= 0;

	if (!identity)
		{
		LogMsg("getCertChain: identity is NULL");
		goto exit;
		}

	err = SecIdentityCopyCertificate(identity, &cert);

	if (err)
		{
		LogMsg("getCertChain: SecIdentityCopyCertificate() returned %d", (int) err);
		goto exit;
		}

    err = SecPolicySearchCreate(CSSM_CERT_X_509v3, &CSSMOID_APPLE_X509_BASIC, NULL, &searchRef);

    if (err)
		{
		LogMsg("getCertChain: SecPolicySearchCreate() returned %d", (int) err);
		goto exit;
		}

    err = SecPolicySearchCopyNext(searchRef, &policy);

	if (err)
		{
		LogMsg("getCertChain: SecPolicySearchCopyNext() returned %d", (int) err);
		goto exit;
		}

	wrappedCert = CFArrayCreate(NULL, (const void**) &cert, 1, &kCFTypeArrayCallBacks);

	if (!wrappedCert)
		{
		LogMsg("getCertChain: wrappedCert is NULL");
		goto exit;
		}

	err = SecTrustCreateWithCertificates(wrappedCert, policy, &trust);
	
	if (err)
		{
		LogMsg("getCertChain: SecTrustCreateWithCertificates() returned %d", (int) err);
		goto exit;
		}

	err = SecTrustEvaluate(trust, NULL);

	if (err)
		{
		LogMsg("getCertChain: SecTrustEvaluate() returned %d", (int) err);
		goto exit;
		}

	err = SecTrustGetResult(trust, NULL, &rawCertChain, &statusChain);

	if (err)
		{
		LogMsg("getCertChain: SecTrustGetResult() returned %d", (int) err);
		goto exit;
		}

	certChain = CFArrayCreateMutableCopy(NULL, 0, rawCertChain);

	if (!certChain)
		{
		LogMsg("getCertChain: certChain is NULL");
		goto exit;
		}

	// Replace the SecCertificateRef at certChain[0] with a SecIdentityRef per documentation for SSLSetCertificate
	// at http://devworld.apple.com/documentation/Security/Reference/secureTransportRef/index.html#//apple_ref/doc/uid/TP30000155

	CFArraySetValueAtIndex(certChain, 0, identity);

	// Remove root from cert chain, but keep any and all intermediate certificates that have been signed by the root
	// certificate

    if (CFArrayGetCount(certChain) > 1)
		{
		CFArrayRemoveValueAtIndex(certChain, CFArrayGetCount(certChain) - 1);
		}

	exit:

    if (searchRef)
		{
		CFRelease(searchRef);
		}

    if (policy)
		{
		CFRelease(policy);
		}

    if (wrappedCert)
		{
		CFRelease(wrappedCert);
		}

    if (trust)
		{
		CFRelease(trust);
		}

	if (rawCertChain)
		{
		CFRelease(rawCertChain);
		}

    if (certChain && err)
    	{
		CFRelease(certChain);
		certChain = NULL;
    	}

    return certChain;
	}
#endif /* NO_SECURITYFRAMEWORK */


mDNSexport mStatus mDNSPlatformTLSSetupCerts(void)
	{
#ifndef NO_SECURITYFRAMEWORK
	SecIdentityRef			identity = nil;
	SecIdentitySearchRef	srchRef = nil;
	OSStatus				err;
	
	// Pick a keychain

	err = SecKeychainOpen(SYSTEM_KEYCHAIN_PATH, &ServerKC);

	if (err)
		{
		LogMsg("ERROR: mDNSPlatformTLSSetupCerts: SecKeychainOpen returned %d", (int) err);
		goto exit;
		}

	// search for "any" identity matching specified key use
	// In this app, we expect there to be exactly one
	 
	err = SecIdentitySearchCreate(ServerKC, CSSM_KEYUSE_DECRYPT, &srchRef);

	if (err)
		{
		LogMsg("ERROR: mDNSPlatformTLSSetupCerts: SecIdentitySearchCreate returned %d", (int) err);
		goto exit;
		}

	err = SecIdentitySearchCopyNext(srchRef, &identity);

	if (err)
		{
		LogMsg("ERROR: mDNSPlatformTLSSetupCerts: SecIdentitySearchCopyNext returned %d", (int) err);
		goto exit;
		}

	if (CFGetTypeID(identity) != SecIdentityGetTypeID())
		{
		LogMsg("ERROR: mDNSPlatformTLSSetupCerts: SecIdentitySearchCopyNext CFTypeID failure");
		err = mStatus_UnknownErr;
		goto exit;
		}

	// Found one. Call getCertChain to create the correct certificate chain.

	ServerCerts = GetCertChain(identity);

	if (ServerCerts == nil)
		{
		LogMsg("ERROR: mDNSPlatformTLSSetupCerts: getCertChain error");
		err = mStatus_UnknownErr;
		goto exit;
		}

	exit:

	return err;
#else
	return mStatus_UnsupportedErr;
#endif /* NO_SECURITYFRAMEWORK */
	}


mDNSexport  void  mDNSPlatformTLSTearDownCerts(void)
	{
#ifndef NO_SECURITYFRAMEWORK
	if (ServerCerts)
		{
		CFRelease(ServerCerts);
		ServerCerts = NULL;
		}

	if (ServerKC)
		{
		CFRelease(ServerKC);
		ServerKC = NULL;
		}
#endif /* NO_SECURITYFRAMEWORK */
	}


// This gets the text of the field currently labelled "Computer Name" in the Sharing Prefs Control Panel
mDNSlocal void GetUserSpecifiedFriendlyComputerName(domainlabel *const namelabel)
	{
	CFStringEncoding encoding = kCFStringEncodingUTF8;
	CFStringRef cfs = SCDynamicStoreCopyComputerName(NULL, &encoding);
	if (cfs)
		{
		CFStringGetPascalString(cfs, namelabel->c, sizeof(*namelabel), kCFStringEncodingUTF8);
		CFRelease(cfs);
		}
	}

// This gets the text of the field currently labelled "Local Hostname" in the Sharing Prefs Control Panel
mDNSlocal void GetUserSpecifiedLocalHostName(domainlabel *const namelabel)
	{
	CFStringRef cfs = SCDynamicStoreCopyLocalHostName(NULL);
	if (cfs)
		{
		CFStringGetPascalString(cfs, namelabel->c, sizeof(*namelabel), kCFStringEncodingUTF8);
		CFRelease(cfs);
		}
	}

mDNSlocal mDNSBool DDNSSettingEnabled(CFDictionaryRef dict)
	{
	mDNSs32 val;
	CFNumberRef state = CFDictionaryGetValue(dict, CFSTR("Enabled"));
	if (!state) return mDNSfalse;
	if (!CFNumberGetValue(state, kCFNumberSInt32Type, &val)) { LogMsg("ERROR: DDNSSettingEnabled - CFNumberGetValue"); return mDNSfalse; }
	return val ? mDNStrue : mDNSfalse;
	}

mDNSlocal mStatus SetupAddr(mDNSAddr *ip, const struct sockaddr *const sa)
	{
	if (!sa) { LogMsg("SetupAddr ERROR: NULL sockaddr"); return(mStatus_Invalid); }

	if (sa->sa_family == AF_INET)
		{
		struct sockaddr_in *ifa_addr = (struct sockaddr_in *)sa;
		ip->type = mDNSAddrType_IPv4;
		ip->ip.v4.NotAnInteger = ifa_addr->sin_addr.s_addr;
		return(mStatus_NoError);
		}

	if (sa->sa_family == AF_INET6)
		{
		struct sockaddr_in6 *ifa_addr = (struct sockaddr_in6 *)sa;
		// Inside the BSD kernel they use a hack where they stuff the sin6->sin6_scope_id
		// value into the second word of the IPv6 link-local address, so they can just
		// pass around IPv6 address structures instead of full sockaddr_in6 structures.
		// Those hacked IPv6 addresses aren't supposed to escape the kernel in that form, but they do.
		// To work around this we always whack the second word of any IPv6 link-local address back to zero.
		if (IN6_IS_ADDR_LINKLOCAL(&ifa_addr->sin6_addr)) ifa_addr->sin6_addr.__u6_addr.__u6_addr16[1] = 0;
		ip->type = mDNSAddrType_IPv6;
		ip->ip.v6 = *(mDNSv6Addr*)&ifa_addr->sin6_addr;
		return(mStatus_NoError);
		}

	LogMsg("SetupAddr invalid sa_family %d", sa->sa_family);
	return(mStatus_Invalid);
	}

mDNSlocal mDNSEthAddr GetBSSID(char *ifa_name)
	{
	mDNSEthAddr eth = zeroEthAddr;
	SCDynamicStoreRef store = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder:GetBSSID"), NULL, NULL);
	if (store)
		{
		CFStringRef entityname = CFStringCreateWithFormat(NULL, NULL, CFSTR("State:/Network/Interface/%s/AirPort"), ifa_name);
		if (entityname)
			{
			CFDictionaryRef dict = SCDynamicStoreCopyValue(store, entityname);
			if (dict)
				{
				CFRange range = { 0, 6 };		// Offset, length
				CFDataRef data = CFDictionaryGetValue(dict, CFSTR("BSSID"));
				if (data && CFDataGetLength(data) == 6) CFDataGetBytes(data, range, eth.b);
				CFRelease(dict);
				}
			CFRelease(entityname);
			}
		CFRelease(store);
		}
	return(eth);
	}

// Returns pointer to newly created NetworkInterfaceInfoOSX object, or
// pointer to already-existing NetworkInterfaceInfoOSX object found in list, or
// may return NULL if out of memory (unlikely) or parameters are invalid for some reason
// (e.g. sa_family not AF_INET or AF_INET6)
mDNSlocal NetworkInterfaceInfoOSX *AddInterfaceToList(mDNS *const m, struct ifaddrs *ifa, mDNSs32 utc)
	{
	mDNSu32 scope_id  = if_nametoindex(ifa->ifa_name);
	mDNSEthAddr bssid = GetBSSID(ifa->ifa_name);

	mDNSAddr ip, mask;
	if (SetupAddr(&ip,   ifa->ifa_addr   ) != mStatus_NoError) return(NULL);
	if (SetupAddr(&mask, ifa->ifa_netmask) != mStatus_NoError) return(NULL);

	NetworkInterfaceInfoOSX **p;
	for (p = &m->p->InterfaceList; *p; p = &(*p)->next)
		if (scope_id == (*p)->scope_id && mDNSSameAddress(&ip, &(*p)->ifinfo.ip) && mDNSSameEthAddress(&bssid, &(*p)->BSSID))
			{
			debugf("AddInterfaceToList: Found existing interface %lu %.6a with address %#a at %p", scope_id, &bssid, &ip, *p);
			(*p)->Exists = mDNStrue;
			// If interface was not in getifaddrs list last time we looked, but it is now, update 'AppearanceTime' for this record
			if ((*p)->LastSeen != utc) (*p)->AppearanceTime = utc;
			return(*p);
			}

	NetworkInterfaceInfoOSX *i = (NetworkInterfaceInfoOSX *)mallocL("NetworkInterfaceInfoOSX", sizeof(*i));
	debugf("AddInterfaceToList: Making   new   interface %lu %.6a with address %#a at %p", scope_id, &bssid, &ip, i);
	if (!i) return(mDNSNULL);
	bzero(i, sizeof(NetworkInterfaceInfoOSX));
	i->ifa_name        = (char *)mallocL("NetworkInterfaceInfoOSX name", strlen(ifa->ifa_name) + 1);
	if (!i->ifa_name) { freeL("NetworkInterfaceInfoOSX", i); return(mDNSNULL); }
	strcpy(i->ifa_name, ifa->ifa_name);		// This is safe because we know we allocated i->ifa_name with sufficient space

	i->ifinfo.InterfaceID = mDNSNULL;
	i->ifinfo.ip          = ip;
	i->ifinfo.mask        = mask;
	strlcpy(i->ifinfo.ifname, ifa->ifa_name, sizeof(i->ifinfo.ifname));
	i->ifinfo.ifname[sizeof(i->ifinfo.ifname)-1] = 0;
	i->ifinfo.Advertise   = m->AdvertiseLocalAddresses;
	i->ifinfo.McastTxRx   = mDNSfalse; // For now; will be set up later at the end of UpdateInterfaceList
	
	i->next            = mDNSNULL;
	i->Exists          = mDNStrue;
	i->AppearanceTime  = utc;		// Brand new interface; AppearanceTime is now
	i->LastSeen        = utc;
	i->Flashing        = mDNSfalse;
	i->Occulting       = mDNSfalse;
	i->scope_id        = scope_id;
	i->BSSID           = bssid;
	i->sa_family       = ifa->ifa_addr->sa_family;
	i->ifa_flags       = ifa->ifa_flags;

	i->ss.m     = m;
	i->ss.info  = i;
	i->ss.sktv4 = i->ss.sktv6 = -1;
	i->ss.kqsv4.context  = i->ss.kqsv6.context  = &i->ss;
	i->ss.kqsv4.callback = i->ss.kqsv6.callback = myKQSocketCallBack;

	*p = i;
	return(i);
	}

mDNSlocal NetworkInterfaceInfoOSX *FindRoutableIPv4(mDNS *const m, mDNSu32 scope_id)
	{
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		if (i->Exists && i->scope_id == scope_id && i->ifinfo.ip.type == mDNSAddrType_IPv4)
			if (!(i->ifinfo.ip.ip.v4.b[0] == 169 && i->ifinfo.ip.ip.v4.b[1] == 254))
				return(i);
	return(mDNSNULL);
	}

mDNSlocal mStatus UpdateInterfaceList(mDNS *const m, mDNSs32 utc)
	{
	mDNSBool foundav4           = mDNSfalse;
	mDNSBool foundav6           = mDNSfalse;
	struct ifaddrs *ifa         = myGetIfAddrs(1);
	struct ifaddrs *v4Loopback  = NULL;
	struct ifaddrs *v6Loopback  = NULL;
	mDNSEthAddr PrimaryMAC      = zeroEthAddr;
	char defaultname[32];
#ifndef NO_IPV6
	int InfoSocket              = socket(AF_INET6, SOCK_DGRAM, 0);
	if (InfoSocket < 3) LogMsg("UpdateInterfaceList: InfoSocket error %d errno %d (%s)", InfoSocket, errno, strerror(errno));
#endif
	if (m->SleepState) ifa = NULL;

	while (ifa)
		{
#if LIST_ALL_INTERFACES
		if (ifa->ifa_addr->sa_family == AF_APPLETALK)
			LogMsg("UpdateInterfaceList: %5s(%d) Flags %04X Family %2d is AF_APPLETALK",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		else if (ifa->ifa_addr->sa_family == AF_LINK)
			LogMsg("UpdateInterfaceList: %5s(%d) Flags %04X Family %2d is AF_LINK",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		else if (ifa->ifa_addr->sa_family != AF_INET && ifa->ifa_addr->sa_family != AF_INET6)
			LogMsg("UpdateInterfaceList: %5s(%d) Flags %04X Family %2d not AF_INET (2) or AF_INET6 (30)",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (!(ifa->ifa_flags & IFF_UP))
			LogMsg("UpdateInterfaceList: %5s(%d) Flags %04X Family %2d Interface not IFF_UP",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (!(ifa->ifa_flags & IFF_MULTICAST))
			LogMsg("UpdateInterfaceList: %5s(%d) Flags %04X Family %2d Interface not IFF_MULTICAST",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (ifa->ifa_flags & IFF_POINTOPOINT)
			LogMsg("UpdateInterfaceList: %5s(%d) Flags %04X Family %2d Interface IFF_POINTOPOINT",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (ifa->ifa_flags & IFF_LOOPBACK)
			LogMsg("UpdateInterfaceList: %5s(%d) Flags %04X Family %2d Interface IFF_LOOPBACK",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
#endif

		if (ifa->ifa_addr->sa_family == AF_LINK)
			{
			struct sockaddr_dl *sdl = (struct sockaddr_dl *)ifa->ifa_addr;
			if (sdl->sdl_type == IFT_ETHER && sdl->sdl_alen == sizeof(PrimaryMAC) && mDNSSameEthAddress(&PrimaryMAC, &zeroEthAddr))
				mDNSPlatformMemCopy(sdl->sdl_data + sdl->sdl_nlen, PrimaryMAC.b, 6);
			}

		if (ifa->ifa_flags & IFF_UP && ifa->ifa_addr)
			if (ifa->ifa_addr->sa_family == AF_INET || ifa->ifa_addr->sa_family == AF_INET6)
				{
				if (!ifa->ifa_netmask)
					{
					mDNSAddr ip;
					SetupAddr(&ip, ifa->ifa_addr);
					LogMsg("getifaddrs: ifa_netmask is NULL for %5s(%d) Flags %04X Family %2d %#a",
						ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family, &ip);
					}
				else if (ifa->ifa_addr->sa_family != ifa->ifa_netmask->sa_family)
					{
					mDNSAddr ip;
					SetupAddr(&ip, ifa->ifa_addr);
					LogMsg("getifaddrs ifa_netmask for %5s(%d) Flags %04X Family %2d %#a has different family: %d",
						ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family, &ip, ifa->ifa_netmask->sa_family);
					}
				else
					{
					int ifru_flags6 = 0;
#ifndef NO_IPV6
					if (ifa->ifa_addr->sa_family == AF_INET6 && InfoSocket >= 0)
						{
						struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)ifa->ifa_addr;
						struct in6_ifreq ifr6;
						bzero((char *)&ifr6, sizeof(ifr6));
						strlcpy(ifr6.ifr_name, ifa->ifa_name, sizeof(ifr6.ifr_name));
						ifr6.ifr_addr = *sin6;
						if (ioctl(InfoSocket, SIOCGIFAFLAG_IN6, &ifr6) != -1)
							ifru_flags6 = ifr6.ifr_ifru.ifru_flags6;
						verbosedebugf("%s %.16a %04X %04X", ifa->ifa_name, &sin6->sin6_addr, ifa->ifa_flags, ifru_flags6);
						}
#endif
					if (!(ifru_flags6 & (IN6_IFF_NOTREADY | IN6_IFF_DETACHED | IN6_IFF_DEPRECATED | IN6_IFF_TEMPORARY)))
						{
						if (ifa->ifa_flags & IFF_LOOPBACK)
							if (ifa->ifa_addr->sa_family == AF_INET) v4Loopback = ifa;
							else                                     v6Loopback = ifa;
						else
							{
							NetworkInterfaceInfoOSX *i = AddInterfaceToList(m, ifa, utc);
							if (i && MulticastInterface(i))
								{
								if (ifa->ifa_addr->sa_family == AF_INET) foundav4 = mDNStrue;
								else                                     foundav6 = mDNStrue;
								}
							}
						}
					}
				}
		ifa = ifa->ifa_next;
		}

    // For efficiency, we don't register a loopback interface when other interfaces of that family are available
	if (!foundav4 && v4Loopback) AddInterfaceToList(m, v4Loopback, utc);
	if (!foundav6 && v6Loopback) AddInterfaceToList(m, v6Loopback, utc);

	// Now the list is complete, set the McastTxRx setting for each interface.
	// We always send and receive using IPv4.
	// To reduce traffic, we send and receive using IPv6 only on interfaces that have no routable IPv4 address.
	// Having a routable IPv4 address assigned is a reasonable indicator of being on a large configured network,
	// which means there's a good chance that most or all the other devices on that network should also have v4.
	// By doing this we lose the ability to talk to true v6-only devices on that link, but we cut the packet rate in half.
	// At this time, reducing the packet rate is more important than v6-only devices on a large configured network,
	// so we are willing to make that sacrifice.
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		if (i->Exists)
			{
			mDNSBool txrx = MulticastInterface(i) && ((i->ifinfo.ip.type == mDNSAddrType_IPv4) || !FindRoutableIPv4(m, i->scope_id));
			if (i->ifinfo.McastTxRx != txrx)
				{
				i->ifinfo.McastTxRx = txrx;
				i->Exists = 2; // State change; need to deregister and reregister this interface
				}
			}

#ifndef NO_IPV6
	if (InfoSocket >= 0) close(InfoSocket);
#endif

	mDNS_snprintf(defaultname, sizeof(defaultname), "Macintosh-%02X%02X%02X%02X%02X%02X",
		PrimaryMAC.b[0], PrimaryMAC.b[1], PrimaryMAC.b[2], PrimaryMAC.b[3], PrimaryMAC.b[4], PrimaryMAC.b[5]);

	// Set up the nice label
	domainlabel nicelabel;
	nicelabel.c[0] = 0;
	GetUserSpecifiedFriendlyComputerName(&nicelabel);
	if (nicelabel.c[0] == 0)
		{
		LogMsg("Couldn't read user-specified Computer Name; using default “%s” instead", defaultname);
		MakeDomainLabelFromLiteralString(&nicelabel, defaultname);
		}

	// Set up the RFC 1034-compliant label
	domainlabel hostlabel;
	hostlabel.c[0] = 0;
	GetUserSpecifiedLocalHostName(&hostlabel);
	if (hostlabel.c[0] == 0)
		{
		LogMsg("Couldn't read user-specified local hostname; using default “%s.local” instead", defaultname);
		MakeDomainLabelFromLiteralString(&hostlabel, defaultname);
		}

	// We use a case-sensitive comparison here because even though changing the capitalization
	// of the name alone is not significant to DNS, it's still a change from the user's point of view
	if (SameDomainLabelCS(m->p->usernicelabel.c, nicelabel.c))
		debugf("Usernicelabel (%#s) unchanged since last time; not changing m->nicelabel (%#s)", m->p->usernicelabel.c, m->nicelabel.c);
	else
		{
		debugf("Updating m->nicelabel to %#s", nicelabel.c);
		m->p->usernicelabel = m->nicelabel = nicelabel;
		}

	if (SameDomainLabelCS(m->p->userhostlabel.c, hostlabel.c))
		debugf("Userhostlabel (%#s) unchanged since last time; not changing m->hostlabel (%#s)", m->p->userhostlabel.c, m->hostlabel.c);
	else
		{
		debugf("Updating m->hostlabel to %#s", hostlabel.c);
		m->p->userhostlabel = m->hostlabel = hostlabel;
		mDNS_SetFQDN(m);
		}

	return(mStatus_NoError);
	}

mDNSlocal int CountMaskBits(mDNSAddr *mask)
	{
	int i = 0, bits = 0;
	int bytes = mask->type == mDNSAddrType_IPv4 ? 4 : mask->type == mDNSAddrType_IPv6 ? 16 : 0;
	while (i < bytes)
		{
		mDNSu8 b = mask->ip.v6.b[i++];
		while (b & 0x80) { bits++; b <<= 1; }
		if (b) return(-1);
		}
	while (i < bytes) if (mask->ip.v6.b[i++]) return(-1);
	return(bits);
	}

// returns count of non-link local V4 addresses registered
mDNSlocal int SetupActiveInterfaces(mDNS *const m, mDNSs32 utc)
	{
	NetworkInterfaceInfoOSX *i;
	int count = 0;
	for (i = m->p->InterfaceList; i; i = i->next)
		if (i->Exists)
			{
			NetworkInterfaceInfo *n = &i->ifinfo;
			NetworkInterfaceInfoOSX *primary = SearchForInterfaceByName(m, i->ifa_name, i->sa_family);
			if (!primary) LogMsg("SetupActiveInterfaces ERROR! SearchForInterfaceByName didn't find %s", i->ifa_name);
	
			if (n->InterfaceID && n->InterfaceID != (mDNSInterfaceID)primary)	// Sanity check
				{
				LogMsg("SetupActiveInterfaces ERROR! n->InterfaceID %p != primary %p", n->InterfaceID, primary);
				n->InterfaceID = mDNSNULL;
				}
	
			if (!n->InterfaceID)
				{
				// NOTE: If n->InterfaceID is set, that means we've called mDNS_RegisterInterface() for this interface,
				// so we need to make sure we call mDNS_DeregisterInterface() before disposing it.
				// If n->InterfaceID is NOT set, then we haven't registered it and we should not try to deregister it
				n->InterfaceID = (mDNSInterfaceID)primary;
				// If i->LastSeen == utc, then this is a brand-new interface, just created, or an interface that never went away.
				// If i->LastSeen != utc, then this is an old interface, previously seen, that went away for (utc - i->LastSeen) seconds.
				// If the interface is an old one that went away and came back in less than a minute, then we're in a flapping scenario.
				i->Occulting = !(i->ifa_flags & IFF_LOOPBACK) && (utc - i->LastSeen > 0 && utc - i->LastSeen < 60);
				mDNS_RegisterInterface(m, n, i->Flashing && i->Occulting);
				if (i->ifinfo.ip.type == mDNSAddrType_IPv4 &&  (i->ifinfo.ip.ip.v4.b[0] != 169 || i->ifinfo.ip.ip.v4.b[1] != 254)) count++;
				LogOperation("SetupActiveInterfaces:   Registered    %5s(%lu) %.6a InterfaceID %p %#a/%d%s%s%s",
					i->ifa_name, i->scope_id, &i->BSSID, primary, &n->ip, CountMaskBits(&n->mask),
					i->Flashing        ? " (Flashing)"  : "",
					i->Occulting       ? " (Occulting)" : "",
					n->InterfaceActive ? " (Primary)"   : "");
				}
	
			if (!n->McastTxRx)
				debugf("SetupActiveInterfaces:   No Tx/Rx on   %5s(%lu) %.6a InterfaceID %p %#a", i->ifa_name, i->scope_id, &i->BSSID, primary, &n->ip);
			else
				{
				if (i->sa_family == AF_INET && primary->ss.sktv4 == -1)
					{
					mStatus err = SetupSocket(m, &primary->ss, mDNStrue, &i->ifinfo.ip, mDNSNULL, AF_INET);
					if (err == 0) debugf("SetupActiveInterfaces:   v4 socket%2d %5s(%lu) %.6a InterfaceID %p %#a/%d",          primary->ss.sktv4, i->ifa_name, i->scope_id, &i->BSSID, n->InterfaceID, &n->ip, CountMaskBits(&n->mask));
					else          LogMsg("SetupActiveInterfaces:   v4 socket%2d %5s(%lu) %.6a InterfaceID %p %#a/%d FAILED",   primary->ss.sktv4, i->ifa_name, i->scope_id, &i->BSSID, n->InterfaceID, &n->ip, CountMaskBits(&n->mask));
					}
			
#ifndef NO_IPV6
				if (i->sa_family == AF_INET6 && primary->ss.sktv6 == -1)
					{
					mStatus err = SetupSocket(m, &primary->ss, mDNStrue, &i->ifinfo.ip, mDNSNULL, AF_INET6);
					if (err == 0) debugf("SetupActiveInterfaces:   v6 socket%2d %5s(%lu) %.6a InterfaceID %p %#a/%d",          primary->ss.sktv6, i->ifa_name, i->scope_id, &i->BSSID, n->InterfaceID, &n->ip, CountMaskBits(&n->mask));
					else          LogMsg("SetupActiveInterfaces:   v6 socket%2d %5s(%lu) %.6a InterfaceID %p %#a/%d FAILED",   primary->ss.sktv6, i->ifa_name, i->scope_id, &i->BSSID, n->InterfaceID, &n->ip, CountMaskBits(&n->mask));
					}
#endif
				}
			}
	return count;
	}

mDNSlocal void MarkAllInterfacesInactive(mDNS *const m, mDNSs32 utc)
	{
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		{
		if (i->Exists) i->LastSeen = utc;
		i->Exists = mDNSfalse;
		}
	}

// returns count of non-link local V4 addresses deregistered
mDNSlocal int ClearInactiveInterfaces(mDNS *const m, mDNSs32 utc)
	{
	// First pass:
	// If an interface is going away, then deregister this from the mDNSCore.
	// We also have to deregister it if the primary interface that it's using for its InterfaceID is going away.
	// We have to do this because mDNSCore will use that InterfaceID when sending packets, and if the memory
	// it refers to has gone away we'll crash.
	// Don't actually close the sockets or free the memory yet: When the last representative of an interface goes away
	// mDNSCore may want to send goodbye packets on that interface. (Not yet implemented, but a good idea anyway.)
	NetworkInterfaceInfoOSX *i;
	int count = 0;
	for (i = m->p->InterfaceList; i; i = i->next)
		{
		// 1. If this interface is no longer active, or its InterfaceID is changing, deregister it
		NetworkInterfaceInfoOSX *primary = SearchForInterfaceByName(m, i->ifa_name, i->sa_family);
		if (i->ifinfo.InterfaceID)
			if (i->Exists == 0 || i->Exists == 2 || i->ifinfo.InterfaceID != (mDNSInterfaceID)primary)
				{
				i->Flashing = !(i->ifa_flags & IFF_LOOPBACK) && (utc - i->AppearanceTime < 60);
				LogOperation("ClearInactiveInterfaces: Deregistering %5s(%lu) %.6a InterfaceID %p %#a/%d%s%s%s",
					i->ifa_name, i->scope_id, &i->BSSID, i->ifinfo.InterfaceID,
					&i->ifinfo.ip, CountMaskBits(&i->ifinfo.mask),
					i->Flashing               ? " (Flashing)"  : "",
					i->Occulting              ? " (Occulting)" : "",
					i->ifinfo.InterfaceActive ? " (Primary)"   : "");
				mDNS_DeregisterInterface(m, &i->ifinfo, i->Flashing && i->Occulting);
				if (i->ifinfo.ip.type == mDNSAddrType_IPv4 && (i->ifinfo.ip.ip.v4.b[0] != 169 || i->ifinfo.ip.ip.v4.b[1] != 254)) count++;
				i->ifinfo.InterfaceID = mDNSNULL;
				// NOTE: If n->InterfaceID is set, that means we've called mDNS_RegisterInterface() for this interface,
				// so we need to make sure we call mDNS_DeregisterInterface() before disposing it.
				// If n->InterfaceID is NOT set, then it's not registered and we should not call mDNS_DeregisterInterface() on it.
				}
		}

	// Second pass:
	// Now that everything that's going to deregister has done so, we can close sockets and free the memory
	NetworkInterfaceInfoOSX **p = &m->p->InterfaceList;
	while (*p)
		{
		i = *p;
		// 2. Close all our KQSockets. We'll recreate them later as necessary.
		// (We may have previously had both v4 and v6, and we may not need both any more.)
		CloseSocketSet(&i->ss);
		// 3. If no longer active, delete interface from list and free memory
		if (!i->Exists)
			{
			if (i->LastSeen == utc) i->LastSeen = utc - 1;
			mDNSBool delete = (NumCacheRecordsForInterfaceID(m, (mDNSInterfaceID)i) == 0) && (utc - i->LastSeen >= 60);
			LogOperation("ClearInactiveInterfaces: %-13s %5s(%lu) %.6a InterfaceID %p %#a/%d Age %d%s", delete ? "Deleting" : "Holding",
				i->ifa_name, i->scope_id, &i->BSSID, i->ifinfo.InterfaceID,
				&i->ifinfo.ip, CountMaskBits(&i->ifinfo.mask), utc - i->LastSeen,
				i->ifinfo.InterfaceActive ? " (Primary)" : "");
			if (delete)
				{
				*p = i->next;
				if (i->ifa_name) freeL("NetworkInterfaceInfoOSX name", i->ifa_name);
				freeL("NetworkInterfaceInfoOSX", i);
				continue;	// After deleting this object, don't want to do the "p = &i->next;" thing at the end of the loop
				}
			}
		p = &i->next;
		}
	return count;
	}

mDNSlocal mStatus GetDNSConfig(void **result)
	{
#if MDNS_NO_DNSINFO
	static int MessageShown = 0;
	if (!MessageShown) { MessageShown = 1; LogMsg("Note: Compiled without Apple-specific Split-DNS support"); }
	*result = NULL;
	return mStatus_UnsupportedErr;
#else

	*result = dns_configuration_copy();

	if (!*result)
		{
		// When running on 10.3 (build 7xxx) and earlier, we don't expect dns_configuration_copy() to succeed
		if (mDNSMacOSXSystemBuildNumber(NULL) < 8) return mStatus_UnsupportedErr;

		// On 10.4, calls to dns_configuration_copy() early in the boot process often fail.
		// Apparently this is expected behaviour -- "not a bug".
		// Accordingly, we suppress syslog messages for the first three minutes after boot.
		// If we are still getting failures after three minutes, then we log them.
		if ((mDNSu32)(mDNSPlatformRawTime()) < (mDNSu32)(mDNSPlatformOneSecond * 180)) return mStatus_NoError;

		LogMsg("GetDNSConfig: Error: dns_configuration_copy returned NULL");
		return mStatus_UnknownErr;
		}
	return mStatus_NoError;
#endif // MDNS_NO_DNSINFO
	}
	
mDNSexport void mDNSPlatformGetDNSConfig(mDNS * const m, domainname *const fqdn, domainname *const regDomain, DNameListElem **browseDomains)
	{
	char buf[MAX_ESCAPED_DOMAIN_NAME];	// Max legal C-string name, including terminating NUL

	fqdn->c[0] = 0;
	regDomain->c[0] = 0;
	buf[0] = 0;
	*browseDomains = NULL;
	
	SCDynamicStoreRef store = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder:GetUserSpecifiedDDNSConfig"), NULL, NULL);
	if (store)
		{
		CFDictionaryRef dict = SCDynamicStoreCopyValue(store, CFSTR("Setup:/Network/DynamicDNS"));
		if (dict)
			{
			CFArrayRef fqdnArray = CFDictionaryGetValue(dict, CFSTR("HostNames"));
			if (fqdnArray && CFArrayGetCount(fqdnArray) > 0)
				{
				CFDictionaryRef fqdnDict = CFArrayGetValueAtIndex(fqdnArray, 0); // for now, we only look at the first array element.  if we ever support multiple configurations, we will walk the list
				if (fqdnDict && DDNSSettingEnabled(fqdnDict))
					{
					CFStringRef name = CFDictionaryGetValue(fqdnDict, CFSTR("Domain"));
					if (name)
						{
						if (!CFStringGetCString(name, buf, sizeof(buf), kCFStringEncodingUTF8) ||
							!MakeDomainNameFromDNSNameString(fqdn, buf) || !fqdn->c[0])
							LogMsg("GetUserSpecifiedDDNSConfig SCDynamicStore bad DDNS host name: %s", buf[0] ? buf : "(unknown)");
						else debugf("GetUserSpecifiedDDNSConfig SCDynamicStore DDNS host name: %s", buf);
						}
					}
				}

			CFArrayRef regArray = CFDictionaryGetValue(dict, CFSTR("RegistrationDomains"));
			if (regArray && CFArrayGetCount(regArray) > 0)
				{
				CFDictionaryRef regDict = CFArrayGetValueAtIndex(regArray, 0);
				if (regDict && DDNSSettingEnabled(regDict))
					{
					CFStringRef name = CFDictionaryGetValue(regDict, CFSTR("Domain"));
					if (name)
						{
						if (!CFStringGetCString(name, buf, sizeof(buf), kCFStringEncodingUTF8) ||
							!MakeDomainNameFromDNSNameString(regDomain, buf) || !regDomain->c[0])
							LogMsg("GetUserSpecifiedDDNSConfig SCDynamicStore bad DDNS registration domain: %s", buf[0] ? buf : "(unknown)");
						else debugf("GetUserSpecifiedDDNSConfig SCDynamicStore DDNS registration domain: %s", buf);
						}
					}
				}
			CFArrayRef browseArray = CFDictionaryGetValue(dict, CFSTR("BrowseDomains"));
			if (browseArray)
				{
				int i;
				for (i = 0; i < CFArrayGetCount(browseArray); i++)
					{
					CFDictionaryRef browseDict = CFArrayGetValueAtIndex(browseArray, i);
					if (browseDict && DDNSSettingEnabled(browseDict))
						{
						CFStringRef name = CFDictionaryGetValue(browseDict, CFSTR("Domain"));
						if (name)
							{
							domainname dname;
							if (!CFStringGetCString(name, buf, sizeof(buf), kCFStringEncodingUTF8) ||
								!MakeDomainNameFromDNSNameString(&dname, buf) || !dname.c[0])
								LogMsg("GetUserSpecifiedDDNSConfig SCDynamicStore bad DDNS browsing domain: %s", buf[0] ? buf : "(unknown)");
							else
								{
								debugf("GetUserSpecifiedDDNSConfig SCDynamicStore DDNS browsing domain: %s", buf);
							
								DNameListElem *browseDomain = (DNameListElem*) mallocL("mDNSPlatformGetDNSConfig", sizeof(DNameListElem));
								if (!browseDomain) { LogMsg("ERROR: mDNSPlatformGetDNSConfig: memory exhausted"); continue; }
								memset(browseDomain, 0, sizeof(DNameListElem));
								AssignDomainName(&browseDomain->name, &dname);
								browseDomain->next = mDNSNULL;
								*browseDomains = browseDomain;
								browseDomains = &browseDomain->next;
								}
							}
						}
					}
				}
			CFRelease(dict);
			}
		CFRelease(store);
		}

	ReadDDNSSettingsFromConfFile(m, CONFIG_FILE, fqdn->c[0] ? NULL : fqdn, regDomain->c[0] ? NULL : regDomain, &DomainDiscoveryDisabled);
	}

// Get the list of DNS Servers

mDNSexport IPAddrListElem * mDNSPlatformGetDNSServers()
	{
	int i;
	CFArrayRef values;
	char buf[256];
	CFStringRef s;
	IPAddrListElem	*	head = NULL;
	IPAddrListElem	*	current = NULL;
	SCDynamicStoreRef	store = NULL;
	CFDictionaryRef		dict	= NULL;
	CFStringRef			key		= NULL;
	mDNSBool			ok;
	mStatus				err		= 0;

	store = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder:DynDNSConfigChanged"), NULL, NULL);
	require_action(store, exit, err = mStatus_UnknownErr);
	
	key = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetDNS);
	require_action(key, exit, err = mStatus_UnknownErr);

	dict = SCDynamicStoreCopyValue(store, key);
	require_action(dict, exit, err = mStatus_UnknownErr);

	values = CFDictionaryGetValue(dict, kSCPropNetDNSServerAddresses);

	if (values)
		{
		for (i = 0; i < CFArrayGetCount(values); i++)
			{
			mDNSAddr			addr = { mDNSAddrType_IPv4, { { { 0 } } } };
			IPAddrListElem	*	last = current;

			s = CFArrayGetValueAtIndex(values, i);
			require_action(s, exit, err = mStatus_UnknownErr);
			
			ok = CFStringGetCString(s, buf, 256, kCFStringEncodingUTF8);

			if (!ok)
				{
				LogMsg("ERROR: mDNSPlatformGetDNSServers - CFStringGetCString");
				continue;
				}

			ok = inet_aton(buf, (struct in_addr *) addr.ip.v4.b);
			
			if (!ok)
				{
				LogMsg("ERROR: mDNSPlatformGetDNSServers - invalid address string %s", buf);
				continue;
				}
				
			current = (IPAddrListElem*) mallocL("IPAddrListElem", sizeof(IPAddrListElem));
		
			if (!current)
				{
				LogMsg("ERROR: mDNSPlatformGetDNSServers - couldn't allocate memory");
				continue;
				}
				
			memset(current, 0, sizeof(IPAddrListElem));
			memcpy(&current->addr, &addr, sizeof(mDNSAddr));

			if (!head)
				{
				head = current;
				}
				
			if (last)
				{
				last->next = current;
				}
			}
		}

	exit:

	if (dict)
		{
		CFRelease(dict);
		}
		
	if (key)
		{
		CFRelease(key);
		}
		
	if (store)
		{
		CFRelease(store);
		}

	return head;
	}


// Get the search domains via OS X resolver routines or System Configuration routines
mDNSexport DNameListElem * mDNSPlatformGetSearchDomainList(void)
	{
	void			*	v				= NULL;
	char				buf[MAX_ESCAPED_DOMAIN_NAME];	// Max legal C-string name, including terminating NUL
	DNameListElem	*	head			= NULL;
	DNameListElem	*	current			= NULL;
	CFArrayRef			searchDomains	= NULL;
	SCDynamicStoreRef	store			= NULL;
	CFDictionaryRef		dict			= NULL;
	CFStringRef			key				= NULL;
	mStatus				err;
	
	err = GetDNSConfig(&v);

#if !MDNS_NO_DNSINFO
	if (!err && v)
		{
		dns_config_t   * config = NULL;
		dns_resolver_t * resolv;
		int              i;

		config = (dns_config_t*) v;

		if (!config->n_resolver)
			{
			dns_configuration_free(config);
			goto exit;
			}

		resolv = config->resolver[0];  // use the first slot for search domains
		require_action(resolv, exit, err = mStatus_UnknownErr);

		for (i = 0; i < resolv->n_search; i++)
			{
			DNameListElem * last = current;
			
			current = (DNameListElem*) mallocL("DNameListElem", sizeof(DNameListElem));
			require_action(current, exit, dns_configuration_free(config); err = mStatus_NoMemoryErr);
			memset(current, 0, sizeof(DNameListElem));
			
			if (!MakeDomainNameFromDNSNameString(&current->name, resolv->search[i]))
				{
				LogMsg("ERROR: mDNSPlatformGetSearchDomainList - MakeDomainNameFromDNSNameString");
				continue;
				}

			if (!head)
				{
				head = current;
				}
				
			if (last)
				{
				last->next = current;
				}
			}
		
		dns_configuration_free(config);
		}
#endif
	
	if (err == mStatus_UnsupportedErr)
		{
		store = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder:DynDNSConfigChanged"), NULL, NULL);
		require_action(store, exit, err = mStatus_UnknownErr);
	
		key = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetDNS);
		require_action(key, exit, err = mStatus_UnknownErr);

		dict = SCDynamicStoreCopyValue(store, key);
		require_action(dict, exit, err = mStatus_UnknownErr);
		
		searchDomains = CFDictionaryGetValue(dict, kSCPropNetDNSSearchDomains);
			
		if (searchDomains)
			{
			int i;
			
			for (i = 0; i < CFArrayGetCount(searchDomains); i++)
				{
				DNameListElem * last = current;
				CFStringRef		s;

				s = CFArrayGetValueAtIndex(searchDomains, i);
				require_action(s, exit, err = mStatus_UnknownErr);

				if (!CFStringGetCString(s, buf, MAX_ESCAPED_DOMAIN_NAME, kCFStringEncodingUTF8))
					{
					LogMsg("ERROR: mDNSPlatformGetSearchDomainList - CFStringGetCString");
					continue;
					}

				current = (DNameListElem*) mallocL("DNameListElem", sizeof(DNameListElem));
				require_action(current, exit, err = mStatus_NoMemoryErr);
				memset(current, 0, sizeof(DNameListElem));
			
				if (!MakeDomainNameFromDNSNameString(&current->name, buf))
					{
					LogMsg("ERROR: mDNSPlatformGetSearchDomainList - MakeDomainNameFromDNSNameString");
					continue;
					}

				if (!head)
					{
					head = current;
					}
				
				if (last)
					{
					last->next = current;
					}
				}
			}
		}
		
	exit:

	if (dict)
		{
		CFRelease(dict);
		}
		
	if (key)
		{
		CFRelease(key);
		}
		
	if (store)
		{
		CFRelease(store);
		}

	if (err && head)
		{
		}

	return head;
	}


// Get the search domains via OS X resolver routines or System Configuration routines
mDNSexport DNameListElem * mDNSPlatformGetFQDN(void)
	{
	void			*	v;
	char				buf[MAX_ESCAPED_DOMAIN_NAME];	// Max legal C-string name, including terminating NUL
	domainname			dname;
	DNameListElem	*	head			= NULL;
	SCDynamicStoreRef	store			= NULL;
	CFDictionaryRef		dict			= NULL;
	CFStringRef			key				= NULL;
	mDNSu8			*	ptr;
	mDNSBool			ok;
	mStatus				err;
	
	err = GetDNSConfig(&v);

#if !MDNS_NO_DNSINFO
	if (!err && v)
		{
		dns_config_t   * config = NULL;
		dns_resolver_t * resolv = NULL;
		
		config = (dns_config_t*) v;
		
		require_action(config->n_resolver, exit, dns_configuration_free(config); err = mStatus_UnknownErr);
			
		resolv = config->resolver[0];  // use the first slot for search domains
		require_action(resolv, exit, dns_configuration_free(config); err = mStatus_UnknownErr);

		if (resolv->domain)
			{
			ptr = MakeDomainNameFromDNSNameString(&dname, resolv->domain);
			require_action(ptr, exit, dns_configuration_free(config); err = mStatus_UnknownErr);

			head = (DNameListElem*) mallocL("DNameListElem", sizeof(DNameListElem));
			require_action(head, exit, dns_configuration_free(config); err = mStatus_NoMemoryErr);
			memset(head, 0, sizeof(DNameListElem));
			
			AssignDomainName(&head->name, &dname);
			}
			
		dns_configuration_free(config);
		}
#endif

	if (err == mStatus_UnsupportedErr)
		{
		store = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder:DynDNSConfigChanged"), NULL, NULL);
		require_action(store, exit, err = mStatus_UnknownErr);
	
		key = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetDNS);
		require_action(key, exit, err = mStatus_UnknownErr);

		dict = SCDynamicStoreCopyValue(store, key);
		require_action(dict, exit, err = mStatus_UnknownErr);
		
		// get DHCP domain field
		
		CFStringRef string = CFDictionaryGetValue(dict, kSCPropNetDNSDomainName);
		
		if (string)
			{
			ok = CFStringGetCString(string, buf, MAX_ESCAPED_DOMAIN_NAME, kCFStringEncodingUTF8);
			require_action(ok, exit, err = mStatus_UnknownErr);

			ptr = MakeDomainNameFromDNSNameString(&dname, buf);
			require_action(ptr, exit, err = mStatus_UnknownErr);
				
			head = (DNameListElem*) mallocL("DNameListElem", sizeof(DNameListElem));
			require_action(head, exit, err = mStatus_NoMemoryErr);
			memset(head, 0, sizeof(DNameListElem));
			
			AssignDomainName(&head->name, &dname);
			}
		}
		
	exit:

	if (dict)
		{
		CFRelease(dict);
		}
		
	if (key)
		{
		CFRelease(key);
		}
		
	if (store)
		{
		CFRelease(store);
		}

	if (err && head)
		{
		}

	return head;
	}
	
mDNSexport mStatus mDNSPlatformGetPrimaryInterface(mDNS * const m, mDNSAddr * v4, mDNSAddr * v6, mDNSAddr * r)
	{
	SCDynamicStoreRef	store	= NULL;
	CFDictionaryRef		dict	= NULL;
	CFStringRef			key		= NULL;
	CFStringRef			string	= NULL;
	int					nAdditions = 0;
	int					nDeletions = 0;
	char				buf[256];
	mStatus				err		= 0;
	
	// get IPv4 settings
	
	store = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder:DynDNSConfigChanged"), NULL, NULL);
	require_action(store, exit, err = mStatus_UnknownErr);

	key = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL,kSCDynamicStoreDomainState, kSCEntNetIPv4);
	require_action(key, exit, err = mStatus_UnknownErr);

	dict = SCDynamicStoreCopyValue(store, key);
	require_action(dict, exit, err = mStatus_UnknownErr);
	
	// handle router changes

	r->type  = mDNSAddrType_IPv4;
	r->ip.v4 = zerov4Addr;
	 
	string = CFDictionaryGetValue(dict, kSCPropNetIPv4Router);

	if (string)
		{
		struct sockaddr_in saddr;
		
		if (!CFStringGetCString(string, buf, 256, kCFStringEncodingUTF8))
			{
			LogMsg("Could not convert router to CString");
			}
		else
			{
			saddr.sin_len = sizeof(saddr);
			saddr.sin_family = AF_INET;
			saddr.sin_port = 0;
			inet_aton(buf, &saddr.sin_addr);
			
			if (AddrRequiresPPPConnection((struct sockaddr *)&saddr))
				{
				debugf("Ignoring router %s (requires PPP connection)", buf);
				}
			else
				{
				*(in_addr_t *)&r->ip.v4 = saddr.sin_addr.s_addr;
				}
			}
		}

	// handle primary interface changes
	// if we gained or lost DNS servers (e.g. logged into VPN) "toggle" primary address so it gets re-registered even if it is unchanged
	if (nAdditions || nDeletions) mDNS_SetPrimaryInterfaceInfo(m, NULL, NULL, NULL);
	
	string = CFDictionaryGetValue(dict, kSCDynamicStorePropNetPrimaryInterface);
	
	if (string)
		{
		mDNSBool HavePrimaryGlobalv6 = mDNSfalse;  // does the primary interface have a global v6 address?
		struct ifaddrs *ifa = myGetIfAddrs(1);
		
		*v4 = *v6 = zeroAddr;

		if (!CFStringGetCString(string, buf, 256, kCFStringEncodingUTF8))
			{
			LogMsg("Could not convert router to CString");
			goto exit;
			}

		// find primary interface in list
		while (ifa && (!v4->ip.v4.NotAnInteger || !HavePrimaryGlobalv6))
			{
			mDNSAddr tmp6 = zeroAddr;
			if (!strcmp(buf, ifa->ifa_name))
				{
				if      (ifa->ifa_addr->sa_family == AF_INET) SetupAddr(v4, ifa->ifa_addr);
				else if (ifa->ifa_addr->sa_family == AF_INET6)
					{
					SetupAddr(&tmp6, ifa->ifa_addr);
					if (tmp6.ip.v6.b[0] >> 5 == 1)   // global prefix: 001
						{ HavePrimaryGlobalv6 = mDNStrue; *v6 = tmp6; }
					}
				}
			else
				{
				// We'll take a V6 address from the non-primary interface if the primary interface doesn't have a global V6 address
				if (!HavePrimaryGlobalv6 && ifa->ifa_addr->sa_family == AF_INET6 && !v6->ip.v6.b[0])
					{
					SetupAddr(&tmp6, ifa->ifa_addr);
					if (tmp6.ip.v6.b[0] >> 5 == 1) *v6 = tmp6;
					}
				}
			ifa = ifa->ifa_next;
			}

		// Note that while we advertise v6, we still require v4 (possibly NAT'd, but not link-local) because we must use
		// V4 to communicate w/ our DNS server

#ifdef _LEGACY_NAT_TRAVERSAL_
		if ((v4->ip.v4.b[0] != 169 || v4->ip.v4.b[1] != 254)							&&
			 (v4->ip.v4.NotAnInteger != m->AdvertisedV4.ip.v4.NotAnInteger	||
			   memcmp(v6->ip.v6.b, m->AdvertisedV6.ip.v6.b, 16)				||
			   r->ip.v4.NotAnInteger != m->Router.ip.v4.NotAnInteger))
			{
			static mDNSBool LegacyNATInitialized = mDNSfalse;
			
			if (LegacyNATInitialized) { LNT_Destroy(); LegacyNATInitialized = mDNSfalse; }
			if (r->ip.v4.NotAnInteger && IsPrivateV4Addr(v4))
				{
				mStatus err = LNT_Init();
				if (err)  LogMsg("ERROR: LNT_Init");
				else LegacyNATInitialized = mDNStrue;
				}
			}
#endif
		}

	exit:

	if (dict)
		{
		CFRelease(dict);
		}
		
	if (key)
		{
		CFRelease(key);
		}
		
	if (store)
		{
		CFRelease(store);
		}

	return err;
	}


mDNSexport DNameListElem * mDNSPlatformGetReverseMapSearchDomainList(void)
	{
	DNameListElem	*	head	= NULL;
	DNameListElem	*	current	= NULL;
	struct ifaddrs	*	ifa		= NULL;
	mStatus				err     = 0;

	// Construct reverse-map search domains

	ifa = myGetIfAddrs(1);
	
	while (ifa)
		{
		mDNSAddr addr;

		if (ifa->ifa_addr->sa_family == AF_INET	&&
		    !SetupAddr(&addr, ifa->ifa_addr)	&&
		    !IsPrivateV4Addr(&addr)				&&
		    !(ifa->ifa_flags & IFF_LOOPBACK)	&&
			ifa->ifa_netmask)
			{
			mDNSAddr	netmask;
			char		buffer[256];

			if (!SetupAddr(&netmask, ifa->ifa_netmask))
				{
				domainname			dname;
				DNameListElem	*	last = current;
				mDNSu8			*	ptr;

				sprintf(buffer, "%d.%d.%d.%d.in-addr.arpa.", addr.ip.v4.b[3] & netmask.ip.v4.b[3],
				                                             addr.ip.v4.b[2] & netmask.ip.v4.b[2],
				                                             addr.ip.v4.b[1] & netmask.ip.v4.b[1],
				                                             addr.ip.v4.b[0] & netmask.ip.v4.b[0]);
			
				ptr = MakeDomainNameFromDNSNameString(&dname, buffer);

				if (!ptr)
					{
					LogMsg("ERROR: mDNSPlatformGetReverseMapSearchDomainList - MakeDomainNameFromDNSNameString");
					continue;
					}

				current = (DNameListElem*) mallocL("DNameListElem", sizeof(DNameListElem));
				require_action(current, exit, err = mStatus_NoMemoryErr);
				memset(current, 0, sizeof(DNameListElem));

				AssignDomainName(&current->name, &dname);

				if (!head)
					{
					head = current;
					}
				
				if (last)
					{
					last->next = current;
					}
				}
			}

			ifa = ifa->ifa_next;
		}

	exit:

	if (err && head)
		{
		mDNS_FreeDNameList(head);
		head = NULL;
		}

	return head;
	}

	
mDNSexport void mDNSPlatformDynDNSHostNameStatusChanged(domainname *const dname, mStatus status)
	{
	SCDynamicStoreRef store = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder:SetDDNSNameStatus"), NULL, NULL);
	if (store)
		{
		char uname[MAX_ESCAPED_DOMAIN_NAME];	// Max legal C-string name, including terminating NUL
		char *p;

		ConvertDomainNameToCString(dname, uname);
		p = uname;

		while (*p)
			{
			*p = tolower(*p);
			if (!(*(p+1)) && *p == '.') *p = 0; // if last character, strip trailing dot
			p++;
			}

		// We need to make a CFDictionary called "State:/Network/DynamicDNS" containing (at present) a single entity.
		// That single entity is a CFDictionary with name "HostNames".
		// The "HostNames" CFDictionary contains a set of name/value pairs, where the each name is the FQDN
		// in question, and the corresponding value is a CFDictionary giving the state for that FQDN.
		// (At present we only support a single FQDN, so this dictionary holds just a single name/value pair.)
		// The CFDictionary for each FQDN holds (at present) a single name/value pair,
		// where the name is "Status" and the value is a CFNumber giving an errror code (with zero meaning success).

			{
			const CFStringRef StateKeys [1] = { CFSTR("HostNames") };
			const CFStringRef HostKeys  [1] = { CFStringCreateWithCString(NULL, uname, kCFStringEncodingUTF8) };
			const CFStringRef StatusKeys[1] = { CFSTR("Status") };
			if (!HostKeys[0]) LogMsg("SetDDNSNameStatus: CFStringCreateWithCString(%s) failed", uname);
			else
				{
				const CFNumberRef StatusVals[1] = { CFNumberCreate(NULL, kCFNumberSInt32Type, &status) };
				if (!StatusVals[0]) LogMsg("SetDDNSNameStatus: CFNumberCreate(%ld) failed", status);
				else
					{
					const CFDictionaryRef HostVals[1] = { CFDictionaryCreate(NULL, (void*)StatusKeys, (void*)StatusVals, 1, NULL, NULL) };
					if (HostVals[0])
						{
						const CFDictionaryRef StateVals[1] = { CFDictionaryCreate(NULL, (void*)HostKeys, (void*)HostVals, 1, NULL, NULL) };
						if (StateVals[0])
							{
							CFDictionaryRef StateDict = CFDictionaryCreate(NULL, (void*)StateKeys, (void*)StateVals, 1, NULL, NULL);
							if (StateDict)
								{
								SCDynamicStoreSetValue(store, CFSTR("State:/Network/DynamicDNS"), StateDict);
								CFRelease(StateDict);
								}
							CFRelease(StateVals[0]);
							}
						CFRelease(HostVals[0]);
						}
					CFRelease(StatusVals[0]);
					}
				CFRelease(HostKeys[0]);
				}
			CFRelease(store);
			}
		}
	}
	
mDNSexport mStatus mDNSPlatformRegisterSplitDNS(mDNS *m, int * nAdditions, int * nDeletions)
	{
	(void)m;  // unused on 10.3 systems
	void *v;
	*nAdditions = *nDeletions = 0;
	mStatus err = GetDNSConfig(&v);

#if !MDNS_NO_DNSINFO
	if (!err && v)
		{
		int i;
		DNSServer *p;
		dns_config_t *config = v;  // use void * to allow compilation on 10.3 systems
		mDNS_Lock(m);
		p = m->Servers;
		while (p) { p->del = mDNStrue; p = p->next; }  // mark all for deletion
		
		LogOperation("RegisterSplitDNS: Registering %d resolvers", config->n_resolver);
		for (i = 0; i < config->n_resolver; i++)
			{
			int j, n;
			domainname d;
			dns_resolver_t *r = config->resolver[i];
			if (r->port == MulticastDNSPort.NotAnInteger) continue; // ignore configurations for .local
			if (r->search_order == DEFAULT_SEARCH_ORDER || !r->domain || !*r->domain) d.c[0] = 0; // we ignore domain for "default" resolver
			else if (!MakeDomainNameFromDNSNameString(&d, r->domain)) { LogMsg("RegisterSplitDNS: bad domain %s", r->domain); continue; }
			
			// check if this is the lowest-weighted server for the domain
			for (j = 0; j < config->n_resolver; j++)
				{
				dns_resolver_t *p = config->resolver[j];
				if (p->port == MulticastDNSPort.NotAnInteger) continue;
				if (p->search_order <= r->search_order)
					{
					domainname tmp;
					if (p->search_order == DEFAULT_SEARCH_ORDER || !p->domain || !*p->domain) tmp.c[0] = '\0';
					else if (!MakeDomainNameFromDNSNameString(&tmp, p->domain)) { LogMsg("RegisterSplitDNS: bad domain %s", p->domain); continue; }
					if (SameDomainName(&d, &tmp))
						if (p->search_order < r->search_order || j < i) break;  // if equal weights, pick first in list, otherwise pick lower-weight (p)
					}
				}
			if (j < config->n_resolver) // found a lower-weighted resolver for this domain
				{ debugf("Rejecting DNS server in slot %d domain %##s (slot %d outranks)", i, d.c, j); continue; }
			// we're using this resolver - find the first IPv4 address
			for (n = 0; n < r->n_nameserver; n++)
				{
				if (r->nameserver[n]->sa_family == AF_INET && !AddrRequiresPPPConnection(r->nameserver[n]))
					{
					// %%% This should use mDNS_AddDNSServer() instead of duplicating functionality here
					mDNSAddr saddr;
					if (SetupAddr(&saddr, r->nameserver[n])) { LogMsg("RegisterSplitDNS: bad IP address"); continue; }
					// mDNSAddr saddr = { mDNSAddrType_IPv4, { { { 192, 168, 1, 1 } } } }; // for testing
					debugf("Adding dns server from slot %d %d.%d.%d.%d for domain %##s", i, saddr.ip.v4.b[0], saddr.ip.v4.b[1], saddr.ip.v4.b[2], saddr.ip.v4.b[3], d.c);
					p = m->Servers;
					while (p)
						{
						if (mDNSSameAddress(&p->addr, &saddr) && SameDomainName(&p->domain, &d)) { p->del = mDNSfalse; break; }
						else p = p->next;
						}
					if (!p)
						{
						p = mallocL("DNSServer", sizeof(*p));
						if (!p) { LogMsg("Error: couldn't allocate memory for DNSServer");  mDNS_Unlock(m); return mStatus_UnknownErr; }
						p->addr      = saddr;
						p->del       = mDNSfalse;
						p->teststate = DNSServer_Untested;
						AssignDomainName(&p->domain, &d);
						p->next = m->Servers;
						m->Servers = p;
						(*nAdditions)++;
						}
					break;  // !!!KRS if we ever support round-robin servers, don't break here
					}
				}
			}

		// remove all servers marked for deletion
		DNSServer **s = &m->Servers;
		while (*s)
			{
			if ((*s)->del)
				{
				p = *s;
				*s = (*s)->next;
				freeL("DNSServer", p);
				(*nDeletions)--;
				}
			else s = &(*s)->next;
			}
		mDNS_Unlock(m);
		dns_configuration_free(config);
		}
#endif

	return err;
	}
	
mDNSexport void mDNSPlatformSetSecretForDomain(mDNS *m, const domainname *domain)
	{
#ifndef NO_SECURITYFRAMEWORK
	OSStatus err = 0;
	char dstring[4 + MAX_ESCAPED_DOMAIN_NAME];  // Extra 4 is for the "dns:" prefix
	domainname *d, canon;
	int i, dlen;
	mDNSu32 type = 'ddns';
	mDNSu32 typelen = sizeof(type);
	char *failedfn = "(none)";
	
	err = SecKeychainSetPreferenceDomain(kSecPreferencesDomainSystem);
	if (err) { failedfn = "SecKeychainSetPreferenceDomain"; goto cleanup; }
	
	// canonicalize name by converting to lower case (keychain and some name servers are case sensitive)
	ConvertDomainNameToCString(domain, dstring);
	dlen = strlen(dstring);
	for (i = 0; i < dlen; i++) dstring[i] = tolower(dstring[i]);  // canonicalize -> lower case
	MakeDomainNameFromDNSNameString(&canon, dstring);
	d = &canon;
	
	// find longest-match key, excluding last label (e.g. excluding ".com")
	while (d->c[0] && *(d->c + d->c[0] + 1))
		{
		snprintf(dstring, sizeof(dstring), "dns:");
		if (!ConvertDomainNameToCString(d, dstring+4)) { LogMsg("SetSecretForDomain: bad domain %##s", d->c); return; }
		dlen = strlen(dstring);
		if (dstring[dlen-1] == '.') { dstring[dlen-1] = '\0'; dlen--; }  // chop trailing dot

		SecKeychainAttribute attrs[] = { { kSecServiceItemAttr, strlen(dstring), dstring } };
		SecKeychainAttributeList attributes = { sizeof(attrs) / sizeof(attrs[0]), attrs };
		SecKeychainSearchRef searchRef;

		err = SecKeychainSearchCreateFromAttributes(NULL, kSecGenericPasswordItemClass, &attributes, &searchRef);
		if (err) { failedfn = "SecKeychainSearchCreateFromAttributes"; goto cleanup; }

		SecKeychainItemRef itemRef = NULL;
		err = SecKeychainSearchCopyNext(searchRef, &itemRef);
		
		// If no keys exist, search again for keys in the old keychain format
		if (err)
			{
			CFRelease(searchRef);
			SecKeychainAttribute attrs[] = { { kSecServiceItemAttr, strlen(dstring)-4, dstring+4 },
                                             { kSecTypeItemAttr, typelen, (UInt32 *)&type } };
			SecKeychainAttributeList attributes = { sizeof(attrs) / sizeof(attrs[0]), attrs };

			err = SecKeychainSearchCreateFromAttributes(NULL, kSecGenericPasswordItemClass, &attributes, &searchRef);
			if (err) { failedfn = "SecKeychainSearchCreateFromAttributes"; goto cleanup; }
			
			err = SecKeychainSearchCopyNext(searchRef, &itemRef);
			}

		if (!err)
			{
			UInt32 tag = kSecAccountItemAttr;
			SecKeychainAttributeInfo attrInfo = { 1, &tag, NULL };
			SecKeychainAttributeList *attrList = NULL;
			UInt32 secretlen;
			void *secret = NULL;
			
			err = SecKeychainItemCopyAttributesAndData(itemRef,  &attrInfo, NULL, &attrList, &secretlen, &secret);
			if (err || !attrList) { failedfn = "SecKeychainItemCopyAttributesAndData"; goto cleanup; }
			if (!secretlen || !secret) { LogMsg("SetSecretForDomain - bad shared secret"); return; }
			if (((char *)secret)[secretlen-1]) { LogMsg("SetSecretForDomain - Shared secret not NULL-terminated"); goto cleanup; }
			
			mDNSu32 i;
			for (i = 0; i < attrList->count; i++)
				{
				SecKeychainAttribute attr = attrList->attr[i];
				if (attr.tag == kSecAccountItemAttr)
					{
					char keybuf[MAX_ESCAPED_DOMAIN_NAME];	// Max legal C-string name, including terminating NUL
					domainname keyname;
					if (!attr.length || attr.length > MAX_ESCAPED_DOMAIN_NAME) { LogMsg("SetSecretForDomain - Bad key length %d", attr.length); goto cleanup; }
					memcpy(keybuf, attr.data, attr.length);
					keybuf[attr.length] = 0;
					if (!MakeDomainNameFromDNSNameString(&keyname, keybuf)) { LogMsg("SetSecretForDomain - bad key %s", keybuf); goto cleanup; }
					debugf("Setting shared secret for zone %s with key %##s", dstring, keyname.c);
					mDNS_SetSecretForZone(m, d, &keyname, secret);
					break;
					}
				}
			if (i == attrList->count) LogMsg("SetSecretForDomain - no key name set");
			SecKeychainItemFreeAttributesAndData(attrList, secret);
			CFRelease(itemRef);
			CFRelease(searchRef);
			goto cleanup;
			}
		else if (err == errSecItemNotFound) d = (domainname *)(d->c + d->c[0] + 1);
		else { failedfn = "SecKeychainSearchCopyNext"; goto cleanup; }
		}

	cleanup:
	if (err && err != errSecItemNotFound) LogMsg("Error: SetSecretForDomain - %s failed with error code %d", failedfn, err);
#else
	(void)m; (void)domain;
	LogMsg("Error: SetSecretForDomain - no keychain support");
#endif /* NO_SECURITYFRAMEWORK */
	}

mDNSexport void mDNSMacOSXNetworkChanged(mDNS *const m)
   {
	LogOperation("***   Network Configuration Change   ***");
	m->p->NetworkChanged = 0;		// If we received a network change event and deferred processing, we're now dealing with it
	mDNSs32 utc = mDNSPlatformUTC();
	MarkAllInterfacesInactive(m, utc);
	UpdateInterfaceList(m, utc);
	int nDeletions = ClearInactiveInterfaces(m, utc);
	int nAdditions = SetupActiveInterfaces(m, utc);
	uDNS_SetupDNSConfig(m);                           // note - call DynDNSConfigChanged *before* mDNS_UpdateLLQs
	if (nDeletions || nAdditions) mDNS_UpdateLLQs(m); // so that LLQs are restarted against the up to date name servers
	
	if (m->MainCallback)
		m->MainCallback(m, mStatus_ConfigChanged);
	}

mDNSlocal void NetworkChanged(SCDynamicStoreRef store, CFArrayRef changedKeys, void *context)
	{
	(void)store;        // Parameter not used
	(void)changedKeys;  // Parameter not used
	mDNS *const m = (mDNS *const)context;
	pthread_mutex_lock(&m->p->BigMutex);
	mDNS_Lock(m);

	mDNSs32 delay = mDNSPlatformOneSecond * 2;							// Start off assuming a two-second delay

	int c = CFArrayGetCount(changedKeys);								// Count changes
	CFRange range = { 0, c };
	CFStringRef k1 = SCDynamicStoreKeyCreateComputerName(NULL);
	CFStringRef k2 = SCDynamicStoreKeyCreateHostNames(NULL);
	if (k1 && k2)
		{
		int c1 = (CFArrayContainsValue(changedKeys, range, k1) != 0);	// See if ComputerName changed
		int c2 = (CFArrayContainsValue(changedKeys, range, k2) != 0);	// See if Local Hostname changed
		int c3 = (CFArrayContainsValue(changedKeys, range, CFSTR("Setup:/Network/DynamicDNS")) != 0);
		if (c && c - c1 - c2 - c3 == 0) delay = mDNSPlatformOneSecond/10;	// If these were the only changes, shorten delay
		}
	if (k1) CFRelease(k1);
	if (k2) CFRelease(k2);

	LogOperation("***   NetworkChanged   *** %d change%s, delay %d", c, c>1?"s":"", delay);

	if (!m->p->NetworkChanged ||
		m->p->NetworkChanged - NonZeroTime(m->timenow + delay) < 0)
		m->p->NetworkChanged = NonZeroTime(m->timenow + delay);
	
	if (!m->SuppressSending ||
		m->SuppressSending - m->p->NetworkChanged < 0)
		m->SuppressSending = m->p->NetworkChanged;
	mDNS_Unlock(m);
	pthread_mutex_unlock(&m->p->BigMutex);
	KQueueWake(m);
	}

mDNSlocal mStatus WatchForNetworkChanges(mDNS *const m)
	{
	mStatus err = -1;
	SCDynamicStoreContext context = { 0, m, NULL, NULL, NULL };
	SCDynamicStoreRef     store    = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder:WatchForNetworkChanges"), NetworkChanged, &context);
	CFStringRef           key1     = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetIPv4);
	CFStringRef           key2     = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetIPv6);
	CFStringRef           key3     = SCDynamicStoreKeyCreateComputerName(NULL);
	CFStringRef           key4     = SCDynamicStoreKeyCreateHostNames(NULL);
	CFStringRef           key5     = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetDNS);
	CFStringRef           pattern1 = SCDynamicStoreKeyCreateNetworkInterfaceEntity(NULL, kSCDynamicStoreDomainState, kSCCompAnyRegex, kSCEntNetIPv4);
	CFStringRef           pattern2 = SCDynamicStoreKeyCreateNetworkInterfaceEntity(NULL, kSCDynamicStoreDomainState, kSCCompAnyRegex, kSCEntNetIPv6);

	CFMutableArrayRef     keys     = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
	CFMutableArrayRef     patterns = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);

	if (!store) { LogMsg("SCDynamicStoreCreate failed: %s", SCErrorString(SCError())); goto error; }
	if (!key1 || !key2 || !key3 || !key4 || !keys || !pattern1 || !pattern2 || !patterns) goto error;

	CFArrayAppendValue(keys, key1);
	CFArrayAppendValue(keys, key2);
	CFArrayAppendValue(keys, key3);
	CFArrayAppendValue(keys, key4);
	CFArrayAppendValue(keys, key5);
	CFArrayAppendValue(keys, CFSTR("Setup:/Network/DynamicDNS"));
	CFArrayAppendValue(patterns, pattern1);
	CFArrayAppendValue(patterns, pattern2);
	CFArrayAppendValue(patterns, CFSTR("State:/Network/Interface/[^/]+/AirPort"));
	if (!SCDynamicStoreSetNotificationKeys(store, keys, patterns))
		{ LogMsg("SCDynamicStoreSetNotificationKeys failed: %s", SCErrorString(SCError())); goto error; }

	m->p->StoreRLS = SCDynamicStoreCreateRunLoopSource(NULL, store, 0);
	if (!m->p->StoreRLS) { LogMsg("SCDynamicStoreCreateRunLoopSource failed: %s", SCErrorString(SCError())); goto error; }

	CFRunLoopAddSource(CFRunLoopGetCurrent(), m->p->StoreRLS, kCFRunLoopDefaultMode);
	m->p->Store = store;
	err = 0;
	goto exit;

	error:
	if (store)    CFRelease(store);

	exit:
	if (key1)     CFRelease(key1);
	if (key2)     CFRelease(key2);
	if (key3)     CFRelease(key3);
	if (key4)     CFRelease(key4);
	if (key5)     CFRelease(key5);
	if (pattern1) CFRelease(pattern1);
	if (pattern2) CFRelease(pattern2);
	if (keys)     CFRelease(keys);
	if (patterns) CFRelease(patterns);
	
	return(err);
	}
	
mDNSlocal OSStatus StoreDomainDataFromKeychain(mDNS * const m)
	{
	SCDynamicStoreRef    store          = NULL;
	SecKeychainRef       systemKeychain = NULL;
	SecKeychainSearchRef searchRef      = NULL;
	CFMutableArrayRef	 stateArray		= NULL;
	CFStringRef			 key		    = NULL;
	OSStatus             err            = 0;
	
	(void) m;
	
	store = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder:StoreDomainDataFromKeychain"), NULL, NULL);
	
	if (!store)
		{
		LogMsg("SCDynamicStoreCreate failed");
		err = mStatus_UnknownErr;
		goto exit;
		}

	err = SecKeychainOpen(SYSTEM_KEYCHAIN_PATH, &systemKeychain);
	
	if (err)
		{
		LogMsg("SecKeychainOpen failed: %d", err);
		goto exit;
		}

	err = SecKeychainSearchCreateFromAttributes(systemKeychain, kSecGenericPasswordItemClass, NULL, &searchRef);
	
	if (err)
		{
		LogMsg("SecKeychainSearchCreateFromAttributes failed: %d", err);
		goto exit;
		}

	stateArray = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);

	if (!stateArray)
		{
		LogMsg("CFArrayCreateMutable failed");
		err = notEnoughMemoryErr;
		goto exit;
		}

	while (1)
		{
		SecKeychainItemRef itemRef = NULL;

		err = SecKeychainSearchCopyNext(searchRef, &itemRef);
		
		if (err)
			{
			break;
			}
		else
			{
			SecKeychainAttributeInfo attrInfo;
			UInt32 secretlen;
			void *secret = NULL;
			SecKeychainAttributeList *attrList = NULL;
	        UInt32 tags[1];

			tags[0]         = kSecServiceItemAttr;
			attrInfo.count  = 1;
			attrInfo.tag    = tags;
			attrInfo.format = NULL;

			err = SecKeychainItemCopyAttributesAndData(itemRef,  &attrInfo, NULL, &attrList, &secretlen, &secret);
	
			if (err)
				{
				LogMsg("SecKeychainItemCopyAttributesAndData failed: %d", err);
				CFRelease(itemRef);
				continue;
				}

			if (attrList)
				{
				unsigned i;

				for (i = 0; i < attrList->count; i++)
					{
					SecKeychainAttribute attr = attrList->attr[i];
				
					if (attr.tag == kSecServiceItemAttr)
						{
						char keybuf[4 + MAX_ESCAPED_DOMAIN_NAME];  // Extra 4 is for the "dns:" prefix
						memcpy(keybuf, attr.data, attr.length);
						keybuf[attr.length] = 0;
						if (strncasecmp(keybuf, "dns:", 4) == 0)
							{
							CFStringRef cfs = CFStringCreateWithCString(NULL, keybuf+4, kCFStringEncodingUTF8);
							CFArrayAppendValue(stateArray, cfs);
							CFRelease(cfs);
							}
						}
					}
				SecKeychainItemFreeAttributesAndData(attrList, secret);
				}
			}
			
		CFRelease(itemRef);
		}

	key = SCDynamicStoreKeyCreate(NULL, CFSTR("%@/%@/%@"), kSCDynamicStoreDomainState, kSCCompNetwork, CFSTR(kDNSServiceCompPrivateDNS));

	if (!key)
		{
		LogMsg("SCDynamicStoreKeyCreateNetworkGlobalEntity failed");
		err = mStatus_UnknownErr;
		goto exit;
		}

	SCDynamicStoreSetValue(store, key, stateArray);

exit:

	if (key)
		{
		CFRelease(key);
		}

	if (searchRef)
		{
		CFRelease(searchRef);
		}
		
	if (systemKeychain)
		{
		CFRelease(systemKeychain);
		}
		
	if (stateArray)
		{
		CFRelease(stateArray);
		}
		
	if (store)
		{
		CFRelease(store);
		}

	return err;
	}
	
mDNSlocal OSStatus KeychainChanged(SecKeychainEvent keychainEvent, SecKeychainCallbackInfo *info, void *context)
	{
	char         path[1024];
	UInt32       pathLen;
	OSStatus     err;
	mDNS *       m;

	(void) keychainEvent;

	m       = (mDNS*) context;
	pathLen = sizeof(path);
	err     = SecKeychainGetPath(info->keychain, &pathLen, path);

	if (err)
		{
		LogMsg("SecKeychainGetPath failed: %d", err);
		goto exit;
		}
	
	if (strncmp(SYSTEM_KEYCHAIN_PATH, path, pathLen) == 0)
		{
		StoreDomainDataFromKeychain(m);
		}

exit:
	
	return 0;
	}

mDNSlocal mStatus WatchForKeychainChanges(mDNS * const m)
	{
	return SecKeychainAddCallback(KeychainChanged, kSecAddEventMask|kSecDeleteEventMask|kSecUpdateEventMask, m);
	}

#ifndef NO_IOPOWER
mDNSlocal void PowerChanged(void *refcon, io_service_t service, natural_t messageType, void *messageArgument)
	{
	mDNS *const m = (mDNS *const)refcon;
	pthread_mutex_lock(&m->p->BigMutex);
	(void)service;    // Parameter not used
	switch(messageType)
		{
		case kIOMessageCanSystemPowerOff:		debugf      ("PowerChanged kIOMessageCanSystemPowerOff (no action)");				break; // E0000240
		case kIOMessageSystemWillPowerOff:		LogOperation("PowerChanged kIOMessageSystemWillPowerOff");
												mDNSCoreMachineSleep(m, true); mDNSMacOSXNetworkChanged(m);							break; // E0000250
		case kIOMessageSystemWillNotPowerOff:	debugf      ("PowerChanged kIOMessageSystemWillNotPowerOff (no action)");			break; // E0000260
		case kIOMessageCanSystemSleep:			debugf      ("PowerChanged kIOMessageCanSystemSleep (no action)");					break; // E0000270
		case kIOMessageSystemWillSleep:			LogOperation("PowerChanged kIOMessageSystemWillSleep");
												mDNSCoreMachineSleep(m, true); mDNSMacOSXNetworkChanged(m);							break; // E0000280
		case kIOMessageSystemWillNotSleep:		debugf      ("PowerChanged kIOMessageSystemWillNotSleep (no action)");				break; // E0000290
		case kIOMessageSystemHasPoweredOn:		LogOperation("PowerChanged kIOMessageSystemHasPoweredOn");
												// If still sleeping (didn't get 'WillPowerOn' message for some reason?) wake now
												if (m->SleepState) mDNSCoreMachineSleep(m, false);
												// Just to be safe, also make sure our interface list is fully up to date, in case we
												// haven't yet received the System Configuration Framework "network changed" event that
												// we expect to receive some time shortly after the kIOMessageSystemWillPowerOn message
												mDNSMacOSXNetworkChanged(m);														break; // E0000300
		case kIOMessageSystemWillRestart:		debugf      ("PowerChanged kIOMessageSystemWillRestart (no action)");				break; // E0000310
		case kIOMessageSystemWillPowerOn:		LogOperation("PowerChanged kIOMessageSystemWillPowerOn");
												// Make sure our interface list is cleared to the empty state, then tell mDNSCore to wake
												mDNSMacOSXNetworkChanged(m); mDNSCoreMachineSleep(m, false);						break; // E0000320
		default:								LogOperation("PowerChanged unknown message %X", messageType);						break;
		}
	IOAllowPowerChange(m->p->PowerConnection, (long)messageArgument);
	pthread_mutex_unlock(&m->p->BigMutex);
	KQueueWake(m);
	}

mDNSlocal mStatus WatchForPowerChanges(mDNS *const m)
	{
	IONotificationPortRef thePortRef;
	m->p->PowerConnection = IORegisterForSystemPower(m, &thePortRef, PowerChanged, &m->p->PowerNotifier);
	if (m->p->PowerConnection)
		{
		m->p->PowerRLS = IONotificationPortGetRunLoopSource(thePortRef);
		CFRunLoopAddSource(CFRunLoopGetCurrent(), m->p->PowerRLS, kCFRunLoopDefaultMode);
		return(mStatus_NoError);
		}
	return(-1);
	}
#endif /* NO_IOPOWER */

CF_EXPORT CFDictionaryRef _CFCopySystemVersionDictionary(void);
CF_EXPORT const CFStringRef _kCFSystemVersionProductNameKey;
CF_EXPORT const CFStringRef _kCFSystemVersionProductVersionKey;
CF_EXPORT const CFStringRef _kCFSystemVersionBuildVersionKey;

// Major version 6 is 10.2.x (Jaguar)
// Major version 7 is 10.3.x (Panther)
// Major version 8 is 10.4.x (Tiger)
mDNSexport int mDNSMacOSXSystemBuildNumber(char *HINFO_SWstring)
	{
	int major = 0, minor = 0;
	char letter = 0, prodname[256]="<Unknown>", prodvers[256]="<Unknown>", buildver[256]="<Unknown>";
	CFDictionaryRef vers = _CFCopySystemVersionDictionary();
	if (vers)
		{
		CFStringRef cfprodname = CFDictionaryGetValue(vers, _kCFSystemVersionProductNameKey);
		CFStringRef cfprodvers = CFDictionaryGetValue(vers, _kCFSystemVersionProductVersionKey);
		CFStringRef cfbuildver = CFDictionaryGetValue(vers, _kCFSystemVersionBuildVersionKey);
		if (cfprodname) CFStringGetCString(cfprodname, prodname, sizeof(prodname), kCFStringEncodingUTF8);
		if (cfprodvers) CFStringGetCString(cfprodvers, prodvers, sizeof(prodvers), kCFStringEncodingUTF8);
		if (cfbuildver) CFStringGetCString(cfbuildver, buildver, sizeof(buildver), kCFStringEncodingUTF8);
		sscanf(buildver, "%d%c%d", &major, &letter, &minor);
		CFRelease(vers);
		}
	if (!major) { major=8; LogMsg("Note: No Major Build Version number found; assuming 8"); }
	if (HINFO_SWstring) mDNS_snprintf(HINFO_SWstring, 256, "%s %s (%s), %s", prodname, prodvers, buildver, mDNSResponderVersionString);
	return(major);
	}

// Test to see if we're the first client running on UDP port 5353, by trying to bind to 5353 without using SO_REUSEPORT.
// If we fail, someone else got here first. That's not a big problem; we can share the port for multicast responses --
// we just need to be aware that we shouldn't expect to successfully receive unicast UDP responses.
mDNSlocal mDNSBool mDNSPlatformInit_CanReceiveUnicast(void)
	{
	int err = -1;
	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s < 3)
		LogMsg("mDNSPlatformInit_CanReceiveUnicast: socket error %d errno %d (%s)", s, errno, strerror(errno));
	else
		{
		struct sockaddr_in s5353;
		s5353.sin_family      = AF_INET;
		s5353.sin_port        = MulticastDNSPort.NotAnInteger;
		s5353.sin_addr.s_addr = 0;
		err = bind(s, (struct sockaddr *)&s5353, sizeof(s5353));
		close(s);
		}

	if (err) LogMsg("No unicast UDP responses");
	else     debugf("Unicast UDP responses okay");
	return(err == 0);
	}

// Construction of Default Browse domain list (i.e. when clients pass NULL) is as follows:
// 1) query for b._dns-sd._udp.local on LocalOnly interface
//    (.local manually generated via explicit callback)
// 2) for each search domain (from prefs pane), query for b._dns-sd._udp.<searchdomain>.
// 3) for each result from (2), register LocalOnly PTR record b._dns-sd._udp.local. -> <result>
// 4) result above should generate a callback from question in (1).  result added to global list
// 5) global list delivered to client via GetSearchDomainList()
// 6) client calls to enumerate domains now go over LocalOnly interface
//    (!!!KRS may add outgoing interface in addition)

mDNSlocal mStatus mDNSPlatformInit_setup(mDNS *const m)
	{
	mStatus err;

	// In 10.4, mDNSResponder is launched very early in the boot process, while other subsystems are still in the process of starting up.
	// If we can't read the user's preferences, then we sleep a bit and try again, for up to five seconds before we give up.
	int i;
	for (i=0; i<100; i++)
		{
		domainlabel testlabel;
		testlabel.c[0] = 0;
		GetUserSpecifiedLocalHostName(&testlabel);
		if (testlabel.c[0]) break;
		usleep(50000);
		}

	m->hostlabel.c[0]        = 0;
	
	char *HINFO_HWstring = "Macintosh";
	char HINFO_HWstring_buffer[256];
	int    get_model[2] = { CTL_HW, HW_MODEL };
	size_t len_model = sizeof(HINFO_HWstring_buffer);
	if (sysctl(get_model, 2, HINFO_HWstring_buffer, &len_model, NULL, 0) == 0)
		HINFO_HWstring = HINFO_HWstring_buffer;

	char HINFO_SWstring[256] = "";
	if (mDNSMacOSXSystemBuildNumber(HINFO_SWstring) < 7) m->KnownBugs |= mDNS_KnownBug_PhantomInterfaces;
	if (mDNSPlatformInit_CanReceiveUnicast())            m->CanReceiveUnicastOn5353 = mDNStrue;

#ifndef NO_HINFO
	mDNSu32 hlen = mDNSPlatformStrLen(HINFO_HWstring);
	mDNSu32 slen = mDNSPlatformStrLen(HINFO_SWstring);
	if (hlen + slen < 254)
		{
		m->HIHardware.c[0] = hlen;
		m->HISoftware.c[0] = slen;
		mDNSPlatformMemCopy(HINFO_HWstring, &m->HIHardware.c[1], hlen);
		mDNSPlatformMemCopy(HINFO_SWstring, &m->HISoftware.c[1], slen);
		}
#endif /* NO_HINFO */

 	m->p->unicastsockets.m     = m;
	m->p->unicastsockets.info  = NULL;
	m->p->unicastsockets.sktv4 = m->p->unicastsockets.sktv6 = -1;
	m->p->unicastsockets.kqsv4.context  = m->p->unicastsockets.kqsv6.context  = &m->p->unicastsockets;
	m->p->unicastsockets.kqsv4.callback = m->p->unicastsockets.kqsv6.callback = myKQSocketCallBack;
	
	err = SetupSocket(m, &m->p->unicastsockets, mDNSfalse, &zeroAddr, mDNSNULL, AF_INET);
#ifndef NO_IPV6
	err = SetupSocket(m, &m->p->unicastsockets, mDNSfalse, &zeroAddr, mDNSNULL, AF_INET6);
#endif

	struct sockaddr_in s4;
	socklen_t n4 = sizeof(s4);
	if (getsockname(m->p->unicastsockets.sktv4, (struct sockaddr *)&s4, &n4) < 0) LogMsg("getsockname v4 error %d (%s)", errno, strerror(errno));
	else m->UnicastPort4.NotAnInteger = s4.sin_port;
#ifndef NO_IPV6
	struct sockaddr_in6 s6;
	socklen_t n6 = sizeof(s6);
	if (getsockname(m->p->unicastsockets.sktv6, (struct sockaddr *)&s6, &n6) < 0) LogMsg("getsockname v6 error %d (%s)", errno, strerror(errno));
	else m->UnicastPort6.NotAnInteger = s6.sin6_port;
#endif

	m->p->InterfaceList      = mDNSNULL;
	m->p->userhostlabel.c[0] = 0;
	m->p->usernicelabel.c[0] = 0;
	m->p->NotifyUser         = 0;
	mDNSs32 utc = mDNSPlatformUTC();
	UpdateInterfaceList(m, utc);
	SetupActiveInterfaces(m, utc);

	err = WatchForNetworkChanges(m);
	if (err) return(err);
	
	StoreDomainDataFromKeychain(m);
	
	err = WatchForKeychainChanges(m);
	if (err) return(err);
	
#ifndef NO_IOPOWER
	err = WatchForPowerChanges(m);
	if (err) return err;
#endif /* NO_IOPOWER */

	uDNS_SetupDNSConfig(m);						// Get initial DNS configuration

 	return(err);
	}

mDNSexport mStatus mDNSPlatformInit(mDNS *const m)
	{
	mStatus result = mDNSPlatformInit_setup(m);
	
	// We don't do asynchronous initialization on OS X, so by the time we get here the setup will already
	// have succeeded or failed -- so if it succeeded, we should just call mDNSCoreInitComplete() immediately
	if (result == mStatus_NoError) mDNSCoreInitComplete(m, mStatus_NoError);
	return(result);
	}

mDNSexport void mDNSPlatformClose(mDNS *const m)
	{
	if (m->p->PowerConnection)
		{
		CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m->p->PowerRLS, kCFRunLoopDefaultMode);
		CFRunLoopSourceInvalidate(m->p->PowerRLS);
		CFRelease(m->p->PowerRLS);
#ifndef NO_IOPOWER
		IODeregisterForSystemPower(&m->p->PowerNotifier);
#endif /* NO_IOPOWER */
		m->p->PowerConnection = 0;
		m->p->PowerNotifier   = 0;
		m->p->PowerRLS        = NULL;
		}
	
	if (m->p->Store)
		{
		CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m->p->StoreRLS, kCFRunLoopDefaultMode);
		CFRunLoopSourceInvalidate(m->p->StoreRLS);
		CFRelease(m->p->StoreRLS);
		CFRelease(m->p->Store);
		m->p->Store    = NULL;
		m->p->StoreRLS = NULL;
		}
	
	mDNSs32 utc = mDNSPlatformUTC();
	MarkAllInterfacesInactive(m, utc);
	ClearInactiveInterfaces(m, utc);
	CloseSocketSet(&m->p->unicastsockets);
	}

mDNSexport mDNSu32 mDNSPlatformRandomSeed(void)
	{
	return(mach_absolute_time());
	}

mDNSexport mDNSs32 mDNSPlatformOneSecond = 1000;

mDNSexport mStatus mDNSPlatformTimeInit(void)
	{
	// Notes: Typical values for mach_timebase_info:
	// tbi.numer = 1000 million
	// tbi.denom =   33 million
	// These are set such that (mach_absolute_time() * numer/denom) gives us nanoseconds;
	//          numer  / denom = nanoseconds per hardware clock tick (e.g. 30);
	//          denom  / numer = hardware clock ticks per nanosecond (e.g. 0.033)
	// (denom*1000000) / numer = hardware clock ticks per millisecond (e.g. 33333)
	// So: mach_absolute_time() / ((denom*1000000)/numer) = milliseconds
	//
	// Arithmetic notes:
	// tbi.denom is at least 1, and not more than 2^32-1.
	// Therefore (tbi.denom * 1000000) is at least one million, but cannot overflow a uint64_t.
	// tbi.denom is at least 1, and not more than 2^32-1.
	// Therefore clockdivisor should end up being a number roughly in the range 10^3 - 10^9.
	// If clockdivisor is less than 10^3 then that means that the native clock frequency is less than 1MHz,
	// which is unlikely on any current or future Macintosh.
	// If clockdivisor is greater than 10^9 then that means the native clock frequency is greater than 1000GHz.
	// When we ship Macs with clock frequencies above 1000GHz, we may have to update this code.
	struct mach_timebase_info tbi;
	kern_return_t result = mach_timebase_info(&tbi);
	if (result == KERN_SUCCESS) clockdivisor = ((uint64_t)tbi.denom * 1000000) / tbi.numer;
	return(result);
	}

mDNSexport mDNSs32 mDNSPlatformRawTime(void)
	{
	if (clockdivisor == 0) { LogMsg("mDNSPlatformRawTime called before mDNSPlatformTimeInit"); return(0); }

	static uint64_t last_mach_absolute_time = 0;
	//static uint64_t last_mach_absolute_time = 0x8000000000000000LL;	// Use this value for testing the alert display
	uint64_t this_mach_absolute_time = mach_absolute_time();
	if ((int64_t)this_mach_absolute_time - (int64_t)last_mach_absolute_time < 0)
		{
		LogMsg("mDNSPlatformRawTime: last_mach_absolute_time %08X%08X", last_mach_absolute_time);
		LogMsg("mDNSPlatformRawTime: this_mach_absolute_time %08X%08X", this_mach_absolute_time);
		// Update last_mach_absolute_time *before* calling NotifyOfElusiveBug()
		last_mach_absolute_time = this_mach_absolute_time;
		// Only show "mach_absolute_time went backwards" notice on 10.4 (build 8xyyy) or later.
		// (This bug happens all the time on 10.3, and we know that's not going to be fixed.)
		if (mDNSMacOSXSystemBuildNumber(NULL) >= 8)
			NotifyOfElusiveBug("mach_absolute_time went backwards!",
				"This error occurs from time to time, often on newly released hardware, "
				"and usually the exact cause is different in each instance.\r\r"
				"Please file a new Radar bug report with the title “mach_absolute_time went backwards” "
				"and assign it to Radar Component “Kernel” Version “X”.");
		}
	last_mach_absolute_time = this_mach_absolute_time;

	return((mDNSs32)(this_mach_absolute_time / clockdivisor));
	}

mDNSexport mDNSs32 mDNSPlatformUTC(void)
	{
	return time(NULL);
	}

// Locking is a no-op here, because we're single-threaded with a CFRunLoop, so we can never interrupt ourselves
mDNSexport void     mDNSPlatformLock   (const mDNS *const m) { (void)m; }
mDNSexport void     mDNSPlatformUnlock (const mDNS *const m) { (void)m; }
mDNSexport void     mDNSPlatformStrCopy(const void *src,       void *dst)              { strcpy((char *)dst, (char *)src); }
mDNSexport mDNSu32  mDNSPlatformStrLen (const void *src)                               { return(strlen((char*)src)); }
mDNSexport void     mDNSPlatformMemCopy(const void *src,       void *dst, mDNSu32 len) { memcpy(dst, src, len); }
mDNSexport mDNSBool mDNSPlatformMemSame(const void *src, const void *dst, mDNSu32 len) { return(memcmp(dst, src, len) == 0); }
mDNSexport void     mDNSPlatformMemZero(                       void *dst, mDNSu32 len) { bzero(dst, len); }
mDNSexport void *   mDNSPlatformMemAllocate(mDNSu32 len) { return(mallocL("mDNSPlatformMemAllocate", len)); }
mDNSexport void     mDNSPlatformMemFree    (void *mem)   { freeL("mDNSPlatformMemFree", mem); }
