#pragma once

#include <dns_sd.h>
#include <vcclr.h>
#include <memory>

using namespace System;
using namespace System::Net;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;
using namespace System::Collections;


namespace Apple
{
	namespace DNSService
	{
		public __value enum : int
		{
			kDNSServiceFlagsMoreComing			=	1,
			kDNSServiceFlagsFinished			=	0,
			kDNSServiceFlagsAdd					=	2,
			kDNSServiceFlagsDefault				=	4,
			kDNSServiceFlagsRemove				=	0,
			kDNSServiceFlagsNoAutoRename		=	8,
			kDNSServiceFlagsAutoRename			=	0,
			kDNSServiceFlagsShared				=	16,
			kDNSServiceFlagsUnique				=	32,
			kDNSServiceFlagsBrowseDomains		=	64,
			kDNSServiceFlagsRegistrationDomains	=	128
		};


		public __value enum : int
		{
			kDNSServiceErr_NoError				=	0,
			kDNSServiceErr_Unknown				=	-65537,
			kDNSServiceErr_NoSuchName			=	-65538,
			kDNSServiceErr_NoMemory				=	-65539,
			kDNSServiceErr_BadParam				=	-65540,
			kDNSServiceErr_BadReference			=	-65541,
			kDNSServiceErr_BadState				=	-65542,
			kDNSServiceErr_BadFlags				=	-65543,
			kDNSServiceErr_Unsupported			=	-65544,
			kDNSServiceErr_NotInitialized		=	-65545,
			kDNSServiceErr_AlreadyRegistered	=	-65547,
			kDNSServiceErr_NameConflict			=	-65548,
			kDNSServiceErr_Invalid				=	-65549,
			kDNSServiceErr_Incompatible			=	-65551,
			kDNSServiceErr_BadinterfaceIndex	=	-65552

			/*
			 * mDNS Error codes are in the range
			 * FFFE FF00 (-65792) to FFFE FFFF (-65537)
			 */
		};


		public __gc class DNSServiceException
		:
			public Exception
		{
		public:

			DNSServiceException
				(
				int err
				);

			DNSServiceException
				(	
				String		*	message,
				Exception	*	innerException
				);

			int err;
		};


		public __gc class NetworkService
		{
		public:

			NetworkService()
			{
				Port = 0;
			}

			String		*	Name;
			String		*	Type;
			String		*	Domain;
			IPAddress	*	Address;
			short				Port;
			ArrayList	*	ServiceText;

			String*
			ToString()
			{
				return Name;
			}
		};


		public __delegate void
		EnumerateDomainsReply
			(
			DNSServiceFlags		flags,
			uint32_t			interfaceIndex,
			DNSServiceErrorType	errorCode,
			String			*	replyDomain
			);


		public __delegate void
		RegisterReply
			(
			long id
			);


		public __delegate void
		BrowseReply
			(
			long				id
			);


		public __delegate void
		ResolveReply
			(	
			long					id
			);


		public __gc class EnumerateDomains
		{
		public:

			EnumerateDomains
				(
				int							flags,
				unsigned int				interfaceIndex,
				EnumerateDomainsReply	*	callback
				);

			~EnumerateDomains
				(
				);

		private:

			__nogc class EnumerateDomainsImpl
			{
			public:

				EnumerateDomainsImpl
					(
					EnumerateDomains 	*	outer,
					int						flags,
					unsigned int			interfaceIndex
					);

				~EnumerateDomainsImpl
					(
					);

				void
				ProcessingThread();

			private:

				gcroot<EnumerateDomains*>	m_outer;
				DNSServiceRef				m_ref;

				static void
				Callback
					(
					DNSServiceRef			sdRef,
					DNSServiceFlags			flags,
					uint32_t				interfaceIndex,
					DNSServiceErrorType		errorCode,
					const char			*	replyDomain,
					void				*	context
					);
			};

			void
			ProcessingThread();

			void
			Callback
				(
				DNSServiceFlags			flags,
				uint32_t				interfaceIndex,
				DNSServiceErrorType		errorCode,
				String				*	replyDomain
				);

			__event EnumerateDomainsReply 		*	OnEnumerateDomainsReply;
			Thread								*	m_thread;
			EnumerateDomainsImpl				*	m_impl;
		};


