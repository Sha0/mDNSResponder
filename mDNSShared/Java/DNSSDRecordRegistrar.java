/*
 * Copyright (c) 2006 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@

	This file declares the public interface to DNSSDRecordRegistrar, a DNSSDService
	subclass that allows efficient registration of multiple individual records.

    Change History (most recent first):

$Log: DNSSDRecordRegistrar.java,v $
Revision 1.1  2006/06/20 23:00:12  rpantos
<rdar://problem/3839132> Java needs to implement DNSServiceRegisterRecord equivalent

 */


package	com.apple.dnssd;


/**	An object for registering records, created by {@link DNSSD#createRecordRegistrar}. */

public interface	DNSSDRecordRegistrar extends DNSSDService
{
	/** Register an independent {@link DNSRecord}.<P> 
		@param	flags
					Possible values are SHARED or UNIQUE (see flag type definitions for details).
		<P>
		@param	ifIndex
					If non-zero, specifies the interface on which to register the record
					(the index for a given interface is determined via the if_nametoindex()
					family of calls.)  Passing 0 causes the record to be registered on all interfaces.
		<P>
		@param	fullname
					The full domain name of the resource record.
		<P>
		@param	rrtype
					The numerical type of the resource record to be queried for (e.g. PTR, SRV, etc)
					as defined in nameser.h.
		<P>
		@param	rrclass
					The class of the resource record, as defined in nameser.h 
					(usually 1 for the Internet class).
		<P>
		@param	rData
					The new rdata  as it is to appear in the DNS record.
		<P>
		@param	ttl
					The time to live of the resource record, in seconds. Pass 0 to use a default value.
		<P>
		@param	listener
					This object will get called when the service is registered.
		<P>
		@return		A {@link DNSSDService} that can be used to abort the record registration.

		@throws SecurityException If a security manager is present and denies <tt>RuntimePermission("getDNSSDInstance")</tt>.
		@see    RuntimePermission
	*/
	public DNSRecord	registerRecord( int flags, int ifIndex, String fullname, int rrtype, 
									int rrclass, byte[] rdata, int ttl)
	throws DNSSDException;
} 

