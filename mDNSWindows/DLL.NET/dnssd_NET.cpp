/*
 * Copyright (c) 2003-2004 Apple Computer, Inc. All rights reserved.
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

$Log: dnssd_NET.cpp,v $
Revision 1.1  2004/06/26 04:01:22  shersche
Initial revision


 */
    
// This is the main DLL file.

#include "stdafx.h"

#include "dnssd_NET.h"
#include "DebugServices.h"
#include "PString.h"


using namespace System::Net::Sockets;
using namespace System::Diagnostics;
using namespace Apple;


//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME	"[dnssd.NET] "


//
// class ServiceRef
//
// ServiceRef serves as the base class for all DNSService operations.
//
// It manages the DNSServiceRef, and implements processing the
// result
//
DNSService::ServiceRef::ServiceRef(Object * callback)
:
	m_bDisposed(false),
	m_callback(callback),
	m_thread(NULL)
{
	m_impl = new ServiceRefImpl(this);
}


DNSService::ServiceRef::~ServiceRef()
{
}


//
// StartThread
//
// Starts the main processing thread
//
void
DNSService::ServiceRef::StartThread()
{
	check( m_impl != NULL );

	m_impl->SetupEvents();

	m_thread		=	new Thread(new ThreadStart(this, ProcessingThread));
    m_thread->Name	=	S"DNSService Thread";
	
	m_thread->Start();
}


//
// ProcessingThread
//
// The Thread class can only invoke methods in MC++ types.  So we
// make a ProcessingThread method that forwards to the impl
//
void
DNSService::ServiceRef::ProcessingThread()
{
	m_impl->ProcessingThread();
}


//
// Dispose
//
// Calls impl-Dispose().  This ultimately will call DNSServiceRefDeallocate()
//
void
DNSService::ServiceRef::Dispose()
{
	check(m_impl != NULL);
	check(m_bDisposed == false);

	if (!m_bDisposed)
	{
		m_bDisposed = true;

		//
		// Call Dispose.  This won't call DNSServiceRefDeallocate()
		// necessarily. It depends on what thread this is being
		// called in.
		//
		m_impl->Dispose();
		m_impl = NULL;

		m_thread = NULL;

		GC::SuppressFinalize(this);  
	}
}


//
// EnumerateDomainsDispatch
//
// Dispatch a reply to the delegate.
//
void
DNSService::ServiceRef::EnumerateDomainsDispatch
						(
						ServiceFlags	flags,
						int				interfaceIndex,
						ErrorCode		errorCode,
						String		*	replyDomain
						)
{
	if ((m_callback != NULL) && (m_impl != NULL))
	{
		EnumerateDomainsReply * OnEnumerateDomainsReply = static_cast<EnumerateDomainsReply*>(m_callback);
		OnEnumerateDomainsReply(this, flags, interfaceIndex, errorCode, replyDomain);
	}
}


//
// RegisterDispatch
//
// Dispatch a reply to the delegate.
//
void
DNSService::ServiceRef::RegisterDispatch
				(
				ServiceFlags	flags,
				ErrorCode		errorCode,
 				String		*	name,
				String		*	regtype,
				String		*	domain
				)
{
	if ((m_callback != NULL) && (m_impl != NULL))
	{
		RegisterReply * OnRegisterReply = static_cast<RegisterReply*>(m_callback);
		OnRegisterReply(this, flags, errorCode, name, regtype, domain);
	}
}


//
// BrowseDispatch
//
// Dispatch a reply to the delegate.
//
void
DNSService::ServiceRef::BrowseDispatch
			(
			ServiceFlags	flags,
			int				interfaceIndex,
			ErrorCode		errorCode,
			String		*	serviceName,
			String		*	regtype,
			String		*	replyDomain
			)
{
	if ((m_callback != NULL) && (m_impl != NULL))
	{
		BrowseReply * OnBrowseReply = static_cast<BrowseReply*>(m_callback);
		OnBrowseReply(this, flags, interfaceIndex, errorCode, serviceName, regtype, replyDomain);
	}
}


