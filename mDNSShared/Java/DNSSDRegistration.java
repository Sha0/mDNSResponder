/*
 * Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
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

    Change History (most recent first):

$Log: DNSSDRegistration.java,v $
Revision 1.1  2004/04/30 16:32:34  rpantos
First checked in.


	This file declares the public interface to DNSSDRegistration, a DNSSDService
	subclass that allows a client to control a service registration.
 */


package	com.apple.dnssd;


/**	A tracking object for a registration created by {@link DNSSD#register}. */

public interface	DNSSDRegistration extends DNSSDService
{
	/** Add a record to a registered service.<P> 
		The name of the record will be the same as the registered service's name.<P>
		The record can later be updated or deregistered by passing the DNSRecord returned 
		by this function to updateRecord() or removeRecord().<P>

		@param	flags
					Currently unused, reserved for future use.
		<P>
		@param	rrType
					The type of the record (e.g. TXT, SRV, etc), as defined in nameser.h.
		<P>
		@param	rData
					The raw rdata to be contained in the added resource record.
		<P>
		@param	ttl
					The time to live of the resource record, in seconds.
		<P>
		@return		A {@link DNSRecord} that may be passed to updateRecord() or removeRecord(). 
					If {@link DNSSDRegistration#stop} is called, the DNSRecord is also 
					invalidated and may not be used further.
	*/
	DNSRecord		addRecord( int flags, int rrType, byte[] rData, int ttl)
	throws DNSSDException;

	/** Update a registered resource record.<P> 
		The record must either be the primary txt record of a service registered via DNSSD.register(), 
		or a record added to a registered service via addRecord().<P>

		@param	record
					A DNSRecord initialized by addRecord(), or null to update the
					service's primary txt record.
		<P>
		@param	flags
					Currently unused, reserved for future use.
		<P>
		@param	rData
					The new rdata to be contained in the updated resource record.
		<P>
		@param	ttl
					The time to live of the updated resource record, in seconds.
	*/
	void			updateRecord( DNSRecord record, int flags, byte[] rData, int ttl)
	throws DNSSDException;

	/** Remove a registered resource record.<P> 
		The record must have been previously added to a service record set via via addRecord().<P>

		@param	record
					A DNSRecord initialized by addRecord().
		<P>
		@param	flags
					Currently unused, reserved for future use.
	*/
	void			removeRecord( DNSRecord record, int flags)
	throws DNSSDException;
} 

