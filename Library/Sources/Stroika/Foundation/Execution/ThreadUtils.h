/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_ThreadUtils_h_
#define	_Stroika_Foundation_Execution_ThreadUtils_h_	1

#include	"../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Configuration/Common.h"

#include	"CopyableCriticalSection.h"
#include	"CriticalSection.h"
#include	"Exceptions.h"
#include	"Event.h"
#include	"IRunnable.h"
#include	"SimpleThread.h"
#include	"Sleep.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_ThreadUtils_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ThreadUtils.inl"
