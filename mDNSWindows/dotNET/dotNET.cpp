// This is the main DLL file.

#include "stdafx.h"

#include "dotNET.h"

using namespace System::Diagnostics;
using namespace Apple::DNSService;

#if defined(_DEBUG)
#	define check(x) Trace::Assert((bool) (x))
#else
#	define check(x)
#endif


EnumerateDomains::EnumerateDomains
					(
					int							flags,
					unsigned int				interfaceIndex,
					EnumerateDomainsReply	*	callback
					)
:
	m_thread(NULL),
	m_impl(NULL)
{
	OnEnumerateDomainsReply += callback;

	m_impl			=	new EnumerateDomainsImpl(this, flags, interfaceIndex);
    m_thread		=	new Thread(new ThreadStart(this, EnumerateDomains::ProcessingThread));
    m_thread->Name	=	S"EnumerateDomains Thread";
    
	m_thread->Start();
}


EnumerateDomains::~EnumerateDomains()
{
	if (m_thread != NULL)
	{
		if (!m_thread->Join(2000))
		{
			throw new DNSServiceException(6);
		}

		m_thread = NULL;
	}

	delete m_impl;
}


void
EnumerateDomains::ProcessingThread()
{
	m_impl->ProcessingThread();
}


void
EnumerateDomains::Callback
					(
					DNSServiceFlags		flags,
					uint32_t			interfaceIndex,
					DNSServiceErrorType	errorCode,
					String			*	replyDomain
					)
{
	if (OnEnumerateDomainsReply)
	{
		OnEnumerateDomainsReply(flags, interfaceIndex, errorCode, replyDomain);
	}
}


EnumerateDomains::EnumerateDomainsImpl::EnumerateDomainsImpl
											(
											EnumerateDomains	*	outer,
											int						flags,
											unsigned int			interfaceIndex
											)
:
	m_outer(outer),
	m_ref(NULL)
{
	int ret;

	ret = DNSServiceEnumerateDomains(&m_ref, flags, interfaceIndex, (DNSServiceDomainEnumReply) Callback, this);

	if (ret != 0)
	{
		throw new DNSServiceException(ret);
	}
}


EnumerateDomains::EnumerateDomainsImpl::~EnumerateDomainsImpl()
{
	if (m_ref != NULL)
	{
		DNSServiceRefDeallocate(m_ref);
	}
}


void
EnumerateDomains::EnumerateDomainsImpl::ProcessingThread()
{
	DNSServiceErrorType err;

	err = DNSServiceProcessResult(m_ref);
}


void
EnumerateDomains::EnumerateDomainsImpl::Callback
											(
											DNSServiceRef			sdRef,
											DNSServiceFlags			flags,
											uint32_t				interfaceIndex,
											DNSServiceErrorType		errorCode,
											const char			*	replyDomain,
											void				*	context
											)
{
	EnumerateDomains::EnumerateDomainsImpl * impl = static_cast<EnumerateDomains::EnumerateDomainsImpl*>(context);

	check( impl != NULL );

	impl->m_outer->Callback(flags, interfaceIndex, errorCode, replyDomain);
}
