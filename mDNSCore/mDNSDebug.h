#ifdef	__cplusplus
	extern "C" {
#endif

#if DEBUGBREAKS
#define debugf debugf_
extern void debugf_(const char *format, ...);
#else // If debug breaks are off, use a preprocessor trick to optimize those calls out of the code
	#if( defined( __GNUC__ ) )
		#define	debugf( ARGS... )
	#elif( defined( __MWERKS__ ) )
		#define	debugf( ... )
	#else
		#define debugf 1 ? ((void) 0) : (void)
	#endif
#endif

#ifdef	__cplusplus
	}
#endif