//
// ResolveDispatch
//
// Dispatch a reply to the delegate.
//
void
DNSService::ServiceRef::ResolveDispatch
			(
			ServiceFlags	flags,
			int				interfaceIndex,
			ErrorCode		errorCode,
			String		*	fullname,
			String		*	hosttarget,
			int				notAnIntPort,
			Byte			txtRecord[]
			)
{
	if ((m_callback != NULL) && (m_impl != NULL))
	{
		ResolveReply * OnResolveReply = static_cast<ResolveReply*>(m_callback);
		OnResolveReply(this, flags, interfaceIndex, errorCode, fullname, hosttarget, notAnIntPort, txtRecord);
	}
}


//
// RegisterRecordDispatch
//
// Dispatch a reply to the delegate.
//
void
DNSService::ServiceRef::RegisterRecordDispatch
				(
				ServiceFlags	flags,
				ErrorCode		errorCode,
				RecordRef	*	record
				)
{
	if ((m_callback != NULL) && (m_impl != NULL))
	{
		RegisterRecordReply * OnRegisterRecordReply = static_cast<RegisterRecordReply*>(m_callback);
		OnRegisterRecordReply(this, flags, errorCode, record);
	}
}


//
// QueryRecordDispatch
//
// Dispatch a reply to the delegate.
//
void
DNSService::ServiceRef::QueryRecordDispatch
					(
					ServiceFlags	flags,
					int				interfaceIndex,
					ErrorCode		errorCode,
					String		*	fullname,
					int				rrtype,
					int				rrclass,
					Byte			rdata[],
					int				ttl
					)
{
	if ((m_callback != NULL) && (m_impl != NULL))
	{
		QueryRecordReply * OnQueryRecordReply = static_cast<QueryRecordReply*>(m_callback);
		OnQueryRecordReply(this, flags, interfaceIndex, errorCode, fullname, rrtype, rrclass, rdata, ttl);
	}
}


//
// ServiceRefImpl::ServiceRefImpl()
//
// Constructs a new ServiceRefImpl.  We save the pointer to our enclosing
// class in a gcroot handle.  This satisfies the garbage collector as
// the outer class is a managed type
//
DNSService::ServiceRef::ServiceRefImpl::ServiceRefImpl(ServiceRef * outer)
:
	m_socketEvent(NULL),
	m_stopEvent(NULL),
	m_disposed(false),
	m_outer(outer),
	m_ref(NULL)
{
	m_threadId = GetCurrentThreadId();
}


//
// ServiceRefImpl::~ServiceRefImpl()
//
// Deallocate all resources associated with the ServiceRefImpl
//
DNSService::ServiceRef::ServiceRefImpl::~ServiceRefImpl()
{
	if (m_socketEvent != NULL)
	{
		CloseHandle(m_socketEvent);
		m_socketEvent = NULL;
	}

	if (m_stopEvent != NULL)
	{
		CloseHandle(m_stopEvent);
		m_stopEvent = NULL;
	}

	if (m_ref != NULL)
	{
		DNSServiceRefDeallocate(m_ref);
		m_ref = NULL;
	}
}


//
// ServiceRefImpl::SetupEvents()
//
// Setup the events necessary to manage multi-threaded dispatch
// of DNSService Events
//
void
DNSService::ServiceRef::ServiceRefImpl::SetupEvents()
{
	check(m_ref != NULL);

	m_socket		=	(SOCKET) DNSServiceRefSockFD(m_ref);
	check(m_socket != INVALID_SOCKET);

	m_socketEvent	=	CreateEvent(NULL, 0, 0, NULL);

	if (m_socketEvent == NULL)
	{
		throw new DNSServiceException(Unknown);
	}

	int err = WSAEventSelect(m_socket, m_socketEvent, FD_READ|FD_CLOSE);

	if (err != 0)
	{
		throw new DNSServiceException(Unknown);
	}

	m_stopEvent = CreateEvent(NULL, 0, 0, NULL);

	if (m_stopEvent == NULL)
	{
		throw new DNSServiceException(Unknown);
	}
}


