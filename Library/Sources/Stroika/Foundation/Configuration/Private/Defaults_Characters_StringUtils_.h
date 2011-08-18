/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_Private_Defaults_Chracters_Common_h_
#define	_Stroika_Foundation_Configuration_Private_Defaults_Chracters_Common_h_	1

	/*
	 * See DOCS in Stroika/Foundation/Characters/StringUtils.h
	 */
	#ifndef	qPlatformSupports_SNPRINTF
		#if		__GNUC__ && __cplusplus
			#define	qPlatformSupports_SNPRINTF	1
		#elif	defined (_MSC_VER) && _MSC_VER <= 1600
			#define	qPlatformSupports_SNPRINTF	0
		#elif	defined (_MSC_VER) && _MSC_VER >= 1700
			// GUESS it will be supported in next major release
			#define	qPlatformSupports_SNPRINTF	1
		#else
			// GUESS - if wrong, add appropriate check here
			#define	qPlatformSupports_SNPRINTF	1
		#endif
	#endif

	/*
	 * See DOCS in Stroika/Foundation/Characters/StringUtils.h
	 */
	#ifndef	qPlatformSupports_wcscasecmp
		#if		XOPEN_SOURCE >= 700 || _POSIX_C_SOURCE >= 200809L
			// This test from http://www.kernel.org/doc/man-pages/online/pages/man3/wcscasecmp.3.html
			#define	qPlatformSupports_wcscasecmp	1
		#elif	defined (_MSC_VER)
			#define	qPlatformSupports_wcscasecmp	0
		#else
			// GUESS - if wrong, add appropriate check here
			#define	qPlatformSupports_wcscasecmp	1
		#endif
	#endif

	/*
	 * See DOCS in Stroika/Foundation/Characters/StringUtils.h
	 */
	#ifndef	qPlatformSupports_wcsncasecmp
		#define	qPlatformSupports_wcsncasecmp	qPlatformSupports_wcscasecmp
	#endif

#endif	/*_Stroika_Foundation_Configuration_Private_Defaults_Chracters_Common_h_*/
