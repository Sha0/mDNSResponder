/*
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
 */
#ifndef H_HELPER_H
#define H_HELPER_H

#define kmDNSHelperServiceName "com.apple.mDNSResponder.helper"

enum mDNSDynamicStoreSetConfigKey {
	kmDNSMulticastConfig = 1,
	kmDNSDynamicConfig,
	kmDNSPrivateConfig,
	kmDNSBackToMyMacConfig
};
enum mDNSPreferencesSetNameKey {
	kmDNSComputerName = 1,
	kmDNSLocalHostName
};
enum mDNSAutoTunnelInterfaceUpDown {
	kmDNSAutoTunnelInterfaceUp = 1,
	kmDNSAutoTunnelInterfaceDown
};
enum mDNSAutoTunnelSetKeysReplaceDelete {
	kmDNSAutoTunnelSetKeysReplace = 1,
	kmDNSAutoTunnelSetKeysDelete
};

#define ERROR(x, y) x,
enum mDNSHelperErrors {
	mDNSHelperErrorBase = 2300,
#include "helper-error.h"
	mDNSHelperErrorEnd
};
#undef ERROR

#include "helpermsg-types.h"

extern const char *mDNSHelperError(int errornum);
extern int mDNSPreferencesSetName(int key, CFStringRef value,
    CFStringEncoding encoding);
extern int mDNSDynamicStoreSetConfig(int key, CFPropertyListRef value);
extern int mDNSKeychainGetSecrets(CFArrayRef *secrets);
extern int mDNSCreateStateDir(void);
extern int mDNSAutoTunnelInterfaceUpDown(int updown, v6addr_t addr);
extern int mDNSRacoonNotify(const char *keydata);
extern int mDNSAutoTunnelSetKeys(int replacedelete, v6addr_t local_inner,
    v4addr_t local_outer, short local_port, v6addr_t remote_inner,
    v4addr_t remote_outer, short remote_port, const char *keydata);

#endif /* H_HELPER_H */