//
// ServiceRefImpl::ProcessingThread()
//
// Wait for socket events on the DNSServiceRefSockFD().  Also wait
// for stop events
//
void
DNSService::ServiceRef::ServiceRefImpl::ProcessingThread()
{
	check( m_socketEvent != NULL );
	check( m_stopEvent != NULL );
	check( m_ref != NULL );
	
	HANDLE handles[2];

	handles[0] = m_socketEvent;
	handles[1] = m_stopEvent;

	while (m_disposed == false)
	{
		int ret = WaitForMultipleObjects(2, handles, FALSE, INFINITE);

		//
		// it's a socket event
		//
		if (ret == WAIT_OBJECT_0)
		{
			DNSServiceProcessResult(m_ref);
		}
		//
		// else it's a stop event
		//
		else if (ret == WAIT_OBJECT_0 + 1)
		{
			break;
		}
		else
		{
			//
			// unexpected wait result
			//
			dlog( kDebugLevelWarning, DEBUG_NAME "%s: unexpected wait result (result=0x%08X)\n", __ROUTINE__, ret );
		}
	}

	delete this;
}


//
// ServiceRefImpl::Dispose()
//
// Calls DNSServiceRefDeallocate()
//
void
DNSService::ServiceRef::ServiceRefImpl::Dispose()
{
	OSStatus	err;
	BOOL		ok;

	check(m_disposed == false);

	m_disposed = true;

	ok = SetEvent(m_stopEvent);
	err = translate_errno( ok, (OSStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );

exit:

	return;
}


//
// ServiceRefImpl::EnumerateDomainsCallback()
//
// This is the callback from dnssd.dll.  We pass this up to our outer, managed type
//
void DNSSD_API
DNSService::ServiceRef::ServiceRefImpl::EnumerateDomainsCallback
											(
											DNSServiceRef			sdRef,
											DNSServiceFlags			flags,
											uint32_t				interfaceIndex,
											DNSServiceErrorType		errorCode,
											const char			*	replyDomain,
											void				*	context
											)
{
	ServiceRef::ServiceRefImpl * self = static_cast<ServiceRef::ServiceRefImpl*>(context);

	check( self != NULL );
	check( self->m_outer != NULL );

	if (self->m_disposed == false)
	{
		self->m_outer->EnumerateDomainsDispatch((ServiceFlags) flags, interfaceIndex, (ErrorCode) errorCode, replyDomain);
	}
}


//
// ServiceRefImpl::RegisterCallback()
//
// This is the callback from dnssd.dll.  We pass this up to our outer, managed type
//
void DNSSD_API
DNSService::ServiceRef::ServiceRefImpl::RegisterCallback
							(
							DNSServiceRef			sdRef,
							DNSServiceFlags			flags,
							DNSServiceErrorType		errorCode,
							const char			*	name,
							const char			*	regtype,
							const char			*	domain,
							void				*	context
							)
{
	ServiceRef::ServiceRefImpl * self = static_cast<ServiceRef::ServiceRefImpl*>(context);

	check( self != NULL );
	check( self->m_outer != NULL );
	
	if (self->m_disposed == false)
	{
		self->m_outer->RegisterDispatch((ServiceFlags) flags, (ErrorCode) errorCode, name, regtype, domain);
	}
}


//
// ServiceRefImpl::BrowseCallback()
//
// This is the callback from dnssd.dll.  We pass this up to our outer, managed type
//
void DNSSD_API
DNSService::ServiceRef::ServiceRefImpl::BrowseCallback
							(
							DNSServiceRef			sdRef,
   							DNSServiceFlags			flags,
							uint32_t				interfaceIndex,
							DNSServiceErrorType		errorCode,
							const char			*	serviceName,
							const char			*	regtype,
							const char			*	replyDomain,
							void				*	context
							)
{
	ServiceRef::ServiceRefImpl * self = static_cast<ServiceRef::ServiceRefImpl*>(context);

	check( self != NULL );
	check( self->m_outer != NULL );
	
	if (self->m_disposed == false)
	{
		self->m_outer->BrowseDispatch((ServiceFlags) flags, interfaceIndex, (ErrorCode) errorCode, serviceName, regtype, replyDomain);
	}
}


//
// ServiceRefImpl::ResolveCallback()
//
// This is the callback from dnssd.dll.  We pass this up to our outer, managed type
//
void DNSSD_API
DNSService::ServiceRef::ServiceRefImpl::ResolveCallback
							(
							DNSServiceRef			sdRef,
							DNSServiceFlags			flags,
							uint32_t				interfaceIndex,
							DNSServiceErrorType		errorCode,
							const char			*	fullname,
							const char			*	hosttarget,
							uint16_t				notAnIntPort,
							uint16_t				txtLen,
							const char			*	txtRecord,
							void				*	context
							)
{
	ServiceRef::ServiceRefImpl * self = static_cast<ServiceRef::ServiceRefImpl*>(context);

	check( self != NULL );
	check( self->m_outer != NULL );
	
	if (self->m_disposed == false)
	{
		Byte txtRecordBytes[];

		txtRecordBytes = NULL;

		if (txtLen > 0)
		{
			//
			// copy raw memory into managed byte array
			//
			txtRecordBytes		=	new Byte[txtLen];
			Byte __pin	*	p	=	&txtRecordBytes[0];
			memcpy(p, txtRecord, txtLen);
		}

		self->m_outer->ResolveDispatch((ServiceFlags) flags, interfaceIndex, (ErrorCode) errorCode, fullname, hosttarget, notAnIntPort, txtRecordBytes);
	}	
}


//
// ServiceRefImpl::RegisterRecordCallback()
//
// This is the callback from dnssd.dll.  We pass this up to our outer, managed type
//
void DNSSD_API
DNSService::ServiceRef::ServiceRefImpl::RegisterRecordCallback
								(
								DNSServiceRef		sdRef,
								DNSRecordRef		rrRef,
								DNSServiceFlags		flags,
								DNSServiceErrorType	errorCode,
								void			*	context
								)
{
	ServiceRef::ServiceRefImpl * self = static_cast<ServiceRef::ServiceRefImpl*>(context);

	check( self != NULL );
	check( self->m_outer != NULL );
	
	if (self->m_disposed == false)
	{
		RecordRef * record = NULL;

		if (errorCode == 0)
		{
			record = new RecordRef;

			record->m_impl->m_ref = rrRef;
		}

		self->m_outer->RegisterRecordDispatch((ServiceFlags) flags, (ErrorCode) errorCode, record);
	}
}


//
// ServiceRefImpl::QueryRecordCallback()
//
// This is the callback from dnssd.dll.  We pass this up to our outer, managed type
//
void DNSSD_API
DNSService::ServiceRef::ServiceRefImpl::QueryRecordCallback
								(
								DNSServiceRef			DNSServiceRef,
								DNSServiceFlags			flags,
								uint32_t				interfaceIndex,
								DNSServiceErrorType		errorCode,
								const char			*	fullname,
								uint16_t				rrtype,
								uint16_t				rrclass,
								uint16_t				rdlen,
								const void			*	rdata,
								uint32_t				ttl,
								void				*	context
								)
{
	ServiceRef::ServiceRefImpl * self = static_cast<ServiceRef::ServiceRefImpl*>(context);

	check( self != NULL );
	check( self->m_outer != NULL );
	
	if (self->m_disposed == false)
	{
		Byte rdataBytes[];

		if (rdlen)
		{
			rdataBytes			=	new Byte[rdlen];
			Byte __pin * p		=	&rdataBytes[0];
			memcpy(p, rdata, rdlen);
		}

		self->m_outer->QueryRecordDispatch((ServiceFlags) flags, (int) interfaceIndex, (ErrorCode) errorCode, fullname, rrtype, rrclass, rdataBytes, ttl);
	}
}


/*
 * EnumerateDomains()
 *
 * This maps to DNSServiceEnumerateDomains().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */
DNSService::ServiceRef*
DNSService::EnumerateDomains
		(
		int							flags,
		int							interfaceIndex,
		EnumerateDomainsReply	*	callback
		)
{
	ServiceRef * ref = new ServiceRef(callback);
	int			 err;

	err = DNSServiceEnumerateDomains(&ref->m_impl->m_ref, flags, interfaceIndex, ServiceRef::ServiceRefImpl::EnumerateDomainsCallback, ref->m_impl);

	if (err != 0)
	{
		throw new DNSServiceException(err);
	}

	ref->StartThread();

	return ref;
}


/*
 * Register()
 *
 * This maps to DNSServiceRegister().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */
DNSService::ServiceRef*
DNSService::Register
				(
				int					flags,
				int					interfaceIndex,
				String			*	name,
				String			*	regtype,
				String			*	domain,
				String			*	host,
				int					notAnIntPort,
				Byte				txtRecord[],
				RegisterReply	*	callback
				)
{
	ServiceRef * ref = new ServiceRef(callback);

	PString		*	pName	= new PString(name);
	PString		*	pType	= new PString(regtype);
	PString		*	pDomain	= new PString(domain);
	PString		*	pHost	= new PString(host);
	int				len		= txtRecord->Length;
	Byte __pin	*	p		= &txtRecord[0];
	void		*	v		= (void*) p;

	int err = DNSServiceRegister(&ref->m_impl->m_ref, flags, interfaceIndex, pName->c_str(), pType->c_str(), pDomain->c_str(), pHost->c_str(), notAnIntPort, len, v, ServiceRef::ServiceRefImpl::RegisterCallback, ref->m_impl );

	if (err != 0)
	{
		throw new DNSServiceException(err);
	}

	ref->StartThread();

	return ref;
}


/*
 * AddRecord()
 *
 * This maps to DNSServiceAddRecord().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */
DNSService::RecordRef*
DNSService::AddRecord
				(
				ServiceRef	*	ref,
				int				flags,
				int				rrtype,
				Byte			rdata[],
				int				ttl
				)
{
	Byte __pin	*	p = &rdata[0];
	void		*	v = (void*) p;

	RecordRef * record = new RecordRef;

	int err = DNSServiceAddRecord(ref->m_impl->m_ref, &record->m_impl->m_ref, flags, rrtype, rdata->Length, v, ttl);

	if (err != 0)
	{
		throw new DNSServiceException(err);
	}

	return record;
}


/*
 * UpdateRecord()
 *
 * This maps to DNSServiceUpdateRecord().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */
void
DNSService::UpdateRecord
				(
				ServiceRef	*	ref,
				RecordRef	*	record,
				int				flags,
				Byte			rdata[],
				int				ttl
				)
{
	Byte __pin	* p = &rdata[0];
	void		* v = (void*) p;

	int err = DNSServiceUpdateRecord(ref->m_impl->m_ref, record->m_impl->m_ref, flags, rdata->Length, v, ttl);

	if (err != 0)
	{
		throw new DNSServiceException(err);
	}
}


/*
 * RemoveRecord()
 *
 * This maps to DNSServiceRemoveRecord().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */
void
DNSService::RemoveRecord
		(
		ServiceRef	*	ref,
		RecordRef	*	record,
		int				flags
		)
{
	int err = DNSServiceRemoveRecord(ref->m_impl->m_ref, record->m_impl->m_ref, flags);

	if (err != 0)
	{
		throw new DNSServiceException(err);
	}
}	


/*
 * Browse()
 *
 * This maps to DNSServiceBrowse().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */
DNSService::ServiceRef*
DNSService::Browse
	(
	int				flags,
	int				interfaceIndex,
	String		*	regtype,
	String		*	domain,
	BrowseReply	*	callback
	)
{
	ServiceRef	*	ref		= new ServiceRef(callback);
	PString		*	pType	= new PString(regtype);
	PString		*	pDomain	= new PString(domain);

	int err = DNSServiceBrowse(&ref->m_impl->m_ref, flags, interfaceIndex, pType->c_str(), pDomain->c_str(),(DNSServiceBrowseReply) ServiceRef::ServiceRefImpl::BrowseCallback, ref->m_impl);

	if (err != 0)
	{
		throw new DNSServiceException(err);
	}

	ref->StartThread();

	return ref;
}


/*
 * Resolve()
 *
 * This maps to DNSServiceResolve().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */
DNSService::ServiceRef*
DNSService::Resolve
	(
	int					flags,
	int					interfaceIndex,
	String			*	name,
	String			*	regtype,
	String			*	domain,
	ResolveReply	*	callback	
	)
{
	ServiceRef	*	ref		= new ServiceRef(callback);
	PString		*	pName	= new PString(name);
	PString		*	pType	= new PString(regtype);
	PString		*	pDomain	= new PString(domain);

	int err = DNSServiceResolve(&ref->m_impl->m_ref, flags, interfaceIndex, pName->c_str(), pType->c_str(), pDomain->c_str(),(DNSServiceResolveReply) ServiceRef::ServiceRefImpl::ResolveCallback, ref->m_impl);

	if (err != 0)
	{
		throw new DNSServiceException(err);
	}

	ref->StartThread();

	return ref;
}


/*
 * CreateConnection()
 *
 * This maps to DNSServiceCreateConnection().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */
DNSService::ServiceRef*
DNSService::CreateConnection
			(
			RegisterRecordReply * callback
			)
{
	ServiceRef * ref = new ServiceRef(callback);

	int err = DNSServiceCreateConnection(&ref->m_impl->m_ref);

	if (err != 0)
	{
		throw new DNSServiceException(err);
	}

	ref->StartThread();

	return ref;
}


/*
 * RegisterRecord()
 *
 * This maps to DNSServiceRegisterRecord().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */

DNSService::RecordRef*
DNSService::RegisterRecord
			(
			ServiceRef			*	sdRef,
			ServiceFlags			flags,
			int						interfaceIndex,
			String				*	fullname,
			int						rrtype,
			int						rrclass,
			Byte					rdata[],
			int						ttl
			)
{
	RecordRef	*	record	= new RecordRef;
	Byte __pin	*	p		= &rdata[0];
	void		*	v		= (void*) p;

	PString * pFullname = new PString(fullname);

	int err = DNSServiceRegisterRecord(sdRef->m_impl->m_ref, &record->m_impl->m_ref, flags, interfaceIndex, pFullname->c_str(), rrtype, rrclass, rdata->Length, v, ttl, (DNSServiceRegisterRecordReply) ServiceRef::ServiceRefImpl::RegisterRecordCallback, sdRef->m_impl);

	if (err != 0)
	{
		throw new DNSServiceException(err);
	}

	return record;
}

/*
 * QueryRecord()
 *
 * This maps to DNSServiceQueryRecord().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */
DNSService::ServiceRef*
DNSService::QueryRecord
		(
		ServiceFlags			flags,
		int						interfaceIndex,
		String				*	fullname,
		int						rrtype,
		int						rrclass,
		QueryRecordReply	*	callback
		)
{
	ServiceRef	*	ref			= new ServiceRef(callback);
	PString		*	pFullname	= new PString(fullname);

	int err = DNSServiceQueryRecord(&ref->m_impl->m_ref, flags, interfaceIndex, pFullname->c_str(), rrtype, rrclass, (DNSServiceQueryRecordReply) ServiceRef::ServiceRefImpl::QueryRecordCallback, ref->m_impl);

	if (err != 0)
	{
		throw new DNSServiceException(err);
	}

	ref->StartThread();

	return ref;
}


/*
 * ReconfirmRecord()
 *
 * This maps to DNSServiceReconfirmRecord().  Returns an
 * initialized ServiceRef on success, throws an exception
 * on failure.
 */
void
DNSService::ReconfirmRecord
		(
		ServiceFlags	flags,
		int				interfaceIndex,
		String		*	fullname,
		int				rrtype,
		int				rrclass,
		Byte			rdata[]
		)
{
	Byte __pin	*	p	= &rdata[0];
	void		*	v	= (void*) p;

	PString * pFullname = new PString(fullname);

	DNSServiceReconfirmRecord(flags, interfaceIndex, pFullname->c_str(), rrtype, rrclass, rdata->Length, v);
}


//
// DNSServiceException::DNSServiceException()
//
// Constructs an exception with an error code
//
DNSService::DNSServiceException::DNSServiceException
				(
				int _err
				)
:
	err(_err)
{
}


//
// This version of the constructor is useful for instances in which
// an inner exception is thrown, caught, and then a new exception
// is thrown in it's place
//
DNSService::DNSServiceException::DNSServiceException
				(	
				String				*	message,
				System::Exception	*	innerException
				)
{
}
