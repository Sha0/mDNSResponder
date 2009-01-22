/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2007 Apple Inc. All rights reserved.
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

$Log: helper.h,v $
Revision 1.14  2009/01/22 02:14:27  cheshire
<rdar://problem/6515626> Sleep Proxy: Set correct target MAC address, instead of all zeroes

Revision 1.13  2009/01/14 01:38:43  mcguire
<rdar://problem/6492710> Write out DynamicStore per-interface SleepProxyServer info

Revision 1.12  2009/01/12 22:26:13  mkrochma
Change DynamicStore location from BonjourSleepProxy/DiscoveredServers to SleepProxyServers

Revision 1.11  2008/12/05 02:35:24  mcguire
<rdar://problem/6107390> Write to the DynamicStore when a Sleep Proxy server is available on the network

Revision 1.10  2008/11/04 23:54:09  cheshire
Added routine mDNSSetARP(), used to replace an SPS client's entry in our ARP cache with
a dummy one, so that IP traffic to the SPS client initiated by the SPS machine can be
captured by our BPF filters, and used as a trigger to wake the sleeping machine.

Revision 1.9  2008/10/24 01:42:36  cheshire
Added mDNSPowerRequest helper routine to request a scheduled wakeup some time in the future

Revision 1.8  2008/10/20 22:01:28  cheshire
Made new Mach simpleroutine "mDNSRequestBPF"

Revision 1.7  2008/09/27 00:58:11  cheshire
Added mDNSRequestBPF declaration

Revision 1.6  2007/09/20 22:33:17  cheshire
Tidied up inconsistent and error-prone naming -- used to be mDNSResponderHelper in
some places and mDNSResponder.helper in others; now mDNSResponderHelper everywhere

Revision 1.5  2007/09/07 22:44:03  mcguire
<rdar://problem/5448420> Move CFUserNotification code to mDNSResponderHelper

Revision 1.4  2007/09/04 22:32:58  mcguire
<rdar://problem/5453633> BTMM: BTMM overwrites /etc/racoon/remote/anonymous.conf

Revision 1.3  2007/08/23 21:51:44  cheshire
Made code layout style consistent with existing project style; added $Log header

Revision 1.1  2007/08/08 22:34:58  mcguire
<rdar://problem/5197869> Security: Run mDNSResponder as user id mdnsresponder instead of root
 */

#ifndef H_HELPER_H
#define H_HELPER_H

#define kmDNSHelperServiceName "com.apple.mDNSResponderHelper"

enum mDNSDynamicStoreSetConfigKey
	{
	kmDNSMulticastConfig = 1,
	kmDNSDynamicConfig,
	kmDNSPrivateConfig,
	kmDNSBackToMyMacConfig,
	kmDNSSleepProxyServersState
	};

enum mDNSPreferencesSetNameKey
	{
	kmDNSComputerName = 1,
	kmDNSLocalHostName
	};

enum mDNSUpDown
	{
	kmDNSUp = 1,
	kmDNSDown
	};

enum mDNSAutoTunnelSetKeysReplaceDelete
	{
	kmDNSAutoTunnelSetKeysReplace = 1,
	kmDNSAutoTunnelSetKeysDelete
	};

#define ERROR(x, y) x,
enum mDNSHelperErrors
	{
	mDNSHelperErrorBase = 2300,
	#include "helper-error.h"
	mDNSHelperErrorEnd
	};
#undef ERROR

#include "mDNSEmbeddedAPI.h"
#include "helpermsg-types.h"

extern const char *mDNSHelperError(int errornum);

extern void mDNSRequestBPF(void);
extern int  mDNSPowerRequest(int key, int interval);
extern int  mDNSSetARP(int ifindex, const v4addr_t ip, const ethaddr_t eth);
extern int  mDNSDynamicStoreSetConfig(int key, const char *subkey, CFPropertyListRef value);
extern int  mDNSPreferencesSetName(int key, domainlabel* old, domainlabel* new);
extern int  mDNSKeychainGetSecrets(CFArrayRef *secrets);
extern int  mDNSAutoTunnelInterfaceUpDown(int updown, v6addr_t addr);
extern int  mDNSConfigureServer(int updown, const char *keydata);
extern int  mDNSAutoTunnelSetKeys(int replacedelete, v6addr_t local_inner,
				v4addr_t local_outer, short local_port, v6addr_t remote_inner,
				v4addr_t remote_outer, short remote_port, const char *keydata);

#endif /* H_HELPER_H */