		public __gc class Register
		{
		public:

			Register
				(
				int					flags,
				unsigned int		interfaceIndex,
				String			*	name,
				String			*	regtype,
				String			*	domain,
				String			*	host,
				unsigned short		notAnIntPort,
				unsigned short		txtLen,
/*
				void			*	txtRecord,
*/
				RegisterReply	*	callback
				);


			~Register
				(
				);


			void
			AddRecord
				(
				DNSRecordRef		recordRef,
				int					flags,
				unsigned short		rrtype,
				unsigned short		rdlen,
				void			*	rdata,
				unsigned int		ttl
				);


			void
			UpdateRecord
				(
				DNSRecordRef		recordRef,
				int					flags,
				unsigned short		rdlen,
				const void *		rdata,
				unsigned int		ttl
				);

			void
			RemoveRecord
				(
				DNSRecordRef		recordRef,	
				int					flags
				);

		private:

			__nogc class RegisterImpl
			{
			public:

				RegisterImpl
					(
					Register * outer
					);

			private:

				static void
				Callback
					(
					DNSServiceRef			ref,
					DNSServiceFlags			flags,
					DNSServiceErrorType		errorCode,
 					const char			*	name,
					const char			*	regtype,
					const char			*	domain,
					void				*	context
					);

				gcroot<Register*>	m_outer;
				DNSServiceRef		m_ref;
			};

			__event RegisterReply	*	OnRegisterReply;
			Thread					*	m_thread;
			RegisterImpl			*	m_impl;
		};


		public __gc class Browse
		{
		public:

			Browse
				(
				int				flags,
				unsigned int	interfaceIndex,
				String		*	regtype,
				String		*	domain,
				BrowseReply	*	callback
				);

			~Browse
				(
				);

		private:

			__nogc class BrowseImpl
			{
			public:

				BrowseImpl
					(
					Browse * outer
					);

			private:

				static void
				Callback
					(
					DNSServiceRef			sdRef,
   					DNSServiceFlags			flags,
					uint32_t				interfaceIndex,
					DNSServiceErrorType		errorCode,
					const char			*	serviceName,
					const char			*	regtype,
					const char			*	replyDomain,
					void				*	context
					);

				gcroot<Browse*>	m_outer;
				DNSServiceRef	m_ref;
			};

			__event BrowseReply	*	OnBrowseReply;
			Thread				*	m_thread;
			BrowseImpl			*	m_impl;
		};


		public __gc class Resolve
		{
		public:

			Resolve
				(
				int					flags,
				unsigned int		interfaceIndex,
				String			*	name,
				String			*	regtype,
				String			*	domain,
				ResolveReply	*	callback	
				);

			~Resolve
				(
				);

		private:

			__nogc class ResolveImpl
			{
			public:

				ResolveImpl
					(
					Resolve * outer
					);

			private:

				static void
				Callback
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
					);

				gcroot<Resolve*>	m_outer;
				DNSServiceRef		m_ref;
			};


			__event ResolveReply	*	OnResolveReply;
			Thread					*	m_thread;
			ResolveImpl				*	m_impl;
		};


#if 0
		{
		private:

			__nogc class DiscoveryImpl
			{
			public:

				DiscoveryImpl(
							Discovery * outer);

				~DiscoveryImpl();

				static sw_result
				onPublishReply(
							sw_discovery_publish_handler	handler,
							sw_discovery						discovery,
							sw_discovery_publish_status	status,
							sw_discovery_publish_id			id,
							sw_opaque							extra);


				static sw_result
				onBrowseReply(
							sw_discovery_browse_handler	handler,
							sw_discovery						discovery,
							sw_discovery_browse_id			id,
							sw_discovery_browse_status		status,
							sw_const_string					name,
							sw_const_string					type,
							sw_const_string					domain,
							sw_opaque							extra);


				static sw_result
				onResolveReply(
							sw_discovery_resolve_handler	handler,
							sw_discovery						discovery,
							sw_discovery_resolve_id			id,
							sw_const_string					name,
							sw_const_string					type,
							sw_const_string					domain,
							sw_ipv4_address					address,
							sw_port								port,
							sw_const_string					textRecordString,
							sw_octets							textRecord,
							sw_ulong								textRecordLength,
							sw_opaque							extra);


				static sw_result
				onPublishHostReply(
							sw_discovery_publish_host_handler	handler,
							sw_discovery								discovery,
							sw_discovery_publish_status			status,
							sw_discovery_publish_host_id			id,
							sw_opaque									extra);


				void
				Init();

				void
				Fina();

				long
				PublishHost(
							String		*	name,
							String		*	domain,
							IPAddress	*	address);

				void
				StopPublishHost(
							long				id);

				long
				Publish(
							String		*	name,
							String		*	type,
							String		*	domain,
							short				port,
							String		*	host,
							ArrayList	*	serviceText);

				void
				StopPublish(
							long				id);

				long
				BrowseDomains();

				void
				StopBrowseDomains(
							long				id);

				long
				Browse(
							String		*	type,
							String		*	domain);

				void
				StopBrowse(
							long				id);

				long
				Resolve(
							String		*	name,
							String		*	type,
							String		*	domain);

				void
				StopResolve(
							long				id);

				void
				Run();

				void
				Stop();

			private:

				void
				ThrowException(
							sw_result result);
			
				sw_discovery			m_session;
				gcroot<Discovery*>	m_outer;
			};

			Thread			*	m_runThread;
			DiscoveryImpl	*	m_impl;
		};
#endif
	}
}
